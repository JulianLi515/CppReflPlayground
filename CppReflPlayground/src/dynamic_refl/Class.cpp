//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Class.h"

#include "dynamic_refl/MemberVariable.h"
#include "dynamic_refl/MemberFunction.h"

namespace my_reflect::dynamic_refl {
    Class::Class(): Type("Unknown_Class", Kind::Class) {

    }

    Class::Class(const std::string& name) : Type(name, Kind::Class) {}

    void Class::AddVar(MemberVariable &&variable) {
        memberVariables_.emplace_back(std::move(variable));
    }

    void Class::AddFunc(MemberFunction &&function) {
        memberFunctions_.emplace_back(std::move(function));
    }

    void Class::AddContainer(MemberContainer &&container) {
        memberContainers_.emplace_back(std::move(container));
    }
}
