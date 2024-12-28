+++
title = "What bothers me about "
date = "2024-12-15"
hideComments=true
tags = ["engine_development", "projects", "digipen"]
draft = true
+++

ECS is an extremely powerful pattern, used in many powerful tools such as bevy, flecs, or entt. Recently, I have become increasingly frustrated by the lack of support for a feature I would like to see more popular support for. The largest issue isn't the implementation but the lack of design consideration, as all of these implementations would likely support this feature but are not designed in a way to take advantage of it.

In most ECS implementations, components are tied directly to the type system. Generics are often used when specifying which component is being accessed, providing an easy way to specify dependencies and find data. It's an incredibly intuitive solution, but flawed in a few ways. To illustrate this, there are two important things that a component should be able to solve:

Data storage: Types are how data storage is handled in most to all languages. Components must be tied to some sort of typing when it comes to this, as data that doesn't fit some sort of archetype is useless. (Remember the term Archetype for later)

Specification: Components must specify entity state or behavior. For example, an entity with a Position component must be in a 3d environment and have movement behaviors. This looks to be a perfect match for types once again, but fail with almost all compiled languages' type systems.

## The Failures of Compilation

Most ECS implementations use compiled type systems, such as those provided by Rust or C++. This means all information about the type must be compiled, and no state changes could allow for the introduction of a new type at runtime. 