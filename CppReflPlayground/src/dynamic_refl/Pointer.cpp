//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Pointer.h"

namespace my_reflect::dynamic_refl {
    Pointer::Pointer(): Type("Pointer", Kind::Pointer), pointedType_(nullptr) {
    }

    Pointer::Pointer(const Type *pointedType)
        :Type(pointedType->GetName() + "*", Kind::Pointer), pointedType_(pointedType){
    }

    Pointer::Pointer(Pointer&& other) noexcept
        : Type(std::move(other)), pointedType_(other.pointedType_) {
        other.pointedType_ = nullptr;
    }
}
