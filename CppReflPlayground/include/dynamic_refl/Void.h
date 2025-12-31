//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include <string>

namespace my_reflect::dynamic_refl {

    class Void : public Type {
    public:
        Void();
    };

    class VoidFactory final {
    public:
        static VoidFactory& Instance();
        Void& GetInfo() { return info_; }
    private:
        VoidFactory();
        Void info_;
    };

}
