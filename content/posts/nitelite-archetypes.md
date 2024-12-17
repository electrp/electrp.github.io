+++
title = "Nitelite Archetypes"
date = "2024-12-15"
hideComments=true
+++ 

(not finished, sorry! come back later)
## One of, if not the most important peice of an ECS is it's data storage.

ECS is defined in two ways:
- ECS as a programming pattern, with it's core philosophy of "Composition over Inehritance.
- ECS as a data access pattern, where objects are stored in a way optimized for computers to proccess.

Both definitions are incredibly important to why ECS is trendy. The programming pattern promotes decoupling and modularity, while the data pattern provides great performance over object-driven approaches. This post will specifically highlight the data access pattern part of ECS.

The data storage aspect of ECS is complex and can be done in many ways, but within the NiteLite engine I took advantage of a version known as an Archetyped ECS. Archetype ECS implementations generally optimize for fast entity querying and iteration within queries. This was something I prioritized.

## What do you want from an ECS data storage implementation?

There are many ways to store data, but they all should attempt a few specific things:
- Supporting SIMD/vectorized operations can speed up your program quite a bit, so data should be laid out in a way to support this.
- Iteration should be quick and not require complex math or excessive memory accesses. This is handled by storing groups of components near to each other,
- Random entity access to entities should be fast enough. 
    - Iteration can be optimized in many unique ways, but not every algorithm can be implemented purely iteratively, especially in game contexts. 

## My version.

My implementation is very similar to those of Bevy and Flecs. A quick overview of the types:
- Archetype: Contains a list tables that store components. One per entity type.
- Table: Stores a set number of components using SOA.
- Entity Provider: Manages the creation, deletion, and movement of entities around archetypes while maintaining a stable index for users.
- Query: A view of entities that match a set of rules provided. For example, all entities with the components (Transform, Velocity, Enemy).

### Archetypes

Archetypes are what differentiates different entities from each other, and are the mechanism for storing components. Each archetype corresponds with a specific collection of components, and stores all entities with that set. This means that operations that involve adding or removing entities/components will result in data being added or removed from at least one archetype storage device.

Archetypes keep track of other archetypes that contain a superset of the components they store. This means that when archetypes are referenced, you can easily find and iterate over the other archetypes that also have the same set of components that the current archetype has.

### Table

In NiteLite, tables are lightweight objects that store components, and by default contain all the components attached to a set of entities. The format for these tables are defined by the archetype itself.

Along with rows and columns, the table contains metadata about the number of entities contained and the index of the first free entity, for faster insertion. First entity free could have been a free-list, but tables are generally small.

The table itself contains a predefined number of rows, which correspond to entities. Each column refers to a unique component, as well as an extra column that stores an Entity reference object. Visually, the storage functions almost identically to an SQL table. The reason for the extra entity column will be explained next.

| Table | Entity | Position    | Rotation   | Health |
| ----- | ------ | ----------- | ---------- | ------ |
| 0     | e2g4   | (0, 1, 0)   | (0, 0, 0)  | 5      |
| 1     | e15g2  | (-1, 1, 1)  | (0, 0, 0)  | 50     |
| 2     | e15g1  | (1, 1, 1)   | (0, -1, 0) | 2      |
| 3     | NULL   | (100, 1, 0) | (1, 1, 1)  | 0      |
### Entity Provider

There are two different entity types provided by this implementation, the default Entity reference object, and a Archetype Entity object. Generally, the Entity object is used to obtain the Archetype Entity object.

An Archetype Entity is a non-stable reference to an entity, meaning that many operations will result in the reference to become invalid or referencing a separate entity. An Archetype Entity contains all that is needed to access the state of an entity:

```cpp
struct ArchetypeEntity
{
	uint16_t archetype_index; // An index to access a specific archetype
	uint16_t table_index; // The table the entity is contained within
	uint16_t entity_index; // The index within the table
}
```

If you 

### Queries / Iteration