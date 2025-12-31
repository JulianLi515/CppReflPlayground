//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/Void.h"
#include "../../include/dynamic_refl/TypeRegistry.h"

namespace my_reflect::dynamic_refl {

    Void::Void() : Type("void", Kind::Void) {}

    VoidFactory::VoidFactory() : info_() {}

    VoidFactory& VoidFactory::Instance() {
        static VoidFactory instance;
        // Register to TypeRegistry on first access
        static bool registered = []() {
            TypeRegistry::Instance().RegisterType("void", &instance.info_);
            return true;
        }();
        (void)registered;
        return instance;
    }

}
