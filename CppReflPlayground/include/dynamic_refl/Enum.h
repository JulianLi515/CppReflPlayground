//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include "TypeRegistry.h"
#include <string>
#include <vector>

namespace my_reflect::dynamic_refl {

    class Enum : public Type {
    public:
        Enum();
        explicit Enum(const std::string& name);

        struct Item {
            using value_type = long;
            std::string name_;
            value_type value_;
        };

        template <typename T>
        void add(const std::string& name, T value) {
            items_.emplace_back(Item{name, static_cast<Item::value_type>(value)});
        }

        const auto& GetItems() const { return items_; }

    private:
        std::vector<Item> items_;
    };

    template <typename T>
    class EnumFactory final {
    public:
        static EnumFactory& Instance() {
            static EnumFactory inst{};
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
        Enum info_;
    };

}
