+++
title = "Slotmap: The budget allocator you probably should use"
date = "2024-12-16"
hideComments=true
tags = ["projects", "digipen"]
draft = true
+++

[__If you came for a good slotmap implementation, go see mine!__](https://github.com/electrp/slotmap)

I was made aware of this data structure a while ago when I was developing the engine for my DigiPen GAM200-250 project. It's brought up a lot in ECS contexts, for the simple reason that it *is* it's own (very limited) ECS storage implementation! While I am stretching the definition of ECS by a lot, a Slotmap perfectly solves entity providing all on its own. 

Slotmaps are known by quite a few names, including stable vector (not to be confused with boost::stable_vector), slab, idvec... far too many to list here. It's usefulness cannot be understated, and many other data structures can be implemented extremely efficiently when relying on this pattern. The most popular implementation I have found of this structure is the rust crate [slotmap](https://docs.rs/slotmap/latest/slotmap/), with most of the implementations that I can find being rust based.

## What does it do?

A small example of a generic (c++) implementation:

```cpp
Slotmap<Node> nodes; 

void user()
{
	// Insert a value into the slotmap, and store the resulting key
	Slotmap<Node>::Key key = nodes.insert(Node{0});

	// Retrieve the value
	assert(nodes[key] == Node{0});
	nodes[key] = Node{1};
	assert(nodes[key] == Node{1});

	// Removal of nodes is easy
	nodes.erase(key);
	assert(nodes[key] == ERROR);

	// Iterate through all nodes
	for (Node const& node : nodes)
	{
		std::cout << node.value() << std::endl;
	}
}
```

Slotmaps tend to function very similar to allocators, with the added restriction that all access operations must go through the original object:
- Insertion assigns a value to a "slot" within the slotmap, and returns a unique key. Each key contains enough information to find the slot that it was created to match, and uniquely identify the specific version of the object within the slot.
- Value retrieval is quick, usually implementable with a single array access. 

## Internals
