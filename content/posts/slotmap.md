+++
title = "Slotmap: The budget allocator you probably should use"
date = "2024-12-26"
hideComments=true
tags = ["data", "engine_development"]
+++

Slotmaps are known by quite a few names, including stable vector (not to be confused with boost::stable_vector), slab, idvec, generational indices... far too many to list here. It's usefulness cannot be understated, and many other data structures can be implemented extremely efficiently when relying on this pattern. The most popular implementation I have found of this structure is the rust crate [slotmap](https://docs.rs/slotmap/latest/slotmap/), with most of the implementations that I can find being rust based.

## What does it do?

A small example of a generic (c++) example:

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

Slotmaps tend to function very similar to allocators, with two added restrictions:
- All access operations must go through the original object. 
- All objects must be the same type (or same byte size).

Insertion assigns a value to a "slot", and returns a unique key specific to that instance. Each key contains information to uniquely identify the instance of data created from the insert call, meaning that insertions and deletions will not cause the key to be re-validated after it's data is erased. 

## How does it work?

There are only a few pieces to understand how a slotmap works:
- Store data: 
	- This can be very easily achieved with a vector type, such as std::vector. 
- Assigning new values:
	- Using a free-list, we can keep track of free values. Doing so would require us to store those next-free values alongside as a part of the slot, which can be done using unions. 
- Differentiating similarly indexed values: 
	- A pattern for this is generally known as generational indices. A key type will generally store two things, a index and a generation. If the generation of the key matches the generation stored at the location checked, the index contains the value matching the index. When erased, just increment the generation of the slot, thus invalidating all keys with a matching generation.
- Testing validity of values:
	- This one is somewhat optional. If you plan to iterate through the slotmap, then you will need a way to test to see if the generation at the slot is valid. This is easy to implement with something like std::optional, or testing for a bit within the generation of a slot.

Below is a fairly simple implementation:

```cpp
// Untested
template<typename T> struct SlotMap {
	// Less writing, maximum/invalid index value
	const MAX = std::numeric_limits<uint32_t>::max();

	struct Key {
		uint32_t generation;
		uint32_t index;
	}

private:
	struct Slot {
		Slot(T && _data) :: data(_data) {}
		
		uint32_t generation = 0; 
		bool has_value = true; // Required for iteration 
		union {
			uint32_t nextFreeIndex;
			T data;
		}
	}

	std::vector<Slot> slots;
	uint32_t firstFree = MAX;

public:
	Key insert(T && data) {
		if(firstFree == MAX) {
			slots.emplace_back(std::forward(data));
		}
		uint32_t idx = firstFree;
		firstFree = slots[idx].nextFreeIndex;
		// Placement new
		new (&slots[idx].data]) T(std::forward(data)); 
		return { slots[idx].generation, idx };
	}
	Slot& test_validity(Key const& key) {
		if(slot.index > key.index)
			throw std::runtime_error("Index doesn't exist!");
		Slot& slot = slots[key.index];
		if(slot.generation == key.generation) 
			return slot.data;
		throw std::runtime_error("Value already removed!");
	}

	T& operator[](Key const& key) {
		return test_validity(key).data;
	}

	void erase(Key const& key) {
		Slot& slot = test_validity(key);
		slot.data.~T(); // Destruct the object
		++slot.generation;
		slot.nextFreeIndex = firstFree;
		firstFree = key.index;
	}
}
```

There are some issues that can come up with implementations like this:
- This design can be somewhat susceptible to memory leakage. Due to how generations are stored, it may not be possible to trim space off of the end. If a generation is removed, there is no way to find what it's value was previously. If the slot storage is expanded to where a generation was removed and readded, it will start at a previously used generation. This breaks the stable index guarantee.
- More of a fun theoretical problem: If too many values are added and removed, there is a chance that the generation counter rolls over. The two options would be to crash or allow keys to point to invalid values. Fortunately, it is incredibly unlikely to ever see a case where a rollover will matter. Storing enough 32 bit keys to guarantee a collision would take about 32 gigabytes, or 4 billion iterations (producing 4 billion errors) of testing and regenerating the same invalid key.

### Iteration 

Algorithmically, this implementation would have a slow iteration speed compared to allocated pointers. The required iterations isn't the number of currently stored values, but the maximum number of values ever stored in the structure. Depending on your usage, this could be just fine or even better. As the memory is guaranteed to be contiguous, you can spend much less time waiting for memory retrieval. For even better iteration performance, you could:
- Tracking the location of the last valid slot would allow you to only iterate until the end of all valid data. Preferring to insert new values at the front can improve this as well, which could be done simply with a priority queue.
 
## Example Uses

### ECS

Slotmap contains many of the operations that an ECS is expected to do: insertion, accessing, and erasing of entities. Keys almost perfectly match what an entity is, and many ECS implementations (including all of mine) use the generation index pattern to differentiate and find the data relating to entities. 

In an archetyped ECS, slotmaps can be used to create stable Entity references using key objects. Storing the required information to get the entities components within the slotmap allows for the quick lookup of the archetype or internal index, which may change based on added or removed components.

Indices can also be used to access components directly. Components can be stored in parallel vectors that match the index provided by the key. This makes lookup for components all managed by the slotmap, with it's only job managing the free-list.

### Trees

Trees are often created by allocating nodes that store pointers to each other. If the tree is able to store each node as the same type, a slotmap would be a great candidate to store the nodes:

```cpp
struct Node {
	int value;
	Key left;
	Key right;
}

struct BST {
	Slotmap<Node> nodes;
	Key root;
}
```

Instead of relying on dynamically allocated nodes, all nodes are stored sequentially within the slotmap. 