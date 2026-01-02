//
// Created by qianq on 1/1/2026.
//
// Implementation of Any::invoke methods
// Include this file after Any.h and Class.h

#pragma once

#include "Any.h"
#include "Class.h"
#include <vector>
#include <stdexcept>

namespace my_reflect::dynamic_refl {

// Helper to convert variadic args to vector<Any>
// Uses make_ref for lvalue references, make_copy for rvalues
namespace detail {
    inline void pack_args(std::vector<Any>&) {}

    template<typename T, typename... Rest>
    void pack_args(std::vector<Any>& vec, T&& arg, Rest&&... rest) {
        // For lvalue references, use make_ref; for rvalues, use make_copy
        if constexpr (std::is_lvalue_reference_v<T>) {
            vec.push_back(make_ref(arg));
        } else {
            vec.push_back(make_copy(std::forward<T>(arg)));
        }
        pack_args(vec, std::forward<Rest>(rest)...);
    }
}

// Invoke member function by name
template<typename... Args>
Any Any::invoke(const std::string& funcName, Args&&... args) {
    // Check if typeInfo is Class type
    if (!typeInfo) {
        throw std::runtime_error("Cannot invoke method on empty Any");
    }

    if (typeInfo->GetKind() != Type::Kind::Class) {
        throw std::runtime_error("Cannot invoke method on non-class type");
    }

    // Cast to Class
    const Class* classInfo = typeInfo->AsClass();
    if (!classInfo) {
        throw std::runtime_error("Failed to cast Type to Class");
    }

    // Find function by name
    const MemberFunction* func = classInfo->FindFunction(funcName);
    if (!func) {
        throw std::runtime_error("Function not found: " + funcName);
    }

    // Pack arguments into vector<Any>
    std::vector<Any> anyArgs;
    detail::pack_args(anyArgs, std::forward<Args>(args)...);

    // Invoke the function
    return func->Invoke(*this, anyArgs);
}

// Invoke member function by index
template<typename... Args>
Any Any::invoke(size_t funcIndex, Args&&... args) {
    // Check if typeInfo is Class type
    if (!typeInfo) {
        throw std::runtime_error("Cannot invoke method on empty Any");
    }

    if (typeInfo->GetKind() != Type::Kind::Class) {
        throw std::runtime_error("Cannot invoke method on non-class type");
    }

    // Cast to Class
    const Class* classInfo = typeInfo->AsClass();
    if (!classInfo) {
        throw std::runtime_error("Failed to cast Type to Class");
    }

    // Check index bounds
    if (funcIndex >= classInfo->memberFunctions_.size()) {
        throw std::out_of_range("Function index out of range");
    }

    const MemberFunction* func = &classInfo->memberFunctions_[funcIndex];

    // Pack arguments into vector<Any>
    std::vector<Any> anyArgs;
    detail::pack_args(anyArgs, std::forward<Args>(args)...);

    // Invoke the function
    return func->Invoke(*this, anyArgs);
}

} // namespace my_reflect::dynamic_refl
