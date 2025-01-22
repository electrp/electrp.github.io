#pragma once
#include <algorithm>
#include <cstdint>

/**
 * @author Will Bender
 *
 ** Defines an interface to interact with types at runtime.
 */

///////////////////////////////////
/// Type Definitions 

/*
 ** An object that defines a type's definition. 
 ** Pointers may be nullptr
 *
 * This class defines functions that may or may not be implemented by types. When objects that do not have access to
 * type information for template functions, they can use this instead.
 *
 * Contains function pointers to act upon types, and any other information required to operate the data types.
 */
struct MetaType
{
    template<typename T>
    static MetaType GenerateType();
    
    ///////////////////////////////////
    /// Function pointer definitions
    
    // Note: count iterates through values assuming data is an array.

    // Constructs a value 
    using DefaultConstruct =    void(*)(void* data, uint32_t count);
    // Destructs a value
    using Destruct =            void(*)(void* data, uint32_t count);
    // Moves a value from one memory location to another
    using MoveConstruct =       void(*)(void* src, void* dst, uint32_t count);
    // Moves a value to another initialized structure's location
    using MoveAssign =          void(*)(void* src, void* dst, uint32_t count);
    // Copies a value from an initialized value to a memory location
    using CopyConstruct =       void(*)(void* src, void* dst, uint32_t count);
    // Copies a value from an initialized structure to another initialized structure
    using CopyAssign =          void(*)(void* src, void* dst, uint32_t count);

    
    ///////////////////////////////////
    /// Data

    uint32_t
        mDataSize,      // The size of the data in bytes
        mDataAlignment; // The alignment rules provided by alignas(n)

    
    /// Function Pointers
    
    // Constructs a value 
    DefaultConstruct    mDefaultConstruct;
    // Destructs a value
    Destruct            mDestruct;
    // Moves a value from one memory location to another
    MoveConstruct       mMoveConstruct;
    // Moves a value to another initialized structure's location
    MoveAssign          mMoveAssign;
    // Copies a value from a initialized value to a memory location
    CopyConstruct       mCopyConstruct;
    // Copies a value from an initialized structure to another initialized structure
    CopyAssign          mCopyAssign;
};

///////////////////////////////////
/// Template Implementations

template<typename T>
MetaType MetaType::GenerateType()
{
    MetaType out{};

    // Fill in size information
    out.mDataSize = sizeof(T);
    out.mDataAlignment = alignof(T);

    
    // Generate lambda functions with known function signatures
    out.mDefaultConstruct = [](void* data, uint32_t count)
    {
        for(uint32_t i = 0; i < count; ++i)
            new (static_cast<T*>(data) + i) T();
    };
    out.mDestruct = [](void* data, uint32_t count)
    {
        for(uint32_t i = 0; i < count; ++i)
            static_cast<T*>(data)[i].~T();
    };
    out.mMoveConstruct = [](void* src, void* dst, uint32_t count)
    {
        for(uint32_t i = 0; i < count; ++i)
            new (static_cast<T*>(dst) + i) T(std::move(*static_cast<T*>(src)));
    };
    out.mMoveAssign = [](void* src, void* dst, uint32_t count)
    {
        for(uint32_t i = 0; i < count; ++i)
            static_cast<T*>(dst)[i] = std::move(*static_cast<T*>(src));
    };
    out.mCopyConstruct = [](void* src, void* dst, uint32_t count)
    {
        for(uint32_t i = 0; i < count; ++i)
            new (static_cast<T*>(dst) + i) T(*static_cast<T*>(src));
    };
    out.mCopyAssign = [](void* src, void* dst, uint32_t count)
    {
        for(uint32_t i = 0; i < count; ++i)
            static_cast<T*>(dst)[i] = static_cast<T*>(*src);
    };
    
    return out;
}
