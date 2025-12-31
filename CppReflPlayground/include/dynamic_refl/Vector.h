//
// Created by qianq on 12/31/2025.
//

#pragma once
#include "dynamic_reflect_core.h"
#include "Type.h"

namespace my_reflect::dynamic_refl {
    class Vector : public Type {
    public:
        Vector();
        Vector(const std::string& name,const Type* elementType);

        template <typename U>
        static Vector Create(const std::string& name) {
            return Vector{name, GetType<U>()};
        }

        const Type* elementType_;
    };

    template <typename T>
    class VectorFactory final {
    public:
        static VectorFactory& Instance() {
            static VectorFactory inst{Vector::Create<T>()};
            return inst;
        }
        Vector& GetInfo() { return info_; }

    private:
        Vector info_;
        VectorFactory(Vector&& info): info_(std::move(info)){}
    };


}



