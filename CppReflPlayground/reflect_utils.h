#pragma once
#include <iostream>
#include <tuple>
#include <type_traits>
#include "reflect_core.h"

namespace my_reflect::utils {
    // forward print all
	template <class T>
    void print_all(const T& obj);

	namespace detail
	{

        template<class Derived, class Tuple, std::size_t... I>
        void print_bases_impl(const Derived& obj, std::index_sequence<I...>) {
            (print_all<std::tuple_element_t<I, Tuple>>(static_cast<const std::tuple_element_t<I, Tuple>&>(obj)), ...);
        }

        template<class Derived, class Tuple>
        void print_bases(const Derived& obj) {
            print_bases_impl<Derived, Tuple>(obj, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
        }
	}

	// tool to check if a type has reflection data
    template <typename T, typename = void>
    struct has_reflection : std::false_type {};

    template <typename T>
    struct has_reflection<T, std::void_t<decltype(TypeData<T>::fields)>> : std::true_type {};


    // The "Visit" pattern: Apply a function 'f' to every element in a tuple
    // this limits to a class instance.
    template <typename Tuple, typename Function, size_t... Idx>
    void visitTuple_Impl(Tuple&& tuple, Function&& f, std::index_sequence<Idx...>) {
        // Unary right fold with comma operator: execute f for each element
        (f(std::get<Idx>(std::forward<Tuple>(tuple))), ...);
    }

	// Public interface to visit all elements in a tuple and apply function f
    template <typename Tuple, typename Function>
    void visitTuple(Tuple&& tuple, Function&& f) {
        using PureT = std::remove_reference_t<Tuple>;
        VisitTuple_Impl(
            std::forward<Tuple>(tuple),
            std::forward<Function>(f),
            std::make_index_sequence<std::tuple_size_v<PureT>>{}
        );
    }

    


	// print all field and function names, also recursively for base classes
    template <typename T>
	// T should be the type description object. e.g. auto typeInfo = type_data<Person>();
    void print_all(const T& obj) {
		// 1. handle base case, print recursively for base classes
		using BaseTypes = typename TypeData<T>::base_types;

        detail::print_bases<T, BaseTypes>(obj);
		// 2. print current class's member variables
        auto data = type_data<T>();
        std::apply([&](auto&&... args) {
            // unary fold right 
            ((std::cout << "[Field] " << args._name << " = " << obj.*(args._ptr) << "\n"), ...);
            }, data.fields);

		// 3. print current class's member functions
        std::apply([&](auto&&... args) {
            ((std::cout << "[Func]  " << args._name << " (params: " << args.param_cout() << ")\n"), ...);
            }, data.functions);
    }
}