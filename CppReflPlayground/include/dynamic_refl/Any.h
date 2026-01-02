//
// Created by qianq on 1/1/2026.
//
// Type-erased wrapper with reflection support
// Supports copy, move, reference, and const reference storage semantics

#pragma once

#include "Type.h"
#include <cassert>
#include <utility>
#include <optional>
#include <stdexcept>

namespace my_reflect::dynamic_refl {

// Forward declaration
template <typename T>
const Type* GetType();

class Any final {
public:
	enum class storage_type {
		Empty, Copy, Move, Ref, ConstRef
	};

	struct operations {
		Any(*copy)(const Any&) = nullptr;
		Any(*move)(Any&) = nullptr;
		void(*destroy)(Any&) = nullptr;
	};

	const Type* typeInfo = nullptr;
	void* payload = nullptr;
	storage_type storageType = storage_type::Empty;
	operations ops;

	Any() = default;
	Any(const Any& other);
	Any(Any&& other) noexcept;
	Any& operator=(const Any& other);
	Any& operator=(Any&& other) noexcept;
	~Any();

	bool empty() const;
	const Type* type() const;
	storage_type storage() const;

	// Invoke member function by name
	template<typename... Args>
	Any invoke(const std::string& funcName, Args&&... args);

	// Invoke member function by index
	template<typename... Args>
	Any invoke(size_t funcIndex, Args&&... args);
};

template <typename T>
struct operations_traits {
	static Any copy(const Any& elem) {
		assert(elem.typeInfo == GetType<T>());
		Any returnValue;
		returnValue.typeInfo = elem.typeInfo;
		returnValue.payload = new T(*static_cast<const T*>(elem.payload));
		returnValue.storageType = Any::storage_type::Copy;
		returnValue.ops = elem.ops;
		return returnValue;
	}

	static Any move(Any& elem) {
		assert(elem.typeInfo == GetType<T>());
		Any returnValue;
		returnValue.typeInfo = elem.typeInfo;
		returnValue.payload = new T(std::move(*static_cast<T*>(elem.payload)));
		returnValue.storageType = Any::storage_type::Move;
		elem.storageType = Any::storage_type::Empty;
		returnValue.ops = elem.ops;
		return returnValue;
	}

	static void destroy(Any& elem) {
		assert(elem.typeInfo == GetType<T>());
		delete static_cast<T*>(elem.payload);
		elem.storageType = Any::storage_type::Empty;
		elem.payload = nullptr;
		elem.typeInfo = nullptr;
	}
};

template <typename T>
Any make_copy(const T& elem) {
	Any returnValue;
	returnValue.payload = new T(elem);
	returnValue.typeInfo = GetType<T>();
	returnValue.storageType = Any::storage_type::Copy;
	if constexpr (std::is_copy_constructible_v<T>) {
		returnValue.ops.copy = &operations_traits<T>::copy;
	}
	if constexpr (std::is_move_constructible_v<T>) {
		returnValue.ops.move = &operations_traits<T>::move;
	}
	if constexpr (std::is_destructible_v<T>) {
		returnValue.ops.destroy = &operations_traits<T>::destroy;
	}
	return returnValue;
}

template <typename T>
Any make_move(T&& elem) {
	Any returnValue;
	returnValue.payload = new T(std::move(elem));
	returnValue.typeInfo = GetType<T>();
	returnValue.storageType = Any::storage_type::Move;
	if constexpr (std::is_copy_constructible_v<T>) {
		returnValue.ops.copy = &operations_traits<T>::copy;
	}
	if constexpr (std::is_move_constructible_v<T>) {
		returnValue.ops.move = &operations_traits<T>::move;
	}
	if constexpr (std::is_destructible_v<T>) {
		returnValue.ops.destroy = &operations_traits<T>::destroy;
	}
	return returnValue;
}

template <typename T>
Any make_ref(T& elem) {
	Any returnValue;
	returnValue.payload = &elem;
	returnValue.typeInfo = GetType<T>();
	returnValue.storageType = Any::storage_type::Ref;
	if constexpr (std::is_copy_constructible_v<T>) {
		returnValue.ops.copy = &operations_traits<T>::copy;
	}
	if constexpr (std::is_move_constructible_v<T>) {
		returnValue.ops.move = &operations_traits<T>::move;
	}
	if constexpr (std::is_destructible_v<T>) {
		returnValue.ops.destroy = &operations_traits<T>::destroy;
	}
	return returnValue;
}

template <typename T>
Any make_cref(const T& elem) {
	Any returnValue;
	returnValue.payload = const_cast<T*>(&elem);
	returnValue.typeInfo = GetType<T>();
	returnValue.storageType = Any::storage_type::ConstRef;
	if constexpr (std::is_copy_constructible_v<T>) {
		returnValue.ops.copy = &operations_traits<T>::copy;
	}
	if constexpr (std::is_move_constructible_v<T>) {
		returnValue.ops.move = &operations_traits<T>::move;
	}
	if constexpr (std::is_destructible_v<T>) {
		returnValue.ops.destroy = &operations_traits<T>::destroy;
	}
	return returnValue;
}

template <typename T>
T* any_cast(Any& elem) {
	if (elem.typeInfo == GetType<T>()) {
		return static_cast<T*>(elem.payload);
	}
	return nullptr;
}

template <typename T>
const T* any_cast(const Any& elem) {
	if (elem.typeInfo == GetType<T>()) {
		return static_cast<const T*>(elem.payload);
	}
	return nullptr;
}

// ========== Layer 1: Generic Any Operations ==========

template<typename T>
bool any_set(Any& any, const T& value) {
	if (any.storage() == Any::storage_type::ConstRef) {
		throw std::runtime_error("Cannot modify const reference Any");
	}
	if (auto* ptr = any_cast<T>(any)) {
		*ptr = value;
		return true;
	}
	return false;
}

template<typename T>
std::optional<T> any_get(const Any& any) {
	if (const auto* ptr = any_cast<T>(any)) {
		return *ptr;
	}
	return std::nullopt;
}

} // namespace my_reflect::dynamic_refl
