//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/MemberVariable.h"

namespace my_reflect::dynamic_refl {

    MemberVariable::MemberVariable(std::string name, const Type* type)
        : name_(std::move(name)), type_(type)
    {
    }

    MemberVariable::MemberVariable(MemberVariable&& other) noexcept
        : name_(std::move(other.name_)), type_(other.type_)
    {
        other.type_ = nullptr;
    }

}