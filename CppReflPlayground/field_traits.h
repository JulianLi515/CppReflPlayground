#pragma once
#include "function_traits.h"
#include "variable_traits.h"
namespace my_reflect {
namespace detail {

	template<bool isFunc, typename T>
	struct basic_field_traits; // main template

	// partial specialization for function pointer types
	template<typename T>
	struct basic_field_traits<true, T> : function_traits<T> {
		// pass through all members of function_traits
		using traits = function_traits<T>;

		constexpr bool is_member() const {
			return traits::is_member;
		}

		constexpr bool is_const() const {
			return traits::is_const;
		}

		constexpr bool is_function() const {
			return true;
		}

		constexpr bool is_variable() const {
			return false;
		}

		constexpr size_t param_cout() const
		{
			return std::tuple_size_v<typename traits::parameter_type>;
		}
	};

	// partial specialization for variable pointer types
	template<typename T>
	struct basic_field_traits<false, T> : variable_traits<T> {
		// pass through all members of variable_traits
		using traits = variable_traits<T>;
	
		constexpr bool is_member() const {
			return traits::is_member;
		}

		constexpr bool is_const() const {
			return traits::is_const;
		}

		constexpr bool is_function() const {
			return false;
		}

		constexpr bool is_variable() const {
			return true;
		}
	};
}

template<typename T>
constexpr bool is_function_v = std::is_member_function_pointer_v<T> || std::is_function_v<std::remove_pointer_t<T>>;

template <typename T>
struct field_traits 
	: detail::basic_field_traits<is_function_v<T>,T> {
	// lvalue constructor (e.g.
	//	auto a = &Class::method;
	//	auto field_traits f(a);
	//	)
	constexpr field_traits(const T& ptr, std::string_view name) 
		: _ptr(ptr), 
			_name{name.substr(name.find_last_of(":") + 1)}
	{}
	// rvalue constructor (e.g.
	//	auto field_traits f(&Class::method);
	//	)
	constexpr field_traits(T&& ptr, std::string_view name)
		: _ptr(ptr),
		_name{ name.substr(name.find_last_of(":") + 1) }
	{}
	
	T _ptr;
	std::string_view _name;
};

// TODO: I don't know why it makes more sense to use decay here, I need someone with higher level
//template <typename T>
//field_traits(T) -> field_traits<std::decay_t<T>>;
}


