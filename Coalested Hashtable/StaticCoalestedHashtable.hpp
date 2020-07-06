#pragma once

#include <array>
#include <optional>

namespace algo {

    /**
     *  @brief Class representing Coalested Hash Table stored on stack
     *
     *  @param Key Type of values stored in table
     *  @param Hash Hash function used for hashing keys
     *  @param Size Size of table for static storage
     */
    template <typename Key,
              std::size_t Size,
              typename Hash = std::hash<Key>>
    class StaticCoalestedHashTable {

        struct Node;

    public:

        class Iterator {
        public:

            using difference_type = std::ptrdiff_t;
            using value_type = const Key&;
            using pointer = const Key*;
            using reference = value_type&;
            using iterator_category = std::forward_iterator_tag;

            explicit Iterator(const std::array<Node, Size>& elem, std::size_t index) noexcept;

            auto operator++ () noexcept -> Iterator&;

            auto operator* () const noexcept -> const Key&;

            auto operator == (const Iterator& other) const noexcept -> bool;
            auto operator != (const Iterator& other) const noexcept -> bool;

        private:
            template <typename _Key,
                      std::size_t _Size,
                      typename _Hash>
            friend class StaticCoalestedHashTable;

            const std::array<Node, Size>& _nodes;
            typename std::array<Node, Size>::const_iterator _curr;
        };

    public:

        using value_type = Key;
        using size_type = std::size_t;
        using iterator = Iterator;
        using const_iterator = Iterator;

        explicit StaticCoalestedHashTable() noexcept = default;

        StaticCoalestedHashTable(std::initializer_list<value_type> list) noexcept;

        template<typename Iter>
        StaticCoalestedHashTable (Iter begin, Iter end) noexcept;

        StaticCoalestedHashTable(const StaticCoalestedHashTable& other) = default;
        StaticCoalestedHashTable(StaticCoalestedHashTable&& other) noexcept = default;

        StaticCoalestedHashTable& operator= (const StaticCoalestedHashTable& other) = default;
        StaticCoalestedHashTable& operator= (StaticCoalestedHashTable&& other) noexcept = default;

        /**
         *  @brief Inserts value to hashtable
         *  @return Iterator on inserted value, if insert was unsuccesful - end()
         */
        auto insert (const value_type & value) noexcept -> Iterator;

        /**
         *  @brief Erase value from hashtable
         */
        void erase (const value_type& value);

        /**
         *  @return Iterator on value in hashtable, if value is not in hashtable - end()
         */
        auto find (const value_type& value) const noexcept -> Iterator;

        /**
         *  @return Number of elements in hashtable
         */
        auto size() const noexcept -> size_type;

        /**
         *  @return Max possible size of hashtable
         */
        constexpr static auto max_size() noexcept -> size_type;

        /**
         *  @return Iterator on first element in container
         */
        auto begin() const noexcept -> Iterator;

        /**
         *  @return Iterator on element after last in container
         */
        auto end() const noexcept -> Iterator;


    private:

        auto _hash (const value_type& value);

        struct Node {
            std::optional<value_type> value;
            std::optional<size_type> next;

            auto operator== (const Node& other) const noexcept -> bool;
        };

        std::array<Node, Size> _nodes;
        size_type _size{};
        Hash _hashfunc{};
    };

    template <typename Key, std::size_t N, typename Hash>
    auto operator== (const StaticCoalestedHashTable<Key, N, Hash>& lhs,
                     const StaticCoalestedHashTable<Key, N, Hash>& rhs) -> bool;
}



#include "StaticCoalestedHashtable.tpp"