//
// Created by qianq on 12/30/2025.
//

#pragma once
#include <string>
namespace my_reflect::dynamic_refl {
    class Enum;
    class Arithmetic;


    class Type {
    public:
        enum class Kind {
            Arithmetic, Enum, Class, Pointer, Vector, Map, Set, Void
        };

        virtual ~Type() = default;
        explicit Type(const std::string& name, Kind kind);
        Type(Type&& other) noexcept;

        const std::string& GetName() const { return name_; }
        void SetName(const std::string& name) { name_ = name; }
        const Kind GetKind() const { return kind_; }

        const Arithmetic* AsArithmetic() const;
        const Enum* AsEnum() const;

    protected:
        std::string name_;
        Kind kind_;
    };
}
