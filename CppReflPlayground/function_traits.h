#pragma once
#include <tuple>

namespace detail {
	// 处理一些通用类型：返回值类型和参数类型
	template <typename T>
	struct basic_function_traits;

	template <typename RetT, typename... ParamT>
	struct basic_function_traits<RetT(ParamT...)> {
		using return_type = RetT;
		using parameter_type = std::tuple<ParamT...>;
	};
}

template <typename T>
struct function_traits;

// 处理全局函数,静态函数
template <typename RetT, typename... ParamT>
struct function_traits<RetT(*)(ParamT...)> 
	: detail::basic_function_traits<RetT(ParamT...)> {
	static constexpr bool is_member = false;
	static constexpr bool is_const = false;
};

// 处理成员函数
template <typename ClassT, typename RetT, typename... ParamT>
struct function_traits<RetT(ClassT::*)(ParamT...)> 
	: detail::basic_function_traits<RetT(ParamT...)> {
	using class_type = ClassT;
	constexpr static bool is_member = true;
	constexpr static bool is_const = false;
};

// 处理成员const 函数
template <typename ClassT, typename RetT, typename... ParamT>
struct function_traits<RetT(ClassT::*)(ParamT...) const> 
	: detail::basic_function_traits<RetT(ParamT...)> {
	using class_type = ClassT;
	static constexpr bool is_member = true;
	static constexpr bool is_const = true;
};
