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

namespace my_reflect::dynamic_refl {

    class Class : public Type {
    public:
        Class();
        explicit Class(const std::string& name);

        std::vector<const Class*> baseClasses_;
        std::vector<MemberFunction> memberFunctions_;
        std::vector<MemberVariable> memberVariables_;

        void AddVar(MemberVariable&& variable);
        void AddFunc(MemberFunction&& function);

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
        ClassFactory& Add(const std::string& name) {
            if constexpr(std::is_member_function_pointer_v<U>) {
                info_.AddFunc(MemberFunction::Create<U>(name));
            }else if constexpr (std::is_member_object_pointer_v<U>) {
                info_.AddVar(MemberVariable::Create<U>(name));
            }else {

            }
            return *this;
        }

        Class& GetInfo() { return info_; }
    private:
        Class info_;
    };

}
