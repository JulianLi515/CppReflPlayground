#pragma once
#include <tuple>
namespace my_reflect {
namespace detail {
	// handle common function traits: return type, parameter types
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

// handle global, static functions
template <typename RetT, typename... ParamT>
struct function_traits<RetT(*)(ParamT...)>
	: detail::basic_function_traits<RetT(ParamT...)> {
	static constexpr bool is_member = false;
	static constexpr bool is_const = false;
};

// handle member functions
template <typename ClassT, typename RetT, typename... ParamT>
struct function_traits<RetT(ClassT::*)(ParamT...)>
	: detail::basic_function_traits<RetT(ParamT...)> {
	using class_type = ClassT;
	constexpr static bool is_member = true;
	constexpr static bool is_const = false;
};

// handle const member functions
template <typename ClassT, typename RetT, typename... ParamT>
struct function_traits<RetT(ClassT::*)(ParamT...) const>
	: detail::basic_function_traits<RetT(ParamT...)> {
	using class_type = ClassT;
	static constexpr bool is_member = true;
	static constexpr bool is_const = true;
};
}
