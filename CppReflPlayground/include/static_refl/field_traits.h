#pragma once
#include "function_traits.h"
#include "variable_traits.h"
namespace my_reflect::static_refl {
namespace detail {

	template<bool isFunc, typename T>
	struct basic_field_traits; // main template

	// partial specialization for function pointer types
	template<typename T>
	struct basic_field_traits<true, T> : function_traits<T> {
		// pass through all members of function_traits
		using traits = function_traits<T>;


		static constexpr bool is_member() {
			return traits::is_member;
		}

		static constexpr bool is_const() {
			return traits::is_const;
		}

		static bool is_function() {
			return true;
		}

		static bool is_variable() {
			return false;
		}

		static constexpr size_t param_cout()
		{
			return traits::parameter_count;
		}
	};

	// partial specialization for variable pointer types
	template<typename T>
	struct basic_field_traits<false, T> : variable_traits<T> {
		// pass through all members of variable_traits
		using traits = variable_traits<T>;
	
		static constexpr bool is_member() {
			return traits::is_member;
		}

		static constexpr bool is_const() {
			return traits::is_const;
		}

		static constexpr bool is_function() {
			return false;
		}

		static constexpr bool is_variable() {
			return true;
		}
	};
}

template<typename T>
constexpr bool is_function_v = std::is_member_function_pointer_v<T> || std::is_function_v<std::remove_pointer_t<T>>;

template <typename T>
struct field_traits 
	: detail::basic_field_traits<is_function_v<T>,T> {
	
	// this is supposed to used in the macro only we just a one pass by value constructor.
	constexpr field_traits(T ptr, std::string_view name)
		: ptr_(ptr), name_{ name } {
	}
	
	T ptr_;
	std::string_view name_;
};

}


