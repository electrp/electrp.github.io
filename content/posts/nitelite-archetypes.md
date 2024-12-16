+++
title = "Nitelite Archetypes"
date = "2024-12-15"
hideComments=true
+++

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

## Now... My version.

My implementation is very simlar to those of Bevy and Flecs. A quick overview of the types:
- Archetype: Contains a list tables that store components. One per entity type.
- Table: Stores a set number of components in a SOA method.
- Entity Provider: Manages the creation, deletion, and movement of entites around archetypes while maintaining a stable index for users.
- Query: A view of entiies that match a set of rules provided. For example, all entities with the components (Transform, Velocity, Enemy).

### Archetypes

Archetypes are what differentiates different entities from each other, and are the mechanism for storing components. Each archetype corresponds with a specific collection of components, and stores all entities with that set. This means that operations that involve adding or removing entities/components will result in data being added or removed from at least one archetype storage device.

Archetypes keep track of other archetypes that contain a superset of the components they store. This means that when archetypes are referenced, you can easily find and iterate over the other archetypes that also have the same set of components that the current archetype has.

### Table

In NiteLite, tables are lightweight objects that hold 

### Entity Provider

### Queries / Iteration