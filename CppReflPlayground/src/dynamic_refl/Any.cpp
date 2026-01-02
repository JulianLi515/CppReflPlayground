//
// Created by qianq on 1/1/2026.
//
// Implementation file for any class

#include "../../include/dynamic_refl/Any.h"

namespace my_reflect::dynamic_refl {

// Any constructors and destructor
Any::Any(const Any& other)
	: typeInfo(other.typeInfo), storageType(other.storageType), ops(other.ops) {
	if (ops.copy) {
		auto new_any = ops.copy(other);
		payload = new_any.payload;
		new_any.payload = nullptr;
		new_any.ops.destroy = nullptr;
	} else {
		storageType = storage_type::Empty;
		typeInfo = nullptr;
	}
}

Any::Any(Any&& other) noexcept
	: typeInfo(other.typeInfo)
	, payload(other.payload)
	, storageType(other.storageType)
	, ops(other.ops) {
	other.typeInfo = nullptr;
	other.payload = nullptr;
	other.storageType = storage_type::Empty;
	other.ops = {};
}

Any& Any::operator=(const Any& other) {
	if (this != &other) {
		// Destroy current content
		if (ops.destroy != nullptr) {
			if (storageType == storage_type::Copy || storageType == storage_type::Move) {
				ops.destroy(*this);
			}
		}

		// Copy from other
		typeInfo = other.typeInfo;
		storageType = other.storageType;
		ops = other.ops;

		if (ops.copy) {
			auto new_any = ops.copy(other);
			payload = new_any.payload;
			new_any.payload = nullptr;
			new_any.ops.destroy = nullptr;
		} else {
			storageType = storage_type::Empty;
			typeInfo = nullptr;
			payload = nullptr;
		}
	}
	return *this;
}

Any& Any::operator=(Any&& other) noexcept {
	if (this != &other) {
		// Destroy current content
		if (ops.destroy != nullptr) {
			if (storageType == storage_type::Copy || storageType == storage_type::Move) {
				ops.destroy(*this);
			}
		}

		// Move from other
		typeInfo = other.typeInfo;
		payload = other.payload;
		storageType = other.storageType;
		ops = other.ops;

		other.typeInfo = nullptr;
		other.payload = nullptr;
		other.storageType = storage_type::Empty;
		other.ops = {};
	}
	return *this;
}

Any::~Any() {
	if (ops.destroy != nullptr) {
		if (storageType == storage_type::Copy || storageType == storage_type::Move) {
			ops.destroy(*this);
		}
	}
}

bool Any::empty() const {
	return storageType == storage_type::Empty; 
}

const Type* Any::type() const {
	return typeInfo; 
}

Any::storage_type Any::storage() const {
	return storageType; 
}

} // namespace my_reflect::dynamic_refl
