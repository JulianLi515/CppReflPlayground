//
// Created by qianq on 12/31/2025.
//

#pragma once
#include <map>
#include <set>
#include <vector>
namespace my_reflect::static_refl {
    enum class ContainerKind {
        Set, Vector, Map
    };

    namespace detail {
        template <typename>
        struct container_traits {
            static constexpr bool is_container = false;
        };

        template <typename T, typename A>
        struct container_traits<std::vector<T, A>> {
            static constexpr auto kind = ContainerKind::Vector;
            static constexpr bool is_container = true;
            static constexpr bool is_member = false;
            using value_type = T;
        };

        template <typename T, typename C, typename A>
        struct container_traits<std::set<T, C, A>> {
            static constexpr auto kind = ContainerKind::Set;
            static constexpr bool is_container = true;
            static constexpr bool is_member = false;
            using value_type = T;
        };

        template <typename K, typename V, typename C, typename A>
        struct container_traits<std::map<K, V, C, A>> {
            static constexpr auto kind = ContainerKind::Map;
            static constexpr bool is_container = true;
            static constexpr bool is_member = false;
            using key_type = K;
            using value_type = V;
        };

        // ===== Member pointer specializations =====

        // std::vector<T> Class::*
        template <typename ClassT, typename T, typename A>
        struct container_traits<std::vector<T, A> ClassT::*> {
            static constexpr auto kind = ContainerKind::Vector;
            static constexpr bool is_container = true;
            using value_type = T;
            using class_type = ClassT;
            static constexpr bool is_member = true;
        };

        // std::set<T> Class::*
        template <typename ClassT, typename T, typename C, typename A>
        struct container_traits<std::set<T, C, A> ClassT::*> {
            static constexpr auto kind = ContainerKind::Set;
            static constexpr bool is_container = true;
            using value_type = T;
            using class_type = ClassT;
            static constexpr bool is_member = true;
        };

        // std::map<K, V> Class::*
        template <typename ClassT, typename K, typename V, typename C, typename A>
        struct container_traits<std::map<K, V, C, A> ClassT::*> {
            static constexpr auto kind = ContainerKind::Map;
            static constexpr bool is_container = true;
            using key_type = K;
            using value_type = V;
            using class_type = ClassT;
            static constexpr bool is_member = true;
        };
    }

    template <typename T>
    constexpr auto container_kind_v = detail::container_traits<T>::kind;

    template <typename T>
    using container_traits_value_t = typename detail::container_traits<T>::value_type;

    template <typename T>
    using container_traits_key_t = typename detail::container_traits<T>::key_type;

    template <typename T>
    constexpr bool is_container_v = detail::container_traits<T>::is_container;


}

