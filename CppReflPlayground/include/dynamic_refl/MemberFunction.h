//
// Created by qianq on 12/30/2025.
//

#pragma once
#include <string>
#include <vector>
#include <functional>

#include "Type.h"
#include "../static_refl/function_traits.h"
#include "../static_refl/type_list.h"

namespace my_reflect::dynamic_refl {

    // Forward declarations
    template <typename T>
    const Type* GetType();
    class Any;

    class MemberFunction {
    public:
        std::string name_;
        const Type* retType_;
        std::vector<const Type*> argTypes_;

        // Type-erased function invoker: takes instance and args, returns result
        std::function<Any(Any&, const std::vector<Any>&)> invoker_;

        MemberFunction(std::string name, const Type* retType, std::vector<const Type*> argTypes,
                      std::function<Any(Any&, const std::vector<Any>&)> invoker);
        MemberFunction(MemberFunction&& other) noexcept;

        // Invoke the member function through Any
        Any Invoke(Any& instance, const std::vector<Any>& args) const;

        template <typename FuncPtr>
        static MemberFunction Create(const std::string& name, FuncPtr funcPtr) {
            using Traits = my_reflect::static_refl::function_traits<FuncPtr>;
            using RetType = typename Traits::return_type;
            using ArgTypes = typename Traits::parameter_type;
            using ClassType = typename Traits::class_type;

            // Create type-erased invoker
            auto invoker = [funcPtr](Any& instance, const std::vector<Any>& args) -> Any {
                return InvokeImpl<FuncPtr, ArgTypes>(funcPtr, instance, args, std::make_index_sequence<ArgTypes::size>{});
            };

            return MemberFunction{name, GetType<RetType>(), TypeListToVector<ArgTypes>(), std::move(invoker)};
        }
    private:
        template <typename TypeList>
        static std::vector<const Type*> TypeListToVector() {
            return TypeListToVectorImpl<TypeList>(std::make_index_sequence<TypeList::size>{});
        }

        template <typename TypeList, size_t... Is>
        static std::vector<const Type*> TypeListToVectorImpl(std::index_sequence<Is...>) {
            return {GetType<static_refl::get_t<TypeList, Is>>()...};
        }

        // Helper to extract a single argument
        template <typename ArgType>
        static decltype(auto) ExtractArg(const Any& arg) {
            using PlainType = std::remove_cv_t<std::remove_reference_t<ArgType>>;
            const PlainType* ptr = any_cast<PlainType>(arg);
            if (!ptr) {
                throw std::runtime_error("Failed to cast argument to correct type");
            }
            return *ptr;
        }

        // Implementation for invoking member functions
        template <typename FuncPtr, typename ArgTypes, size_t... Is>
        static Any InvokeImpl(FuncPtr funcPtr, Any& instance, const std::vector<Any>& args, std::index_sequence<Is...>) {
            using Traits = my_reflect::static_refl::function_traits<FuncPtr>;
            using ClassType = typename Traits::class_type;
            using RetType = typename Traits::return_type;

            // Get the instance pointer
            ClassType* inst = any_cast<ClassType>(instance);
            if (!inst) {
                throw std::runtime_error("Failed to cast instance to correct type");
            }

            // Check argument count
            if (args.size() != ArgTypes::size) {
                throw std::runtime_error("Argument count mismatch");
            }

            // Extract arguments and invoke
            if constexpr (std::is_void_v<RetType>) {
                (inst->*funcPtr)(ExtractArg<static_refl::get_t<ArgTypes, Is>>(args[Is])...);
                return Any{}; // Return empty Any for void functions
            } else {
                RetType result = (inst->*funcPtr)(ExtractArg<static_refl::get_t<ArgTypes, Is>>(args[Is])...);
                return make_copy(result);
            }
        }
    };

}

// Include Any.h at the end for template implementations
#include "Any.h"

