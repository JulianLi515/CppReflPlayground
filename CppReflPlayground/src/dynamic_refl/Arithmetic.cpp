//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Arithmetic.h"

namespace my_reflect::dynamic_refl {

    Arithmetic::Arithmetic(const Kind kind, const bool isSigned)
        : Type(getName(kind), Type::Kind::Arithmetic), kind_(kind), isSigned_(isSigned) {
    }

    std::string Arithmetic::getName(Kind kind) {
        switch (kind) {
            case Kind::Unknown: return "unknown";
            case Kind::Bool: return "bool";
            case Kind::Char: return "char";
            case Kind::Short: return "short";
            case Kind::Int: return "int";
            case Kind::Long: return "long";
            case Kind::LongLong: return "long long";
            case Kind::Float: return "float";
            case Kind::Double: return "double";
            default: return "unknown";
        }
    }

}
