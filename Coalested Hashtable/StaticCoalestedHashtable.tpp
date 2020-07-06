#pragma once

#include "StaticCoalestedHashtable.hpp"

#include <cassert>
#include <vector>
#include <cstdint>

namespace algo {

    template <typename Key,
             std::size_t Size,
              typename Hash>
    StaticCoalestedHashTable<Key, Size, Hash>::StaticCoalestedHashTable(std::initializer_list<Key> list) noexcept
    {
        for (const auto& elem : list) {
            insert(elem);
        }
    }

    template <typename Key,
             std::size_t Size,
             typename Hash>
    template<typename Iter>
    StaticCoalestedHashTable<Key, Size, Hash>::StaticCoalestedHashTable(Iter begin, Iter end) noexcept
    {
        for (; begin != end; ++begin) {
            insert(*begin);
        }
    }

    template <typename Key,
              std::size_t Size,
              typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::insert(const Key &value) noexcept -> StaticCoalestedHashTable::Iterator
    {
        auto pos = _hash(value);
        bool inserted = false;

        if (_nodes[pos].value) {

            while (_nodes[pos].value) {
                if (*_nodes[pos].value == value) {
                    return Iterator {_nodes,  pos};
                }
                if (_nodes[pos].next) {
                    pos = *_nodes[pos].next;
                }
                else {
                    break;
                }
            }

            for (int32_t i = Size - 1; i >= 0; --i) {
                if (!_nodes[i].value) {
                    _nodes[pos].next = i;
                    _nodes[i].value = value;
                    pos = i;
                    inserted = true;
                    break;
                }
            }
        }
        else {
            _nodes[pos].value = value;
            inserted = true;
        }
        ++_size;
        return inserted ? Iterator{_nodes, pos}
                        : Iterator {_nodes, _nodes.size()};

    }

    template <typename Key,
             std::size_t Size,
             typename Hash>
    void StaticCoalestedHashTable<Key, Size, Hash>::erase(const Key &value) {

        auto pos = _hashfunc(value) % Size;
        auto prev = pos;

        while (_nodes[pos].value && *_nodes[pos].value != value) {
            if (_nodes[pos].next) {
                prev = pos;
                pos = *_nodes[pos].next;
            }
            else {
                return;
            }
        }
        if (!_nodes[pos].value) {
            return;
        }

        std::vector<value_type> to_reinsert;

        auto elem_to_reinsert = _nodes[pos].next;

        {
            _nodes[pos].value = std::nullopt;
            _nodes[pos].next = std::nullopt;
            _nodes[prev].next = std::nullopt;
        }

        while (elem_to_reinsert) {
            to_reinsert.push_back(*_nodes[*elem_to_reinsert].value);
            _nodes[*elem_to_reinsert].value = std::nullopt;
            elem_to_reinsert = std::exchange(_nodes[*elem_to_reinsert].next, std::nullopt);
        }

        for (const auto& elem : to_reinsert) {
            insert(elem);
            --_size;
        }

        --_size;
    }

    template <typename Key,
              std::size_t Size,
              typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::find(const Key &value) const noexcept -> StaticCoalestedHashTable::Iterator
    {
        auto pos = _hashfunc(value) % Size;

        while (_nodes[pos].value && *_nodes[pos].value != value) {
            if (_nodes[pos].next) {
                pos = *_nodes[pos].next;
            }
            else {
                return Iterator {_nodes, _nodes.size()};
            }
        }
        return _nodes[pos].value ? Iterator{_nodes, pos}
                                 : Iterator{_nodes, _nodes.size()};
    }


    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::size() const noexcept -> StaticCoalestedHashTable::size_type
    {
        return _size;
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::begin() const noexcept -> StaticCoalestedHashTable::Iterator
    {
        return Iterator{_nodes, 0};
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::end() const noexcept -> StaticCoalestedHashTable::Iterator
    {
        return Iterator {_nodes, _nodes.size()};
    }

    template<typename Key, std::size_t Size, typename Hash>
    constexpr auto StaticCoalestedHashTable<Key, Size, Hash>::max_size() noexcept -> StaticCoalestedHashTable::size_type {
        return Size;
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::_hash(const value_type &value) {
        return _hashfunc(value) % Size;
    }

    template<typename Key, std::size_t N, typename Hash>
    auto operator==(const StaticCoalestedHashTable<Key, N, Hash>& lhs,
                    const StaticCoalestedHashTable<Key, N, Hash>& rhs) -> bool
    {
        return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename Key, std::size_t Size, typename Hash>
    StaticCoalestedHashTable<Key, Size, Hash>::Iterator::Iterator(const std::array<
            StaticCoalestedHashTable<Key, Size, Hash>::Node, Size>& nodes, std::size_t index) noexcept
            : _nodes (nodes),
              _curr (nodes.cbegin() + index)
    {
        while (!_curr->value && _curr != _nodes.cend()) {
            ++_curr;
        }
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::Iterator::operator++() noexcept -> StaticCoalestedHashTable::Iterator&
    {
        do {
            ++_curr;
        }
        while (!_curr->value && _curr != _nodes.end());
        return *this;
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::Iterator::operator*() const noexcept -> const Key &
    {
        return *_curr->value;
    }


    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::Iterator::operator==(
            const StaticCoalestedHashTable::Iterator &other) const noexcept -> bool
    {
        return &_nodes == &other._nodes && _curr == other._curr;
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::Iterator::
            operator!= (const StaticCoalestedHashTable<Key, Size, Hash>::Iterator& other) const noexcept -> bool
    {
        return !(*this == other);
    }

    template<typename Key, std::size_t Size, typename Hash>
    auto StaticCoalestedHashTable<Key, Size, Hash>::Node::operator==(
            const StaticCoalestedHashTable::Node &other) const noexcept -> bool {
        return value == other.value && next == other.next;
    }
}