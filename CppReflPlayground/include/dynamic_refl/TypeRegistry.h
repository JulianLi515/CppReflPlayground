//
// Created by qianq on 12/30/2025.
//
// Singleton registry to store all type information

#pragma once
#include "Type.h"
#include <unordered_map>
#include <string>

namespace my_reflect::dynamic_refl {

    class TypeRegistry {
    public:
        static TypeRegistry& Instance();

        // Register a type with its name
        // Returns early if already registered
        void RegisterType(const std::string& name, Type* type);

        // Lookup type by name
        Type* GetTypeByName(const std::string& name) const;

        // Get all registered types
        const std::unordered_map<std::string, Type*>& GetAllTypes() const;

        TypeRegistry(const TypeRegistry&) = delete;
        TypeRegistry& operator=(const TypeRegistry&) = delete;
    private:
        TypeRegistry() = default;

        std::unordered_map<std::string, Type*> types_;
    };

}
