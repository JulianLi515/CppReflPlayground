//
// Created by qianq on 12/29/2025.
//
// Main header file for dynamic reflection system
// Include this file to use all dynamic reflection features

#pragma once

#include "Type.h"
#include "Arithmetic.h"
#include "Enum.h"
#include "Void.h"
#include "Pointer.h"
#include "TypeRegistry.h"
#include <type_traits>

#include "Class.h"

class TrivialFactory {
    public:
    static TrivialFactory& Instance() {
        static TrivialFactory instance;
        return instance;
    }
};

namespace my_reflect::dynamic_refl {

    template <typename T>
    auto& GetFactory() {
        if constexpr (std::is_void_v<T>) return VoidFactory::Instance();
        else if constexpr (std::is_pointer_v<T>) return PointerFactory<T>::Instance();
        else if constexpr (std::is_arithmetic_v<T>) return ArithmeticFactory<T>::Instance();
        else if constexpr (std::is_enum_v<T>) return EnumFactory<T>::Instance();
        else if constexpr (std::is_class_v<T>) return ClassFactory<T>::Instance();
        else return TrivialFactory::Instance();

    }

    template <typename T>
    auto& Register() {
        return GetFactory<T>();
    }

    template <typename T>
    const Type* GetType() {
        // Remove const, volatile, and reference qualifiers
        using RawType = std::remove_cv_t<std::remove_reference_t<T>>;
        return &GetFactory<RawType>().GetInfo();
    }

    // Lookup type by name from registry
    inline Type* GetType(const std::string& name) {
        return TypeRegistry::Instance().GetTypeByName(name);
    }

    // Get all registered types
    inline const std::unordered_map<std::string, Type*>& GetAllTypes() {
        return TypeRegistry::Instance().GetAllTypes();
    }

}
