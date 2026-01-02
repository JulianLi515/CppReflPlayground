#pragma once
#include "field_traits.h"
#include <stdexcept>
#include <string_view>
#include <optional>

/**
 * @brief Compile-time reflection metadata for a user-defined type T.
 *
 * TypeData<T> is the "static reflection" entry point of this library.
 * It stores type information (fields/functions/containers lists, type-lists, etc.) as compile-time
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
 * class Person {
 * public:
 *     int age;
 *     std::string name;
 *     std::vector<std::string> friends;
 *     void speak();
 * };
 *
 * BEGIN_REFLECT(Person)
 * BASE_CLASSES()
 * variables(
 *     var(&Person::age),
 *     var(&Person::name)
 * )
 * containers(
 *     container(&Person::friends)
 * )
 * functions(
 *     func(&Person::speak)
 * )
 * END_REFLECT()
 *
 * // runtime: operates on values -> requires an instance
 * Person person;
 * my_reflect::static_refl::utils::print_all(person);
 *
 * // compile-time: operates on metadata/types -> no instance required
 * using TD = my_reflect::static_refl::TypeData<Person>;
 * using VarTypes = TD::variable_types;     // type_list of variables
 * using ContTypes = TD::container_types;   // type_list of containers
 * using FnTypes = TD::function_types;      // type_list of functions
 * @endcode
 *
 * Note:
 * - Accessing nested aliases (e.g. TD::function_types) triggers template instantiation of TypeData<T>,
 *   but does not require constructing an instance of T.
 */
namespace my_reflect::static_refl {

	// Helper for finding function index by name in tuple
	namespace detail {
		template<typename Tuple, size_t... Is>
		constexpr int find_function_index_impl(const Tuple& funcs, std::string_view name, std::index_sequence<Is...>) {
			int result = -1;
			((std::get<Is>(funcs).name_ == name ? (result = static_cast<int>(Is), true) : false) || ...);
			return result;
		}

		// Helper for invoking function by index
		template<size_t Index, typename Tuple, typename Instance, typename... Args>
		auto invoke_by_index_impl(const Tuple& funcs, Instance& instance, Args&&... args) {
			static_assert(Index < std::tuple_size_v<Tuple>, "Function index out of range");
			const auto& func_field = std::get<Index>(funcs);
			return (instance.*(func_field.ptr_))(std::forward<Args>(args)...);
		}

		// Helper for finding variable index by name
		template<typename Tuple, size_t... Is>
		constexpr int find_variable_index_impl(const Tuple& vars, std::string_view name, std::index_sequence<Is...>) {
			int result = -1;
			((std::get<Is>(vars).name_ == name ? (result = static_cast<int>(Is), true) : false) || ...);
			return result;
		}

		// Helper for getting variable by index
		template<size_t Index, typename Tuple, typename Instance>
		decltype(auto) get_by_index_impl(const Tuple& vars, Instance& instance) {
			static_assert(Index < std::tuple_size_v<Tuple>, "Variable index out of range");
			const auto& var_field = std::get<Index>(vars);
			return instance.*(var_field.ptr_);
		}

		// Helper for setting variable by index
		template<size_t Index, typename Tuple, typename Instance, typename Value>
		void set_by_index_impl(const Tuple& vars, Instance& instance, Value&& value) {
			static_assert(Index < std::tuple_size_v<Tuple>, "Variable index out of range");
			const auto& var_field = std::get<Index>(vars);
			instance.*(var_field.ptr_) = std::forward<Value>(value);
		}
	}

	// Helper to extract class type from function tuple
	template<typename Tuple>
	struct get_class_type {
		using first_field = std::tuple_element_t<0, Tuple>;
		using first_func_ptr = decltype(std::declval<first_field>().ptr_);
		using type = typename function_traits<first_func_ptr>::class_type;
	};

	template<typename Tuple>
	using get_class_type_t = typename get_class_type<Tuple>::type;

	// Helper to extract class type from variable tuple
	template<typename Tuple>
	struct get_var_class_type {
		using first_field = std::tuple_element_t<0, Tuple>;
		using first_var_ptr = decltype(std::declval<first_field>().ptr_);
		using type = typename variable_traits<first_var_ptr>::class_type;
	};

	template<typename Tuple>
	using get_var_class_type_t = typename get_var_class_type<Tuple>::type;

	// C++20 type_identity for C++17 compatibility
	template<typename T>
	struct type_identity { using type = T; };

	template<typename T>
	using type_identity_t = typename type_identity<T>::type;

	// static_reflection core
    template<typename T>
	struct TypeData
    {
		using base_types = type_list<>; // default no base types
    }; // main template, to be specialized by macros

    #define BEGIN_REFLECT(X) template <> struct my_reflect::static_refl::TypeData<X> {

	#define BASE_CLASSES(...)\
		using base_types = type_list<__VA_ARGS__>;

    #define functions(...)\
        static constexpr auto functions = std::make_tuple(__VA_ARGS__);\
		using function_types = type_list_from_tuple_t<decltype(functions)>;\
		\
		static constexpr int find_function_index(std::string_view funcName) {\
			return detail::find_function_index_impl(functions, funcName, std::make_index_sequence<std::tuple_size_v<decltype(functions)>>{});\
		}\
		\
		template<size_t Index, typename... Args>\
		static auto invoke(type_identity_t<get_class_type_t<decltype(functions)>>& instance, Args&&... args) {\
			return detail::invoke_by_index_impl<Index>(functions, instance, std::forward<Args>(args)...);\
		}

    #define variables(...)\
        static constexpr auto variables = std::make_tuple(__VA_ARGS__);\
        using variable_types = type_list_from_tuple_t<decltype(variables)>;\
        \
        static constexpr int find_variable_index(std::string_view varName) {\
            return detail::find_variable_index_impl(variables, varName, std::make_index_sequence<std::tuple_size_v<decltype(variables)>>{});\
        }\
        \
        template<size_t Index>\
        static decltype(auto) get(type_identity_t<get_var_class_type_t<decltype(variables)>>& instance) {\
            return detail::get_by_index_impl<Index>(variables, instance);\
        }\
        \
        template<size_t Index>\
        static decltype(auto) get(const type_identity_t<get_var_class_type_t<decltype(variables)>>& instance) {\
            return detail::get_by_index_impl<Index>(variables, instance);\
        }\
        \
        template<size_t Index, typename Value>\
        static void set(type_identity_t<get_var_class_type_t<decltype(variables)>>& instance, Value&& value) {\
            detail::set_by_index_impl<Index>(variables, instance, std::forward<Value>(value));\
        }

    #define containers(...)\
        static constexpr auto containers = std::make_tuple(__VA_ARGS__);\
        using container_types = type_list_from_tuple_t<decltype(containers)>;

    #define func(F)\
        my_reflect::static_refl::field_traits{ F, #F }

    #define var(F)\
        my_reflect::static_refl::field_traits{ F, #F }

    #define container(F)\
        my_reflect::static_refl::field_traits{ F, #F }

    #define END_REFLECT() };

	// main interface to get type data
    template<typename T>
    constexpr auto type_data() {
        return TypeData<T>{};
    }
}

