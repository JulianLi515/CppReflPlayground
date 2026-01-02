//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Enum.h"
#include "../../include/dynamic_refl/Any.h"
#include <algorithm>
#include <typeinfo>
#include <cstdint>

namespace my_reflect::dynamic_refl {

    Enum::Enum() : Type("Unknown_Enum", Kind::Enum), underlyingSize_(sizeof(int)) {}

    Enum::Enum(const std::string& name, size_t underlyingSize)
        : Type(name, Kind::Enum), underlyingSize_(underlyingSize) {}

    // ========== Any Support Methods Implementation ==========

    bool Enum::SetByName(Any& any, const std::string& name) const {
        if (any.typeInfo->GetKind() != Type::Kind::Enum) {
            throw std::bad_cast();
        }

        if (any.storage() == Any::storage_type::ConstRef) {
            throw std::runtime_error("Cannot modify const reference Any");
        }

        // Find the enum value by name
        auto it = std::find_if(items_.begin(), items_.end(),
            [&name](const Item& item) { return item.name_ == name; });

        if (it == items_.end()) {
            return false; // Name not found
        }

        // Set the value based on the underlying type size
        void* ptr = any.payload;
        if (ptr) {
            switch (underlyingSize_) {
                case 1: *static_cast<uint8_t*>(ptr) = static_cast<uint8_t>(it->value_); break;
                case 2: *static_cast<uint16_t*>(ptr) = static_cast<uint16_t>(it->value_); break;
                case 4: *static_cast<uint32_t*>(ptr) = static_cast<uint32_t>(it->value_); break;
                case 8: *static_cast<uint64_t*>(ptr) = static_cast<uint64_t>(it->value_); break;
                default: return false;
            }
            return true;
        }

        return false;
    }

    bool Enum::SetByValue(Any& any, long long value) const {
        if (any.typeInfo->GetKind() != Type::Kind::Enum) {
            throw std::bad_cast();
        }

        if (any.storage() == Any::storage_type::ConstRef) {
            throw std::runtime_error("Cannot modify const reference Any");
        }

        // Check if the value exists in the enum
        auto it = std::find_if(items_.begin(), items_.end(),
            [value](const Item& item) { return item.value_ == value; });

        if (it == items_.end()) {
            return false; // Value not found
        }

        // Set the value based on the underlying type size
        void* ptr = any.payload;
        if (ptr) {
            switch (underlyingSize_) {
                case 1: *static_cast<uint8_t*>(ptr) = static_cast<uint8_t>(value); break;
                case 2: *static_cast<uint16_t*>(ptr) = static_cast<uint16_t>(value); break;
                case 4: *static_cast<uint32_t*>(ptr) = static_cast<uint32_t>(value); break;
                case 8: *static_cast<uint64_t*>(ptr) = static_cast<uint64_t>(value); break;
                default: return false;
            }
            return true;
        }

        return false;
    }

    std::optional<std::string> Enum::GetName(const Any& any) const {
        if (any.typeInfo->GetKind() != Type::Kind::Enum) {
            throw std::bad_cast();
        }

        const void* ptr = any.payload;
        if (ptr) {
            // Read the value based on the underlying type size
            long long value = 0;
            switch (underlyingSize_) {
                case 1: value = *static_cast<const uint8_t*>(ptr); break;
                case 2: value = *static_cast<const uint16_t*>(ptr); break;
                case 4: value = *static_cast<const uint32_t*>(ptr); break;
                case 8: value = *static_cast<const uint64_t*>(ptr); break;
                default: return std::nullopt;
            }

            // Find the name for this value
            auto it = std::find_if(items_.begin(), items_.end(),
                [value](const Item& item) { return item.value_ == value; });

            if (it != items_.end()) {
                return it->name_;
            }
        }

        return std::nullopt;
    }

    std::optional<long long> Enum::GetValue(const Any& any) const {
        if (any.typeInfo->GetKind() != Type::Kind::Enum) {
            throw std::bad_cast();
        }

        const void* ptr = any.payload;
        if (ptr) {
            // Read the value based on the underlying type size
            long long value = 0;
            switch (underlyingSize_) {
                case 1: value = *static_cast<const uint8_t*>(ptr); break;
                case 2: value = *static_cast<const uint16_t*>(ptr); break;
                case 4: value = *static_cast<const uint32_t*>(ptr); break;
                case 8: value = *static_cast<const uint64_t*>(ptr); break;
                default: return std::nullopt;
            }
            return value;
        }

        return std::nullopt;
    }

}
