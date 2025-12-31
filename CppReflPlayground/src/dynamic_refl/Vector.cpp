//
// Created by qianq on 12/31/2025.
//

#include "../../include/dynamic_refl/Vector.h"

namespace my_reflect::dynamic_refl {
    Vector::Vector()
        : Type( "Unknown_vector", Kind::Vector), elementType_(nullptr) {
    }

    Vector::Vector(const std::string& name, const Type *elementType)
        :Type( name + "[]", Kind::Vector), elementType_(elementType){
    }
}
