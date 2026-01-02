//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include "TypeRegistry.h"
#include <string>
#include <optional>
#include <typeinfo>

namespace my_reflect::dynamic_refl {

    // Forward declare Any
    class Any;

    class Arithmetic : public Type {
    public:
        enum class Kind {
            Unknown, Bool, Char, Short, Int, Long, LongLong, Float, Double
        };

        explicit Arithmetic(Kind kind, bool isSigned);

        template <typename T>
        static Arithmetic Create() {
            return Arithmetic(detectKind<T>(), std::is_signed_v<T>);
        }

        // ========== Any Support Methods ==========
        template<typename T>
        static bool SetValue(Any& any, T value);

        template<typename T>
        static std::optional<T> GetValue(const Any& any);

    private:
        Kind kind_;
        bool isSigned_;

        static std::string getName(Kind kind);

        template <typename T>
        static Kind detectKind() {
            if constexpr (std::is_same_v<T, bool>) return Kind::Bool;
            else if constexpr (std::is_same_v<T, char>) return Kind::Char;
            else if constexpr (std::is_same_v<T, short>) return Kind::Short;
            else if constexpr (std::is_same_v<T, int>) return Kind::Int;
            else if constexpr (std::is_same_v<T, long>) return Kind::Long;
            else if constexpr (std::is_same_v<T, long long>) return Kind::LongLong;
            else if constexpr (std::is_same_v<T, float>) return Kind::Float;
            else if constexpr (std::is_same_v<T, double>) return Kind::Double;
            else return Kind::Unknown;
        }
    };

    template <typename T>
    class ArithmeticFactory final {
    public:
        static ArithmeticFactory& Instance() {
            static ArithmeticFactory inst{Arithmetic::Create<T>()};
            // Register to TypeRegistry on first access
            static bool registered = []() {
                TypeRegistry::Instance().RegisterType(inst.info_.GetName(), &inst.info_);
                return true;
            }();
            (void)registered; // Suppress unused variable warning
            return inst;
        }
        Arithmetic& GetInfo() { return info_; }
    private:
        Arithmetic info_;
        ArithmeticFactory(Arithmetic&& info): info_(std::move(info)){}
    };

}

// ========== Template Implementations (requires Any definition) ==========
#include "Any.h"

namespace my_reflect::dynamic_refl {

    template<typename T>
    bool Arithmetic::SetValue(Any& any, T value) {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

        if (any.typeInfo->GetKind() != Type::Kind::Arithmetic) {
            throw std::bad_cast();
        }

        return any_set(any, value);
    }

    template<typename T>
    std::optional<T> Arithmetic::GetValue(const Any& any) {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

        if (any.typeInfo->GetKind() != Type::Kind::Arithmetic) {
            throw std::bad_cast();
        }

        return any_get<T>(any);
    }

}
