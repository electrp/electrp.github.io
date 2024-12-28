+++
title = "It has ai..."
date = "2024-12-15"
hideComments=true
tags = ["engine_development", "projects", "digipen"]
draft = true
+++

I was eating brunch with some family recently, and my mom was talking about a bird feeder she had gotten my dad for Christmas. It's really nice looking, but it's special feature is a camera that can detect bird species, and send its information to your phone. In her description though, I heard a phrase that made my blood boil 

# "... It has ai that can..."

I was even shocked by my reaction to that phrase. I felt my breath and heart rate quicken, a wall in my throat formed, and I felt more agitated and anxious than I had all day. Even thinking about it makes me feel uncomfortable.

Now, the specific application of AI to detect species is a little gimmicky but very cool application that likely couldn't be as good without some sort of machine learning involved. Unfortunately, it seems to be hit or miss. Some people are reporting good results and others not so much. I can accept mistakes from a learning model, and from what I see it has features that seem to point towards sending user-provided training data that can improve the model. So why would I be so mad over such a decent and reasonable use of AI?

For context, my mom only interacts with software as a user, truly interacting AI for the first time with ChatGPT. She's not someone who necessarily cares about tech, and truly only cares about the end product, in this case the bird recognition. I think the phrase truly set my off due to the fact that a user, who doesn't care about the AI world, used this phrase that I often only hear from those who have bought into what I see as an oversaturated AI bubble. This phrase is often matched with applications that are so incredibly stupid that I'm conditioned to expect nothing but the worst.

# My view of AI might be weird.

I see AI as we use it now as a jackhammer. We as humans have quite a few problems to solve, and generally we can solve them with reasoning and logic. Machine learning doesn't really do it that way, instead it looks at a problem and tries to drill it's way through without understanding what even is happening. It's really odd to compare these tools that are supposed to be super intelligent to a tool that breaks apart rocks and generally makes a mess, but they are really the same.

AI is really just throwing a bunch of stuff at a wall until it sticks. When it learns, it just brute forces a bunch of options until it figures out what the best way to do something is. In the process it takes a while bunch of compute and an insane amount of filtering to clean up the mess it makes. Thus, AI can solve problems often faster than humans just because of how insanely fast we can get a GPU to run.

For something that people don't want to write the insanely large logic trees for, like human speech or computer vision, AI will completely dominate. What unfortunately breaks down is where those logic trees aren't needed, and compute is wasted trying to solve a problem that doesn't need AI to be solved.

AI can never solve a perfectly optimized problem better than the human generated solution. If something claims to, it means it's sacrificing edge cases you cannot see. Those edge cases may be unimportant and a human may never find those edge cases, but they exist and you may never see them until they become a problem.

You can take a jackhammer to the wall of human speech, but something like arithmetic can't and won't be optimized by AI. It's something thats going to be a part of almost all software in some way, but often the solution can be reached faster and more effectively when you can skip the filtering and skip the jackhammer.

## The useless applications, and where humans shine

Ray-traced denoising might be one of the best examples I can find. I took a class recently where the professor said, almost verbatim, "It might seem really simple, but the result is like magic." That's a phrase I've definitely heard variations of, and very often around AI.

Ray-tracing is a way of 3d rendering, where you "trace rays" from your camera to objects in the scene, using how it bounces to calculate the light of objects you are looking at. It's tech used in practically all animated movies to provide an accurate simulation of almost exactly how light works. Unfortunately, its reeeally slow. Render farms will sit for hours on a single frame, calculating as many light rays as possible. Now, what if we wanted to do this in realtime?

In our implementation, we restricted ourselves to one ray per pixel per frame. This means that each pixel has a single attempt to find a light in a given frame. If your ray doesn't hit a light, that pixel is black for that frame. Obviously, that looks pretty bad. 

// TODO: Add example image of one ray per pixel
The first frame of our simulation. With pixels accumulating color and motion information used to move some information around, it gets better over time. As you move the camera though, you will see many frames that look like this pre-denoising.

This image took basically all of my GPU power to produce at a reasonable frame rate. i cannot imagine that this is what you want from your game or software. This needs to improve.

The solution to removing noise is obviously: denoising. 

