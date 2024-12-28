+++
title = "What bothers me about "
date = "2024-12-15"
hideComments=true
tags = ["engine_development", "projects", "digipen"]
draft = true
+++

ECS is an extremely powerful pattern, used in many powerful tools such as bevy, flecs, or entt. Recently, I have become increasingly frustrated by the lack of support for a feature I would like to see more popular support for. The largest issue isn't the implementation but the lack of design consideration, as all of these implementations would likely support this feature but are not designed in a way to take advantage of it.

In most ECS implementations, components are tied directly to the type system. When you request a component, you specify it by type. This is incredibly intuitive, 