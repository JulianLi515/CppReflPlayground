//
// Created by qianq on 12/30/2025.
//

#include "../../include/dynamic_refl/MemberFunction.h"
#include "../../include/dynamic_refl/Any.h"

namespace my_reflect::dynamic_refl {

    MemberFunction::MemberFunction(std::string name, const Type* retType, std::vector<const Type*> argTypes,
                                  std::function<Any(Any&, const std::vector<Any>&)> invoker)
        : name_(std::move(name)), retType_(retType), argTypes_(std::move(argTypes)), invoker_(std::move(invoker))
    {
    }

    MemberFunction::MemberFunction(MemberFunction&& other) noexcept
        : name_(std::move(other.name_)), retType_(other.retType_), argTypes_(std::move(other.argTypes_)),
          invoker_(std::move(other.invoker_))
    {
        other.retType_ = nullptr;
    }

    Any MemberFunction::Invoke(Any& instance, const std::vector<Any>& args) const {
        if (!invoker_) {
            throw std::runtime_error("Function invoker is not set");
        }
        return invoker_(instance, args);
    }

}
