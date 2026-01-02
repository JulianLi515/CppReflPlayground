//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Class.h"
#include "../../include/dynamic_refl/Any.h"

#include "dynamic_refl/MemberVariable.h"
#include "dynamic_refl/MemberFunction.h"
#include <typeinfo>

namespace my_reflect::dynamic_refl {
    Class::Class(): Type("Unknown_Class", Kind::Class) {

    }

    Class::Class(const std::string& name) : Type(name, Kind::Class) {}

    void Class::AddVar(MemberVariable &&variable) {
        memberVariables_.emplace_back(std::move(variable));
    }

    void Class::AddFunc(MemberFunction &&function) {
        memberFunctions_.emplace_back(std::move(function));
    }

    void Class::AddContainer(MemberContainer &&container) {
        memberContainers_.emplace_back(std::move(container));
    }

    // ========== Any Support Methods Implementation ==========

    Any Class::GetMemberValue(Any& instance, const std::string& memberName) const {
        if (instance.typeInfo->GetKind() != Type::Kind::Class) {
            throw std::bad_cast();
        }

        // Search in member variables
        for (const auto& var : memberVariables_) {
            if (var.name_ == memberName) {
                // TODO: Need member pointer stored in MemberVariable to access the actual member
                // For now, return empty Any
                throw std::runtime_error("GetMemberValue not fully implemented - need member pointers");
            }
        }

        // Search in member containers
        for (const auto& container : memberContainers_) {
            if (container.name_ == memberName) {
                // TODO: Need member pointer stored in MemberContainer
                throw std::runtime_error("GetMemberValue not fully implemented - need member pointers");
            }
        }

        throw std::runtime_error("Member '" + memberName + "' not found");
    }

    bool Class::SetMemberValue(Any& instance, const std::string& memberName, const Any& value) const {
        if (instance.typeInfo->GetKind() != Type::Kind::Class) {
            throw std::bad_cast();
        }

        if (instance.storage() == Any::storage_type::ConstRef) {
            throw std::runtime_error("Cannot modify const reference Any");
        }

        // Search in member variables
        for (const auto& var : memberVariables_) {
            if (var.name_ == memberName) {
                // TODO: Need member pointer stored in MemberVariable to access the actual member
                throw std::runtime_error("SetMemberValue not fully implemented - need member pointers");
            }
        }

        throw std::runtime_error("Member '" + memberName + "' not found");
    }

    const MemberFunction* Class::FindFunction(const std::string& name) const {
        for (const auto& func : memberFunctions_) {
            if (func.name_ == name) {
                return &func;
            }
        }
        return nullptr;
    }

}
