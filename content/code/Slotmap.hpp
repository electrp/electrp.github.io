/**
 *  @author Will Bender
 */

#pragma once
#include <optional>
#include <stdexcept>
#include <vector>
#include <stdint.h>

/**
 * Slotmap Data Structure
 *
 *  Associative data structure where keys are provided upon data instertion.
 *  Functions as an im-place allocator/arena.
 * 
 * @tparam Value Value type
 */

/**
 * Key type returned by SlotMaps, referenced using `SlotMap<Value>::Key`
 *
 * - Index: Index into the `SlotMap`
 * - Generation: The number of times the index in the `SlotMap` has been used.
 *      - Intended to allow for comparisons between two keys, to ensure reused slots to not have identical Keys.
 *
 * DO NOT MODIFY DATA DIRECTLY.
 */
template<typename IndexType = uint32_t, typename GenerationType = uint32_t>
class SlotKey
{ 
public:
    GenerationType mGeneration;
    IndexType mIndex;

    SlotKey() :
        mGeneration(std::numeric_limits<GenerationType>::max()), mIndex(std::numeric_limits<IndexType>::max()) {};
    
    /**
     * Construct Key. Do not do this unless you know what you are doing.
     * @param generation generation 
     * @param index index
     */
    SlotKey(unsigned generation, unsigned index);

    /**
     * Copy Key
     * @param other Other Key
     */
    SlotKey(const SlotKey& other) = default;
    
    /**
     * Copy key 
     * @param other Other key
     * @return Key
     */
    SlotKey& operator=(const SlotKey& other) = default;

    /**
     * Return the generation of the current key (See `Key`)
     * @return Generation
     */
    GenerationType GetGeneration() const;
    /**
     * Return the index of the current key (See `Key`)
     * @return Index
     */
    IndexType GetIndex() const;

    friend bool operator==(const SlotKey& lhs, const SlotKey& rhs)
    {
        return lhs.mGeneration == rhs.mGeneration
            && lhs.mIndex == rhs.mIndex;
    }

    friend bool operator!=(const SlotKey& lhs, const SlotKey& rhs)
    {
        return !(lhs == rhs);
    }
};

template <typename _Value, typename _IndexType = uint32_t, typename _GenerationType = uint32_t>
class SlotMap 
{
public:
    using Value = _Value;
    using IndexType = _IndexType;
    using GenerationType = _GenerationType;
    using Key = SlotKey<IndexType, GenerationType>;

    /**
     * Key with type verification, to ensure keys aren't used outside their
     * allocated SlotMaps
     */
    struct TypedKey
    {
        Key mKey;
        
        /**
         * Copy Key
         * @param other Other Key
         */
        TypedKey() : mKey() {};
        
        TypedKey(const TypedKey& other);

        TypedKey(GenerationType generation, IndexType index) :
            mKey(generation, index) {}
        
        TypedKey(Key key) : mKey(key) {}

        /**
         * Copy key 
         * @param other Other key
         * @return Key
         */
        TypedKey& operator=(const TypedKey& other);;

        /**
         * Return the generation of the current key (See `Key`)
         * @return Generation
         */
        GenerationType GetGeneration() const;
        /**
         * Return the index of the current key (See `Key`)
         * @return Index
         */
        IndexType GetIndex() const;

        friend bool operator==(const TypedKey& lhs, const TypedKey& rhs)
        {
            return lhs.mKey == rhs.mKey;
        }

        friend bool operator!=(const TypedKey& lhs, const TypedKey& rhs)
        {
            return !(lhs == rhs);
        }

    private:
    };

private:
    /**
     * Internal node structure
     * Contains the data for the SlotMap
     */
    struct Node
    {
        Node(Value&& data, unsigned generation);
        Node(const Node& other) = delete;
        Node(Node&& other) noexcept;
        Node& operator=(const Node& other) = delete;
        Node& operator=(Node&& other) noexcept = delete;

        ~Node()
        {
            if (mHasData) uData.~Value();
        }

        union
        {
            Value uData;
            size_t uNextFree;
        };

        unsigned mGeneration = 0;
        bool mHasData;
    };

    // limited to sizeof(IndexType) byte indices
    std::vector<Node> mNodes;
    size_t mFreeList;
    mutable uint32_t mSize;
    
public:
    

    // Iterator for `SlotMap`
    class iterator
    {
        iterator(SlotMap* map, size_t index) : mPtr(map), mIndex(index)
        {
        }
        SlotMap* mPtr;
        size_t mIndex;

        friend SlotMap;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Value;
        using pointer = Value*;
        using reference = Value&;

        iterator(const iterator& other) = default;
        iterator(iterator&& other) noexcept = default;
        iterator& operator=(const iterator& other) = default;
        iterator& operator=(iterator&& other) noexcept = default;
        ~iterator() = default;

        // Returns a data reference to the corresponding data contained within the `SlotMap`
        reference operator*();
        // Returns the data pointer to the corresponding data contained within the `SlotMap`
        pointer operator->();
        // Returns a const data reference to the corresponding data contained within the `SlotMap`
        reference operator*() const;
        // Returns a const data pointer to the corresponding data contained within the `SlotMap`
        pointer operator->() const;
        // Increments the iterator;
        iterator& operator++();
        // Increments a new iterator;
        iterator operator++(int);

        reference get();
        reference get() const;
        
        Key GetKey();

        // Equivalence function
        friend bool operator==(const iterator& a, const iterator& b)
        {
            return a.mPtr == b.mPtr && a.mIndex == b.mIndex;
        }

        // Inequality function
        friend bool operator!=(const iterator& a, const iterator& b)
        {
            return !(a == b);
        }
    };

    // Const iterator for `SlotMap`
    class const_iterator
    {
        const_iterator(SlotMap* map, size_t index) : mPtr(map), mIndex(index)
        {
        }

        const SlotMap* mPtr;
        size_t mIndex;

        friend SlotMap;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Value;
        using pointer = const Value*;
        using reference = const Value&;

        const_iterator(const const_iterator& other) = default;
        const_iterator(const_iterator&& other) noexcept = default;
        const_iterator& operator=(const const_iterator& other) = default;
        const_iterator& operator=(const_iterator&& other) noexcept = default;
        ~const_iterator() = default;

        // Returns a const reference to the corresponding data contained within `SlotMap`
        reference operator*() const;
        // Returns a const pointer to the corresponding data contained within `SlotMap`
        pointer operator->() const;
        // Increments the iterator
        iterator& operator++();
        // Increments a new iterator
        iterator operator++(int);

        reference get() const;

        Key GetKey();

        // Equality Function
        friend bool operator==(const iterator& a, const iterator& b)
        {
            return a.mPtr == b.mPtr && a.mIndex == b.mIndex;
        }

        // Inequality Function
        friend bool operator!=(const iterator& a, const iterator& b)
        {
            return !(a == b);
        }
    };

    // Create a new `SlotMap`
    SlotMap() : mNodes(), mFreeList(SIZE_MAX)
    {
    }
    // Copy an iterator into a `SlotMap`
    template <typename I>
    SlotMap(I& data) : mNodes(data.begin(), data.end()), mFreeList(SIZE_MAX)
    {
    }
    // Move `SlotMap` data into another 
    SlotMap(SlotMap&& other) noexcept : mNodes(std::move(other.mNodes)), mFreeList(other.mFreeList)
    {
    }

    // Copy a SlotMap from one to another
    SlotMap(const SlotMap& other) : mNodes(other.mNodes), mFreeList(other.mFreeList)
    {
    }

    SlotMap(std::initializer_list<Value> initializerList) : mNodes(initializerList), mFreeList(SIZE_MAX)
    {
    }

    /**
     * Insert new data, returning a `TypedKey`
     * @param data Value to insert
     * @return Key referring to data
     */
    TypedKey insert(Value&& data);
    /**
     * Remove data corresponding to given `Key`
     * @param key Data to remove
     */
    void remove(Key key);
    void remove(TypedKey key);
    /**
     * Attempt to remove data corresponding to given `Key`, return false on error/fail.
     * @param key Data to remove
     * @return Success
     */
    bool TryRemove(Key key);
    bool TryRemove(TypedKey key);
    /**
     * Check if data is contained within the `SlotMap`
     * @param key Data to check
     * @return Contained
     */
    bool contains(const Key& key);
    bool contains(const TypedKey& key);
    /**
     * Remove data in `SlotMap` at location
     * @param iter Iterator to remove at
     */
    iterator erase(iterator& iter);
    /**
     * Remove data in `SlotMap` at location
     * @param iter Iterator to remove at
     */
    const_iterator erase(const_iterator& iter);

    /**
     * Return the iterator at the start of the `SlotMap`
     * @return Iterator at the start
     */
    iterator begin();
    /**
     * Return the iterator at the start of the `SlotMap`
     * @return Constant iterator at the start
     */
    const_iterator begin() const;
    /**
     * Return the iterator at the end of the `SlotMap`
     * @return Iterator at the end
     */
    iterator end();
    /**
     * Return the iterator at the end of the `SlotMap`
     * @return Const iterator at the end
     */
    const_iterator end() const;

    /**
     * Index into the slotmap
     * @param key Key to index with
     * @return Iterator at the location
     */
    iterator operator[](const Key& key);
    iterator operator[](const TypedKey& key);
    /**
     * Index into the slotmap
     * @param key Key to index with
     * @return Const iterator at the location
     */
    const_iterator operator[](const Key& key) const;
    const_iterator operator[](const TypedKey& key) const;
    /**
     * Find a value in the slotmap
     * @param key Key to index with
     * @return Iterator at the location
     */
    iterator find(const Key& key);
    iterator find(const TypedKey& key);
    /**
     * Find a value in the slotmap
     * @param key Key to index with
     * @return Const iterator at the location
     */
    const_iterator find(const Key& key) const;
    const_iterator find(const TypedKey& key) const;

    GenerationType GetGeneration(const IndexType& key) const;

    uint32_t Size() const;

};


template<typename IndexType = uint32_t, typename GenerationType = uint32_t>
class KeyHasher
{
public:
    std::size_t operator()(const SlotKey<IndexType, GenerationType>& key) const
    {
        size_t seed = std::hash<GenerationType>()(key.GetGeneration());
        seed ^= std::hash<IndexType>()(key.GetIndex()) 
            + 0x9e3779b9 + (seed<<6) + (seed>>2);
        return seed;
    }
};

template<typename Value, typename GenerationType = uint32_t, typename IndexType = uint32_t>
class TypedKeyHasher
{
public:
    std::size_t operator()(const typename SlotMap<Value, IndexType, GenerationType>::TypedKey& key) const
    {
        size_t seed = std::hash<GenerationType>()(key.GetGeneration());
        seed ^= std::hash<IndexType>()(key.GetIndex()) 
            + 0x9e3779b9 + (seed<<6) + (seed>>2);
        return seed;
    }
};

template <typename Value, typename GenerationType, typename IndexType>
bool operator==(const typename SlotMap<Value, GenerationType, IndexType>::iterator& a,
                const typename SlotMap<Value, GenerationType, IndexType>::iterator& b)
{
    return a.ptr == b.ptr && a.index == b.index;
}

template <typename Value, typename GenerationType, typename IndexType>
bool operator!=(const typename SlotMap<Value>::iterator& a, const typename SlotMap<Value>::iterator& b)
{
    return !(a == b);
}

template <typename Value, typename GenerationType, typename IndexType>
bool operator==(const typename SlotMap<Value>::const_iterator& a, const typename SlotMap<Value>::const_iterator& b)
{
    return a.ptr == b.ptr && a.index == b.index;
}

template <typename Value, typename GenerationType, typename IndexType>
bool operator!=(const typename SlotMap<Value>::const_iterator& a, const typename SlotMap<Value>::const_iterator& b)
{
    return !(a == b);
}

template <typename GenerationType, typename IndexType>
SlotKey<GenerationType, IndexType>::SlotKey(unsigned generation, unsigned index): mGeneration(generation),
    mIndex(index)
{
}

template <typename GenerationType, typename IndexType>
IndexType SlotKey<GenerationType, IndexType>::GetGeneration() const
{
    return mGeneration;
}

template <typename GenerationType, typename IndexType>
GenerationType SlotKey<GenerationType, IndexType>::GetIndex() const
{
    return mIndex;
}

template <typename Value, typename IndexType, typename GenerationType>
SlotMap<Value, IndexType, GenerationType>::TypedKey::TypedKey(const TypedKey& other) : mKey(other.mKey)
{
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::TypedKey& SlotMap<Value, IndexType, GenerationType>::TypedKey::
operator=(const TypedKey& other)
{
    mKey = other.mKey;
    return *this;
}

template <typename Value, typename IndexType, typename GenerationType>
GenerationType SlotMap<Value, IndexType, GenerationType>::TypedKey::GetGeneration() const
{
    return mKey.GetGeneration();
}

template <typename Value, typename IndexType, typename GenerationType>
IndexType SlotMap<Value, IndexType, GenerationType>::TypedKey::GetIndex() const
{
    return mKey.GetIndex();
}

template <typename Value, typename IndexType, typename GenerationType>
SlotMap<Value, IndexType, GenerationType>::Node::Node(Value&& data, unsigned generation): uData(data), mGeneration(generation), mHasData(true)
{
}

template <typename Value, typename IndexType, typename GenerationType>
SlotMap<Value, IndexType, GenerationType>::Node::Node(Node&& other) noexcept
{
    mHasData = other.mHasData;
    mGeneration = other.mGeneration;
    if(mHasData)
    {
        uData = other.uData;
    }
    else
    {
        uNextFree = other.uNextFree;
    }
}

template <typename Value, typename GenerationType, typename IndexType>
bool operator==(const typename SlotMap<Value, GenerationType, IndexType>::Key& lhs,
                const typename SlotMap<Value>::Key& rhs)
{
    return lhs.mGeneration == rhs.mGeneration
        && lhs.mIndex == rhs.mIndex;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator::reference SlotMap<
    Value, GenerationType, IndexType>::iterator::operator*()
{
    return mPtr->mNodes[mIndex].uData;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator::pointer SlotMap<
    Value, GenerationType, IndexType>::iterator::operator->()
{
    return &mPtr->mNodes[mIndex].uData;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator::reference SlotMap<
    Value, GenerationType, IndexType>::iterator::operator*() const
{
    return mPtr->mNodes[mIndex].uData;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator::pointer SlotMap<
    Value, GenerationType, IndexType>::iterator::operator->() const
{
    return &mPtr->mNodes[mIndex].uData;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator& SlotMap<
    Value, GenerationType, IndexType>::iterator::operator++()
{
    if (mIndex >= mPtr->mNodes.size())
    {
        throw std::runtime_error("Incremented iterator on end");
    }

    mIndex++;

    while (!mPtr->mNodes[mIndex].mHasData)
    {
        if (mIndex >= mPtr->mNodes.size())
        {
            return *this;
        }

        mIndex++;
    }
    return *this;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<
    Value, GenerationType, IndexType>::iterator::operator++(int)
{
    iterator out = *this;
    ++(*this);
    return out;
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::iterator::reference SlotMap<Value, IndexType, GenerationType>::
iterator::get()
{
    return **this;
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::iterator::reference SlotMap<Value, IndexType, GenerationType>::
iterator::get() const
{
    return **this;
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::Key SlotMap<Value, IndexType, GenerationType>::iterator::GetKey()
{
    auto& map = mPtr->mNodes[mIndex];
    return Key{map.mGeneration, mIndex};
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator::reference SlotMap<
    Value, GenerationType, IndexType>::const_iterator::operator*() const
{
    return mPtr->mNodes[mIndex].uData;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator::pointer SlotMap<
    Value, GenerationType, IndexType>::const_iterator::operator->() const
{
    return &mPtr->mNodes[mIndex].uData;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator& SlotMap<
    Value, GenerationType, IndexType>::const_iterator::operator++()
{
    if (mIndex >= mPtr->mNodes.size())
    {
        throw std::runtime_error("Incremented iterator on end");
    }

    mIndex++;

    while (!mPtr->mNodes[mIndex].has_data)
    {
        if (mIndex >= mPtr->mNodes.size())
        {
            return *this;
        }

        mIndex++;
    }
    return *this;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<
    Value, GenerationType, IndexType>::const_iterator::operator++(int)
{
    iterator out = *this;
    ++(*this);
    return out;
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::const_iterator::reference SlotMap<Value, IndexType,
GenerationType>::const_iterator::get() const
{
    return **this;
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::Key SlotMap<Value, IndexType, GenerationType>::const_iterator::
GetKey()
{
    auto& map = mPtr->mNodes[mIndex];
    return Key{map.mGeneration, mIndex};   
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::TypedKey SlotMap<Value, GenerationType, IndexType>::insert(Value&& data)
{
    if (mFreeList != SIZE_MAX)
    {
        size_t next = mNodes[mFreeList].uNextFree;
        new (&mNodes[mFreeList].uData) Value{std::move(data)};
        Key key(mNodes[mFreeList].mGeneration, static_cast<unsigned>(mFreeList));
        mNodes[mFreeList].mHasData = true;
        mFreeList = next;
        ++mSize;
        return key;
    }

    Key key(0, static_cast<unsigned>(mNodes.size()));
    mNodes.emplace_back(std::move(data), 0);
    ++mSize;
    return TypedKey{key};
}

template <typename Value, typename GenerationType, typename IndexType>
void SlotMap<Value, GenerationType, IndexType>::remove(Key key)
{
    if (key.mIndex >= mNodes.size())
    {
        throw std::runtime_error("Invalid key - invalid index");
    }

    Node& node = mNodes[key.mIndex];
    if (node.mGeneration != key.mGeneration)
    {
        throw std::runtime_error("Invalid key - object already destroyed");
    }

    node.mGeneration += 1;
    node.uData.~Value();
    node.uNextFree = mFreeList;
    node.mHasData = false;
    mFreeList = key.mIndex;
    --mSize;
}

template <typename Value, typename IndexType, typename GenerationType>
void SlotMap<Value, IndexType, GenerationType>::remove(TypedKey key)
{
    remove(key.mKey);
}

template <typename Value, typename GenerationType, typename IndexType>
bool SlotMap<Value, GenerationType, IndexType>::TryRemove(Key key)
{
    if (key.mIndex >= mNodes.size())
    {
        return false;
    }

    Node& node = mNodes[key.mIndex];
    if (node.mGeneration != key.mGeneration)
    {
        return false;
    }

    node.mGeneration += 1;
    node.uData.~Value();
    node.uNextFree = mFreeList;
    node.mHasData = false;
    mFreeList = key.index;

    --mSize;

    return true;
}

template <typename Value, typename IndexType, typename GenerationType>
bool SlotMap<Value, IndexType, GenerationType>::TryRemove(TypedKey key)
{
    return TryRemove(key.mKey);
}

template <typename Value, typename GenerationType, typename IndexType>
bool SlotMap<Value, GenerationType, IndexType>::contains(const Key& key)
{
    if (key.mIndex >= mNodes.size()) return false;
    return mNodes[key.mIndex].mGeneration == key.mGeneration;
}

template <typename Value, typename IndexType, typename GenerationType>
bool SlotMap<Value, IndexType, GenerationType>::contains(const TypedKey& key)
{
    return contains(key.mKey);
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<
    Value, GenerationType, IndexType>::erase(iterator& iter)
{
    if (iter.mPtr != this)
        throw std::runtime_error("Attempted to remove value from incorrect SlotMap");

    if (iter.mPtr == end())
        throw std::runtime_error("Erased called with end iterator");

    if (iter.mIndex >= mNodes.size())
        throw std::runtime_error("Attempted to remove value with invalid index");

    Node& node = mNodes[iter.mIndex];

    node.mGeneration += 1;
    node.uData.~Value();
    node.uNextFree = mFreeList;
    node.mHasData = false;
    mFreeList = iter.mIndex;

    ++iter.mIndex;

    --mSize;

    return iter;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator SlotMap<Value, GenerationType, IndexType>::erase(
    const_iterator& iter)
{
    if (iter.mPtr != this)
        throw std::runtime_error("Attempted to remove value from incorrect SlotMap");

    if (iter.mPtr == end())
        throw std::runtime_error("Erased called with end iterator");

    if (iter.mIndex >= mNodes.size())
        throw std::runtime_error("Attempted to remove value with invalid index");

    Node& node = mNodes[iter.mIndex];

    node.mGeneration += 1;
    node.uData.~T();
    node.uNextFree = mFreeList;
    node.mHasData = false;
    mFreeList = iter.mIndex;

    ++iter.mIndex;

    ++mSize;

    return iter;
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<Value, GenerationType, IndexType>::begin()
{
    return iterator(this, 0);
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator SlotMap<
    Value, GenerationType, IndexType>::begin() const
{
    return const_iterator(this, 0);
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<Value, GenerationType, IndexType>::end()
{
    return iterator(this, mNodes.size());
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator SlotMap<
    Value, GenerationType, IndexType>::end() const
{
    return const_iterator(this, mNodes.size());
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<Value, GenerationType, IndexType>::operator[
](const Key& key)
{
    return find(key);
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::iterator SlotMap<Value, IndexType, GenerationType>::operator[](
    const TypedKey& key)
{
    return (*this)[key.mKey];
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator SlotMap<Value, GenerationType, IndexType>::operator[
](const Key& key) const
{
    return find(key);
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::const_iterator SlotMap<Value, IndexType, GenerationType>::
operator[](const TypedKey& key) const
{
    return (*this)[key.mKey];
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::iterator SlotMap<
    Value, GenerationType, IndexType>::find(const Key& key)
{
    if (key.mIndex >= mNodes.size())
    {
        throw std::runtime_error("Invalid key - invalid index");
    }

    Node& node = mNodes[key.mIndex];
    if (node.mGeneration != key.mGeneration)
    {
        throw std::runtime_error("Invalid key - object already destroyed");
    }

    return iterator(this, key.mIndex);
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::iterator SlotMap<Value, IndexType, GenerationType>::find(
    const TypedKey& key)
{
    return find(key.mKey);
}

template <typename Value, typename GenerationType, typename IndexType>
typename SlotMap<Value, GenerationType, IndexType>::const_iterator SlotMap<Value, GenerationType, IndexType>::find(
    const Key& key) const
{
    if (key.mIndex >= mNodes.size())
    {
        throw std::runtime_error("Invalid key - invalid index");
    }

    const Node& node = mNodes[key.mIndex];
    if (node.mGeneration != key.mGeneration)
    {
        throw std::runtime_error("Invalid key - object already destroyed");
    }

    return const_iterator(this, key.mIndex);
}

template <typename Value, typename IndexType, typename GenerationType>
typename SlotMap<Value, IndexType, GenerationType>::const_iterator SlotMap<Value, IndexType, GenerationType>::find(
    const TypedKey& key) const
{
    return find(key.mKey);
}

template <typename _Value, typename _IndexType, typename _GenerationType>
typename SlotMap<_Value, _IndexType, _GenerationType>::GenerationType SlotMap<_Value, _IndexType, _GenerationType>::
GetGeneration(const IndexType& key) const
{
    return mNodes[key].mGeneration;
}

template <typename _Value, typename _IndexType, typename _GenerationType>
uint32_t SlotMap<_Value, _IndexType, _GenerationType>::Size() const
{
    return mSize;
}
