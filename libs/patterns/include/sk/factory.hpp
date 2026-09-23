#pragma once

#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace sk {

/// Registry-based factory: maps a key to a function that creates a concrete
/// product behind the Base interface.
///
/// Compared with the textbook `switch (option) { case 1: return new A; ... }`
/// this keeps the factory closed for modification (Open/Closed principle):
/// new products are registered from outside without editing the factory, and
/// ownership is explicit because products are returned as std::unique_ptr.
template <typename Base, typename Key, typename... CtorArgs>
class Factory {
    static_assert(std::has_virtual_destructor_v<Base>,
                  "Base must have a virtual destructor to be deleted through a base pointer");

public:
    using Creator = std::function<std::unique_ptr<Base>(CtorArgs...)>;

    /// Returns false if the key is already registered.
    bool register_creator(Key key, Creator creator) {
        return creators_.emplace(std::move(key), std::move(creator)).second;
    }

    template <typename Derived>
        requires std::is_base_of_v<Base, Derived> && std::is_constructible_v<Derived, CtorArgs...>
    bool register_type(Key key) {
        return register_creator(std::move(key), [](CtorArgs... args) -> std::unique_ptr<Base> {
            return std::make_unique<Derived>(std::forward<CtorArgs>(args)...);
        });
    }

    /// Returns nullptr for an unknown key.
    [[nodiscard]] std::unique_ptr<Base> create(const Key& key, CtorArgs... args) const {
        const auto it = creators_.find(key);
        if (it == creators_.end()) {
            return nullptr;
        }
        return it->second(std::forward<CtorArgs>(args)...);
    }

    bool contains(const Key& key) const { return creators_.contains(key); }

    std::vector<Key> keys() const {
        std::vector<Key> result;
        result.reserve(creators_.size());
        for (const auto& [key, creator] : creators_) {
            result.push_back(key);
        }
        return result;
    }

private:
    std::map<Key, Creator> creators_;
};

} // namespace sk
