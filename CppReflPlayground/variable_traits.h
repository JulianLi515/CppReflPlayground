#pragma once
#include <type_traits>

namespace detail {
	// 处理通用类型：变量类型，是否常量
	template <typename T>
	struct basic_variable_traits {
		using type = T;
		static constexpr bool is_const = std::is_const_v<T>;
	};
}

template <typename T>
struct variable_traits;

// 处理全局变量，静态变量
template <typename T>
struct variable_traits<T*>
	: detail::basic_variable_traits<T> {
	using pointer_type = T*;
	static constexpr bool is_member = false;
};

// 处理成员变量
template <typename ClassT, typename T>
struct variable_traits<T ClassT::*>
	: detail::basic_variable_traits<T> {
	using pointer_type = T ClassT::*;
	using class_type = ClassT;
	static constexpr bool is_member = true;
};



