//
// Created by qianq on 12/30/2025.
//

#pragma once
#include <string>
#include "Type.h"
#include "../static_refl/variable_traits.h"

namespace my_reflect::dynamic_refl {

    // Forward declaration
    template <typename T>
    const Type* GetType();
    class MemberVariable {
    public:
        std::string name_;
        const Type* type_;

        MemberVariable(std::string name, const Type* type);
        MemberVariable(MemberVariable&& other) noexcept;

        template <typename T>
        static MemberVariable Create(const std::string& name) {
            using V_Type = typename my_reflect::static_refl::variable_traits<T>::type;
            return MemberVariable{name, GetType<V_Type>()};
        }
    };
}
