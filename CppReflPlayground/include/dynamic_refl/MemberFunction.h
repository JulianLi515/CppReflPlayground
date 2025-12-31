//
// Created by qianq on 12/30/2025.
//

#pragma once
#include <string>
#include <vector>

#include "Type.h"
#include "../static_refl/function_traits.h"
#include "../static_refl/type_list.h"

namespace my_reflect::dynamic_refl {

    // Forward declaration
    template <typename T>
    const Type* GetType();
    class MemberFunction {
    public:
        std::string name_;
        const Type* retType_;
        const std::vector<const Type*> argTypes_;

        template <typename T>
        static MemberFunction Create(const std::string& name) {
            using RetType = typename my_reflect::static_refl::function_traits<T>::return_type;
            using ArgTypes = typename my_reflect::static_refl::function_traits<T>::parameter_type;
            return MemberFunction{name, GetType<RetType>(), TypeListToVector<ArgTypes>()};

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
    };

}

