//
// Created by qianq on 12/31/2025.
//

#pragma once
#include <string>
#include <utility>
#include "Type.h"
#include "static_refl/container_traits.h"
#include "static_refl/field_traits.h"

namespace my_reflect::dynamic_refl {
    template <typename T>
    const Type* GetType(); // forward declare

    class MemberContainer {
    public:
        std::string name_;
        const Type* valueType_;
        const Type* keyType_;
        static_refl::ContainerKind kind_;

        MemberContainer(std::string name, static_refl::ContainerKind kind, const Type* valueType, const Type* keyType = nullptr);
        MemberContainer(MemberContainer&& other) noexcept;

        template <typename T>
        static MemberContainer Create(std::string name) {
            using V_Type = static_refl::container_traits_value_t<T>;
            constexpr static_refl::ContainerKind kind = static_refl::container_kind_v<T>;
            if constexpr (kind == static_refl::ContainerKind::Map) {
                using K_Type = static_refl::container_traits_key_t<T>;
                return MemberContainer{std::move(name), kind, GetType<V_Type>(), GetType<K_Type>()};
            }else {
                return MemberContainer{std::move(name), kind, GetType<V_Type>()};
            }
        }
    };

}

