#pragma once
#include <type_traits>
namespace my_reflect::static_refl {
namespace detail {
	// handle common variable traits: type, constness
	template <typename T>
	struct basic_variable_traits {
		using type = T;
		static constexpr bool is_const = std::is_const_v<T>;
	};
}

template <typename T>
struct variable_traits; // primary template

// partial specialization for global/static variables
template <typename T>
struct variable_traits<T*>
	: detail::basic_variable_traits<T> {
	using pointer_type = T*;
	static constexpr bool is_member = false;
};

// partial specialization for member variables
template <typename ClassT, typename T>
struct variable_traits<T ClassT::*>
	: detail::basic_variable_traits<T> {
	using pointer_type = T ClassT::*;
	using class_type = ClassT;
	static constexpr bool is_member = true;
};

}

