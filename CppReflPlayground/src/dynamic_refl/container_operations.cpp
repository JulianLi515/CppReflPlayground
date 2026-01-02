//
// Created by qianq on 1/1/2026.
//

#include "../../include/dynamic_refl/container_operations.h"
#include "../../include/dynamic_refl/MemberContainer.h"

namespace my_reflect::dynamic_refl {

namespace container_ops {

    size_t Size(const MemberContainer& containerInfo, const Any& container) {
        if (!containerInfo.ops_.size) {
            throw std::runtime_error("Container does not support size() operation");
        }
        return containerInfo.ops_.size(container);
    }

    void Clear(const MemberContainer& containerInfo, Any& container) {
        if (container.storage() == Any::storage_type::ConstRef) {
            throw std::runtime_error("Cannot modify const reference Any");
        }
        if (!containerInfo.ops_.clear) {
            throw std::runtime_error("Container does not support clear() operation");
        }
        containerInfo.ops_.clear(container);
    }

    bool Push(const MemberContainer& containerInfo, Any& container, const Any& value) {
        if (container.storage() == Any::storage_type::ConstRef) {
            throw std::runtime_error("Cannot modify const reference Any");
        }
        if (!containerInfo.ops_.push) {
            throw std::runtime_error("Container does not support push/insert operation");
        }
        return containerInfo.ops_.push(container, value);
    }

    Any At(const MemberContainer& containerInfo, const Any& container, size_t index) {
        if (!containerInfo.ops_.at) {
            throw std::runtime_error("Container does not support at() operation (only for vectors)");
        }
        return containerInfo.ops_.at(container, index);
    }

    bool InsertKV(const MemberContainer& containerInfo, Any& map, const Any& key, const Any& value) {
        if (map.storage() == Any::storage_type::ConstRef) {
            throw std::runtime_error("Cannot modify const reference Any");
        }
        if (!containerInfo.ops_.insert_kv) {
            throw std::runtime_error("Container does not support insert key-value operation (only for maps)");
        }
        return containerInfo.ops_.insert_kv(map, key, value);
    }

    Any GetValue(const MemberContainer& containerInfo, const Any& map, const Any& key) {
        if (!containerInfo.ops_.get_value) {
            throw std::runtime_error("Container does not support get value operation (only for maps)");
        }
        return containerInfo.ops_.get_value(map, key);
    }

    bool ContainsKey(const MemberContainer& containerInfo, const Any& map, const Any& key) {
        if (!containerInfo.ops_.contains_key) {
            throw std::runtime_error("Container does not support contains key operation (only for maps)");
        }
        return containerInfo.ops_.contains_key(map, key);
    }

} // namespace container_ops

} // namespace my_reflect::dynamic_refl
