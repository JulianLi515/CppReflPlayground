//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include "TypeRegistry.h"
#include <string>
#include <vector>
#include <optional>

namespace my_reflect::dynamic_refl {

    // Forward declare Any
    class Any;

    class Enum : public Type {
    public:
        Enum();
        explicit Enum(const std::string& name, size_t underlyingSize = sizeof(int));

        struct Item {
            using value_type = long long;
            std::string name_;
            value_type value_;
        };

        template <typename T>
        void add(const std::string& name, T value) {
            items_.emplace_back(Item{name, static_cast<Item::value_type>(value)});
        }

        const auto& GetItems() const { return items_; }
        size_t GetUnderlyingSize() const { return underlyingSize_; }

        // ========== Any Support Methods ==========
        bool SetByName(Any& any, const std::string& name) const;
        bool SetByValue(Any& any, long long value) const;
        std::optional<std::string> GetName(const Any& any) const;
        std::optional<long long> GetValue(const Any& any) const;

    private:
        std::vector<Item> items_;
        size_t underlyingSize_; // Size of the underlying enum type
    };

    template <typename T>
    class EnumFactory final {
    public:
        static EnumFactory& Instance() {
            static EnumFactory inst{sizeof(T)};
            return inst;
        }
        EnumFactory& Register(const std::string& name) {
            info_.SetName(name);
            // Register to TypeRegistry after name is set
            TypeRegistry::Instance().RegisterType(name, &info_);
            return *this;
        }

        template <typename U>
        EnumFactory& Add(const std::string& name, U value) {
            info_.add(name, value);
            return *this;
        }
        Enum& GetInfo() { return info_; }
    private:
        explicit EnumFactory(size_t size) : info_("Unknown_Enum", size) {}
        Enum info_;
    };

}
