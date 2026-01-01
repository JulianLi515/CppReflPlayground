//
// Created by qianq on 12/31/2025.
//

#include "../../include/dynamic_refl/MemberContainer.h"
namespace my_reflect::dynamic_refl {
    MemberContainer::MemberContainer(std::string name, static_refl::ContainerKind kind, const Type* valueType, const Type* keyType)
        : name_(std::move(name)), valueType_(valueType), keyType_(keyType), kind_(kind)
    {
    }

    MemberContainer::MemberContainer(MemberContainer&& other) noexcept
        : name_(std::move(other.name_)), valueType_(other.valueType_), keyType_(other.keyType_), kind_(other.kind_)
    {
        other.valueType_ = nullptr;
        other.keyType_ = nullptr;
    }
}
