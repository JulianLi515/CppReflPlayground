#pragma once
#include "function_traits.h"
#include "variable_traits.h"

namespace detail {

	template<bool isFunc, typename T>
	struct basic_field_traits;

	// 偏特化，处理函数指针类型
	template<typename T>
	struct basic_field_traits<true, T> : function_traits<T> {
		// 透传 function_traits 的所有成员
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
	};

	// 偏特化，处理变量指针类型
	template<typename T>
	struct basic_field_traits<false, T> : variable_traits<T> {
		// 透传 variable_traits 的所有成员
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
	// 左值匹配构造函数 (传入 auto a where auto a = &Class::method)
	constexpr field_traits(const T& ptr) : _ptr(ptr) {}
	// 右值匹配构造函数 (直接传入 &Class:method)
	constexpr field_traits(T&& ptr): _ptr(ptr){}
	T _ptr;
};

// TODO: 不理解这里为什么decay好一点，需要高人指点
// 强制类型推导为decay类型，防止传左值产生引用类型（将依赖外部左值生命周期）
//template <typename T>
//field_traits(T) -> field_traits<std::decay_t<T>>;


