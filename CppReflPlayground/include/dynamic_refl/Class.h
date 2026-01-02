//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include <string>
#include <vector>

#include "TypeRegistry.h"
#include "MemberFunction.h"
#include "MemberVariable.h"
#include "MemberContainer.h"
#include "static_refl/container_traits.h"

namespace my_reflect::dynamic_refl {

    // Forward declare Any
    class Any;

    class Class : public Type {
    public:
        Class();
        explicit Class(const std::string& name);

        std::vector<const Class*> baseClasses_;
        std::vector<MemberFunction> memberFunctions_;
        std::vector<MemberVariable> memberVariables_;
        std::vector<MemberContainer> memberContainers_;

        void AddVar(MemberVariable&& variable);
        void AddFunc(MemberFunction&& function);
        void AddContainer(MemberContainer&& container);

        // ========== Any Support Methods ==========
        Any GetMemberValue(Any& instance, const std::string& memberName) const;
        bool SetMemberValue(Any& instance, const std::string& memberName, const Any& value) const;

        // Find member function by name (for invoke support)
        const MemberFunction* FindFunction(const std::string& name) const;

    };

    template <typename T>
    class ClassFactory final {
    public:
        static ClassFactory& Instance() {
            static ClassFactory inst{};
            return inst;
        }
        ClassFactory& Register(const std::string& name) {
            info_.SetName(name);
            TypeRegistry::Instance().RegisterType(name, &info_);
            return *this;
        };

        template <typename U>
        ClassFactory& Add(const std::string& name, U ptr) {
            if constexpr(std::is_member_function_pointer_v<U>) {
                info_.AddFunc(MemberFunction::Create<U>(name, ptr));
            }else if constexpr (std::is_member_object_pointer_v<U>) {
                // Extract member type from member pointer
                using MemberType = std::remove_cv_t<std::remove_reference_t<decltype(std::declval<T>().*std::declval<U>())>>;
                if constexpr (static_refl::is_container_v<MemberType>) {
                    info_.AddContainer(MemberContainer::Create<MemberType>(name));
                } else {
                    info_.AddVar(MemberVariable::Create<U>(name));
                }
            }
            return *this;
        }

        // Overload for type-only (used for decltype)
        template <typename U>
        ClassFactory& Add(const std::string& name) {
            // This overload is for backward compatibility and type deduction
            // For member functions, user should provide the pointer
            if constexpr (std::is_member_object_pointer_v<U>) {
                using MemberType = std::remove_cv_t<std::remove_reference_t<decltype(std::declval<T>().*std::declval<U>())>>;
                if constexpr (static_refl::is_container_v<MemberType>) {
                    info_.AddContainer(MemberContainer::Create<MemberType>(name));
                } else {
                    info_.AddVar(MemberVariable::Create<U>(name));
                }
            }
            return *this;
        }

        template <typename U>
        ClassFactory& AddBaseClass() {
            static_assert(std::is_class_v<U>, "Base class must be a class type");
            info_.baseClasses_.push_back(static_cast<const Class*>(GetType<U>()));
            return *this;
        }

        Class& GetInfo() { return info_; }
    private:
        Class info_;
    };

}
