//
// Created by qianq on 12/31/2025.
//

#pragma once
#include <string>
#include <utility>
#include <functional>
#include "Type.h"
#include "static_refl/container_traits.h"
#include "static_refl/field_traits.h"

namespace my_reflect::dynamic_refl {
    // Forward declarations
    template <typename T>
    const Type* GetType();
    class Any;

    // Type-erased container operations
    struct ContainerOperations {
        std::function<size_t(const Any&)> size = nullptr;
        std::function<void(Any&)> clear = nullptr;
        std::function<bool(Any&, const Any&)> push = nullptr;  // for vector/set
        std::function<Any(const Any&, size_t)> at = nullptr;   // for vector
        std::function<bool(Any&, const Any&, const Any&)> insert_kv = nullptr;  // for map
        std::function<Any(const Any&, const Any&)> get_value = nullptr;  // for map
        std::function<bool(const Any&, const Any&)> contains_key = nullptr;  // for map
    };

    class MemberContainer {
    public:
        std::string name_;
        const Type* valueType_;
        const Type* keyType_;
        static_refl::ContainerKind kind_;
        ContainerOperations ops_;

        MemberContainer(std::string name, static_refl::ContainerKind kind, const Type* valueType,
                       const Type* keyType = nullptr, ContainerOperations ops = {});
        MemberContainer(MemberContainer&& other) noexcept;

        template <typename T>
        static MemberContainer Create(std::string name);
    };

}

// ========== Template Implementation (requires Any definition) ==========
#include "Any.h"

namespace my_reflect::dynamic_refl {

    template <typename T>
    MemberContainer MemberContainer::Create(std::string name) {
        using V_Type = static_refl::container_traits_value_t<T>;
        constexpr static_refl::ContainerKind kind = static_refl::container_kind_v<T>;

        ContainerOperations ops;

        // Size operation (common for all containers)
        ops.size = [](const Any& any) -> size_t {
            if (auto* ptr = any_cast<T>(any)) {
                return ptr->size();
            }
            throw std::runtime_error("any_cast failed in container size()");
        };

        // Clear operation (common for all containers)
        ops.clear = [](Any& any) {
            if (auto* ptr = any_cast<T>(any)) {
                ptr->clear();
                return;
            }
            throw std::runtime_error("any_cast failed in container clear()");
        };

        if constexpr (kind == static_refl::ContainerKind::Vector) {
            // Vector-specific operations
            ops.push = [](Any& any, const Any& value) -> bool {
                auto* vec = any_cast<T>(any);
                auto* val = any_cast<V_Type>(value);
                if (vec && val) {
                    vec->push_back(*val);
                    return true;
                }
                return false;
            };

            ops.at = [](const Any& any, size_t index) -> Any {
                auto* vec = any_cast<T>(any);
                if (vec && index < vec->size()) {
                    return make_cref((*vec)[index]);
                }
                throw std::out_of_range("Container index out of range");
            };

            return MemberContainer{std::move(name), kind, GetType<V_Type>(), nullptr, std::move(ops)};
        }
        else if constexpr (kind == static_refl::ContainerKind::Set) {
            // Set-specific operations
            ops.push = [](Any& any, const Any& value) -> bool {
                auto* set = any_cast<T>(any);
                auto* val = any_cast<V_Type>(value);
                if (set && val) {
                    set->insert(*val);
                    return true;
                }
                return false;
            };

            return MemberContainer{std::move(name), kind, GetType<V_Type>(), nullptr, std::move(ops)};
        }
        else if constexpr (kind == static_refl::ContainerKind::Map) {
            // Map-specific operations
            using K_Type = static_refl::container_traits_key_t<T>;

            ops.insert_kv = [](Any& any, const Any& key, const Any& value) -> bool {
                auto* map = any_cast<T>(any);
                auto* k = any_cast<K_Type>(key);
                auto* v = any_cast<V_Type>(value);
                if (map && k && v) {
                    (*map)[*k] = *v;
                    return true;
                }
                return false;
            };

            ops.get_value = [](const Any& any, const Any& key) -> Any {
                auto* map = any_cast<T>(any);
                auto* k = any_cast<K_Type>(key);
                if (map && k) {
                    auto it = map->find(*k);
                    if (it != map->end()) {
                        return make_cref(it->second);
                    }
                }
                throw std::runtime_error("Key not found in map");
            };

            ops.contains_key = [](const Any& any, const Any& key) -> bool {
                auto* map = any_cast<T>(any);
                auto* k = any_cast<K_Type>(key);
                if (map && k) {
                    return map->find(*k) != map->end();
                }
                return false;
            };

            return MemberContainer{std::move(name), kind, GetType<V_Type>(), GetType<K_Type>(), std::move(ops)};
        }
        else {
            return MemberContainer{std::move(name), kind, GetType<V_Type>()};
        }
    }

}

