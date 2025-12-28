#pragma once
#include "field_traits.h"
namespace my_reflect {

    template<typename T>
    struct TypeData; // 主模板定义

    #define BEGIN_REFLECT(X) template <> struct my_reflect::TypeData<X> {

    #define functions(...)\
        static constexpr auto functions = std::make_tuple(__VA_ARGS__);

    #define fields(...)\
        static constexpr auto fields = std::make_tuple(__VA_ARGS__);

    #define func(F)\
        my_reflect::field_traits{ F, #F }

    #define var(F)\
        my_reflect::field_traits{ F, #F }

    #define END_REFLECT() };

    // 主接口，调用后生成类型信息类
    template<typename T>
    constexpr auto type_data() {
        return TypeData<T>{};
    }
}