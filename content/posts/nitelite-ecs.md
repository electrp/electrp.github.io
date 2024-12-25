+++
title = "Nitelite ECS"
date = "2024-12-15"
hideComments=true
tags = ["engine_development", "projects", "digipen"]
+++

I recently had the pleasure of joining a new team as a part of the DigiPen project courses, where we all worked together to develop a game engine. Me and another (Travis) had taken on the responsibility of the "Core Engine" development, so I have spent much of my time over the past semester working on implementing the best tools I can. 

Our primary goal for this project was to make a tool that others could use to make different types of games, somewhat of a small-scale Unity. Since we don't have the time to implement everything unity can, we decided to make some general-purpose tools while implementing some game specific tools as well for a final product we can show after next semester or beyond.

We knew pretty early on that we would like to use the ECS pattern, for a variety of reasons:
- A well implemented ECS can outperform other engines by a long margin, due to their data-oriented design and cache friendliness.
- Keeping things modular was a priority, we needed things that could come and go and the ECS pattern is very friendly to that.
- The compository nature of ECS is farily unique, but not incredibly difficult to onboard others into. Defining features and traits for entities becomes trivial.
- We wanted to attempt some form of multithreading, and ECS gave us our best chance. Due to how ECS commonly stores data, ECS provides a great starting point for defining system to system restrictions and relationships on how they will be run.

Challenges we forsaw and attempted to mitigate:
- Nobody on the team had used ECS in a project before outside of the other Core Engine developer, meaning we had to onboard others on how to understand the pattern very quickly.
- We need this engine to be usable to those who may not be familiar to what an ECS is, so we made specific design decisions (mostly regarding scripting) that hides some of the complexity to those who do not want/need to interact with it.

Due to my experience using and developing an ECS for a separate game project, there was no doubt in my mind that it was something I wanted to attempt again. 

## General ECS Design

I came forward to my team with designs heavily based on two key inspirations: '
- [bevy](https://bevyengine.org/), a full featured game engine built in rust over an ecs, with a incredibly simple and functional pattern for managing data flows.
- [flecs](https://github.com/SanderMertens/flecs), a fast and easy ECS emetnation, which is on the cutting edge of many helpful and useful ECS-related features. SanderMertens also publishes a great [series of blog posts](https://ajmmertens.medium.com/building-an-ecs-1-where-are-my-entities-and-components-63d07c7da742) on how flecs is designed and how to make your own ECS.

Here are some key design decisions that were made:
- Borrowed from bevy is the concept of [Resources](https://bevy-cheatbook.github.io/programming/res.html). These are data storages that act as singletons, and can be passed between systems to act as the engine state. Resources act as a bridge between systems, requiring them to store little data and minimizing dependencies within them. If you are building an engine or ECS implementation, I higly recommend looking into a pattern such as this. Allowing state to be transfered in a non-managed way by systems lead to some incredibly terrible bugs in my previous project.
- There was an attempt to get my teammates onboard with using functional systems. Many ECS implementations prefer systems to be defined as a function over systems as on object. Functional systems give the ECS more control over how to handle it, and help specify dependencies. Unfortunately, the interface become challenging for my team members to work with and trust, and so it was decided that we would go with a familiar object-based implementation for systems. 
- The interface needed to be simple enough, and relate more towards common game terms than the data structures defined by ECS. While ECS::insert makes more sense than ECS::create_entity from a data structures perspective, interfaces like the former require much more knowledge about ECS as a pattern while the former is something easily searchable and most importantly easily understandable.
- Entity reference must be stable, and must refer to the same entity (even if dead) until the end of the program. This was achieved using a [Slotamp](https://docs.rs/slotmap/latest/slotmap/), with a entity index as well as a generation.
- We wanted components to be limited to POD or trivial types, but this would limit system authors greatly. Trivial types are still recommended, but it was implemented with the assumption that types may have constructors.
- The ECS and all it's data must be able to be accessed without the use of template parameters. Some systems that require special features like serilization and scripting must be able to access values without compile time specific specilization.

Travis was commonly in charge of making the interface usable for the rest of the team, while I spent much of my time optimizing the data structures and developing storage methods. Things I implemented:
- Stable entity refrences (Entity Object/Slotmap)
- Data storage for components (See [Archetypes](nitelite-archetypes))
- Entity parenting (not yet merged due to conflicts with jolt and rendering)

