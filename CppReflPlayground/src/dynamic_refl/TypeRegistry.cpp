//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/TypeRegistry.h"

namespace my_reflect::dynamic_refl {

    TypeRegistry& TypeRegistry::Instance() {
        static TypeRegistry instance;
        return instance;
    }

    void TypeRegistry::RegisterType(const std::string& name, Type* type) {
        if (types_.find(name) != types_.end()) {
            return; // Already registered
        }
        types_[name] = type;
    }

    Type* TypeRegistry::GetTypeByName(const std::string& name) const {
        auto it = types_.find(name);
        return it != types_.end() ? it->second : nullptr;
    }

    const std::unordered_map<std::string, Type*>& TypeRegistry::GetAllTypes() const {
        return types_;
    }

}
