//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Type.h"
#include "../../include/dynamic_refl/Arithmetic.h"
#include "../../include/dynamic_refl/Enum.h"
#include "../../include/dynamic_refl/Class.h"

namespace my_reflect::dynamic_refl {

    Type::Type(const std::string& name, Kind kind) : name_(name), kind_(kind) {}

    Type::Type(Type&& other) noexcept
        : name_(std::move(other.name_)), kind_(other.kind_) {}

    const Arithmetic* Type::AsArithmetic() const {
        return kind_ == Kind::Arithmetic ? static_cast<const Arithmetic*>(this) : nullptr;
    }

    const Enum* Type::AsEnum() const {
        return kind_ == Kind::Enum ? static_cast<const Enum*>(this) : nullptr;
    }

    const Class* Type::AsClass() const {
        return kind_ == Kind::Class ? static_cast<const Class*>(this) : nullptr;
    }

}
