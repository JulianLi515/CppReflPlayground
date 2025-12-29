#pragma once
#include "field_traits.h"

/**
 * @brief Compile-time reflection metadata for a user-defined type T.
 *
 * TypeData<T> is the "static reflection" entry point of this library.
 * It stores type information (fields/functions lists, type-lists, etc.) as compile-time
 * metadata, which can be queried WITHOUT constructing an instance of T.
 *
 * There are two usage modes:
 *
 * 1) Runtime (needs an instance):
 *    - When you want to read/write actual member values, call functions that take an object,
 *      e.g. utils::print_all(obj). This uses the compile-time metadata to generate direct access
 *      (obj.*member_ptr) at runtime.
 *
 * 2) Compile-time / type-only (no instance needed):
 *    - When you only need "what does this type contain" (e.g. list of reflected fields/functions,
 *      their trait types, signatures, etc.), you can query TypeData<T> directly.
 *    - This is useful for meta algorithms (type_list map/filter/count), concept checks, generating
 *      adapters, etc., without ever creating an object.
 *
 * Example:
 * @code
 * // runtime: operates on values -> requires an instance
 * Student stu("Bob", 22, 654321L);
 * my_reflect::utils::print_all(stu);
 *
 * // compile-time: operates on metadata/types -> no instance required
 * using TD = my_reflect::TypeData<Student>;
 * using FnTypes = TD::function_types;   // type_list<...> (traits or signatures)
 * @endcode
 *
 * Note:
 * - Accessing nested aliases (e.g. TD::function_types) triggers template instantiation of TypeData<T>,
 *   but does not require constructing an instance of T.
 */
namespace my_reflect {

	// static_reflection core
    template<typename T>
	struct TypeData
    {
		using base_types = type_list<>; // default no base types
    }; // main template, to be specialized by macros

    #define BEGIN_REFLECT(X) template <> struct my_reflect::TypeData<X> {

	#define BASE_CLASSES(...)\
		using base_types = type_list<__VA_ARGS__>;

    #define functions(...)\
        static constexpr auto functions = std::make_tuple(__VA_ARGS__);\
		using function_types = type_list_from_tuple_t<decltype(functions)>;

    #define variables(...)\
        static constexpr auto variables = std::make_tuple(__VA_ARGS__);\
        using variable_types = type_list_from_tuple_t<decltype(variables)>;

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

