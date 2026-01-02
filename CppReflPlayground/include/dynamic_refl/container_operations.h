//
// Created by qianq on 1/1/2026.
//
// Container operations for Any type
// Provides generic operations for std::vector, std::set, std::map wrapped in Any

#pragma once

#include "Any.h"
#include "MemberContainer.h"
#include <vector>
#include <set>
#include <map>
#include <stdexcept>

namespace my_reflect::dynamic_refl {

namespace container_ops {

    // ========== Operations using MemberContainer (recommended) ==========

    // Get container size
    size_t Size(const MemberContainer& containerInfo, const Any& container);

    // Clear container
    void Clear(const MemberContainer& containerInfo, Any& container);

    // Push element to vector or insert to set
    bool Push(const MemberContainer& containerInfo, Any& container, const Any& value);

    // Get element at index (vector only)
    Any At(const MemberContainer& containerInfo, const Any& container, size_t index);

    // Insert key-value pair into map
    bool InsertKV(const MemberContainer& containerInfo, Any& map, const Any& key, const Any& value);

    // Get value by key from map
    Any GetValue(const MemberContainer& containerInfo, const Any& map, const Any& key);

    // Check if map contains key
    bool ContainsKey(const MemberContainer& containerInfo, const Any& map, const Any& key);

} // namespace container_ops

} // namespace my_reflect::dynamic_refl
