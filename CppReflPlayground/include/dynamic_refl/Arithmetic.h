//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include "TypeRegistry.h"
#include <string>

namespace my_reflect::dynamic_refl {

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
