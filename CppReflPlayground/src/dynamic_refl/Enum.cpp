//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Enum.h"

namespace my_reflect::dynamic_refl {

    Enum::Enum() : Type("Unknown_Enum", Kind::Enum) {}

    Enum::Enum(const std::string& name) : Type(name, Kind::Enum) {}

}
