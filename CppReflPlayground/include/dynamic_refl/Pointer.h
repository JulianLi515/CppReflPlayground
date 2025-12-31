//
// Created by qianq on 12/30/2025.
//

#pragma once
#include "Type.h"
#include "TypeRegistry.h"
#include <type_traits>

namespace my_reflect::dynamic_refl {

    // Forward declaration
    template <typename T>
    const Type* GetType();
    class Pointer: public Type {
    public:
        Pointer();
        Pointer(const Type* pointedType);
        const Type* pointedType_;

        template <typename T>
        static Pointer Create() {
            using RawType = std::remove_pointer_t<T>;
            return Pointer{GetType<RawType>()};
        }
    };

    template <typename T>
    class PointerFactory final {
    public:
        static PointerFactory& Instance() {
            static PointerFactory inst{Pointer::Create<T>()};
            // Register to TypeRegistry on first access
            static bool registered = []() {
                TypeRegistry::Instance().RegisterType(inst.info_.GetName(), &inst.info_);
                return true;
            }();
            (void)registered;
            return inst;
        }
        Pointer& GetInfo() { return info_; }
    private:
        Pointer info_;
        PointerFactory(Pointer&& info): info_(std::move(info)){}
    };

}
