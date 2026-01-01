//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/MemberFunction.h"
namespace my_reflect::dynamic_refl {

    MemberFunction::MemberFunction(std::string name, const Type* retType, std::vector<const Type*> argTypes)
        : name_(std::move(name)), retType_(retType), argTypes_(std::move(argTypes))
    {
    }

    MemberFunction::MemberFunction(MemberFunction&& other) noexcept
        : name_(std::move(other.name_)), retType_(other.retType_), argTypes_(std::move(other.argTypes_))
    {
        other.retType_ = nullptr;
    }

}
