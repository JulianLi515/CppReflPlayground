#pragma once
#include "field_traits.h"
namespace my_reflect {

	// static_reflection core
    template<typename T>
	struct TypeData
    {
		using base_types = std::tuple<>; // default no base types
    }; // main template, to be specialized by macros

    #define BEGIN_REFLECT(X) template <> struct my_reflect::TypeData<X> {

	#define BASE_CLASSES(...)\
		using base_types = std::tuple<__VA_ARGS__>;

    #define functions(...)\
        static constexpr auto functions = std::make_tuple(__VA_ARGS__);

    #define fields(...)\
        static constexpr auto fields = std::make_tuple(__VA_ARGS__);

    #define func(F)\
        my_reflect::field_traits{ F, #F }

    #define var(F)\
        my_reflect::field_traits{ F, #F }

    #define END_REFLECT() };

	// main interface to get type data
    template<typename T>
    constexpr auto type_data() {
        return TypeData<T>{};
    }
}