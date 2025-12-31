#pragma once
#include "container_traits.h"
#include "function_traits.h"
#include "variable_traits.h"
namespace my_reflect::static_refl {
	enum class Kind { Function, Variable, Container };
namespace detail {

	template<Kind kind, typename T>
	struct basic_field_traits; // main template

	// partial specialization for function pointer types
	template<typename T>
	struct basic_field_traits<Kind::Function, T> : function_traits<T> {
		// pass through all members of function_traits
		using traits = function_traits<T>;


		static constexpr bool is_member() {
			return traits::is_member;
		}

		static constexpr bool is_const() {
			return traits::is_const;
		}

		static constexpr bool is_function() {
			return true;
		}

		static constexpr bool is_variable() {
			return false;
		}

		static constexpr bool is_plain_variable() {
			return false;
		}

		static constexpr bool is_container() {
			return false;
		}

		static constexpr size_t param_count()
		{
			return traits::parameter_count;
		}
	};


	template <typename T>
	struct basic_field_traits<Kind::Container, T> : detail::container_traits<T> {
		using traits = detail::container_traits<T>;

		static constexpr bool is_member() {
			return traits::is_member;
		}

		static constexpr bool is_function() {
			return false;
		}

		static constexpr bool is_variable() {
			return true;  // Container is also Variable
		}

		static constexpr bool is_plain_variable() {
			return false;  // But not a plain variable
		}

		static constexpr bool is_container() {
			return true;
		}

		static constexpr ContainerKind container_kind() {
			return traits::kind;
		}
	};
	// partial specialization for variable pointer types
	template<typename T>
	struct basic_field_traits<Kind::Variable, T> : variable_traits<T> {
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

		static constexpr bool is_plain_variable() {
			return true;  // Plain variable
		}

		static constexpr bool is_container() {
			return false;
		}
	};
}

template<typename T>
constexpr bool is_function_v = std::is_member_function_pointer_v<T> || std::is_function_v<std::remove_pointer_t<T>>;


template <typename T>
constexpr auto get_kind_v() {
	if constexpr (is_function_v<T>) return Kind::Function;
	else if constexpr (is_container_v<T>) return Kind::Container;
	else return Kind::Variable;
}


template <typename T>
struct field_traits
	: detail::basic_field_traits<get_kind_v<T>(), T> {

	// this is supposed to used in the macro only we just a one pass by value constructor.
	constexpr field_traits(T ptr, std::string_view name)
		: ptr_(ptr), name_(strip_scope(name)) {
	}

	T ptr_;
	std::string_view name_;

private:
	// Helper function to strip scope prefix like "&Person::" from "&Person::name"
	static constexpr std::string_view strip_scope(std::string_view full_name) {
		auto pos = full_name.rfind(':');
		if (pos != std::string_view::npos) {
			return full_name.substr(pos + 1);
		}
		return full_name;
	}
};

}


