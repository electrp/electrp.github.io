+++
title = "Nitelite Audio"
date = "2024-12-15"
hideComments=true
tags = ["engine_development", "projects", "digipen"]
draft = true
+++

We have a game engine, and game engines generally need audio. Nobody really wanted to do it, so I took it on. Unfortunately, there's not a great audio library that would match well with our ECS engine, so the obvious solution was to make my own. Obviously I don't plan to implement extremely complex sound effects, but there are quite a few problems to solve with just 3d audio.

The libraries I have picked out:
- [PortAudio](https://www.portaudio.com/) - An abstraction over audio I/O operations that are generally OS specific. Provide the utilities for playing and recording audio.
- [minimp3](https://github.com/lieff/minimp3) - Mp3 file decoder in a single header file.
- [libsamplerate](https://github.com/libsndfile/libsamplerate) - Resamples audio. I almost wrote my own, but I wouldn't be able to match the quality or speed of an external library.

# A Basic Pipeline

The basic pipeline for audio playing using the shared libraries would go:
- Load the mp3 of a specified file into memory using minimp3.
- Query the available sample rates supported by the default sound output with PortAudio.
- Resample the data from the mp3 sample rate to something that can be handled by the supported sample rates using libsamplerate.
- Send sound data to OS using PortAudio.

This implementation allows for playback of a single sound. What else do we need 
- Simultaneous audio playback, requiring multiple audio signals to be combined. After individually processing sounds, we need some way to convince them. Once we have all our data streams, simply adding them together will be enough.
- 


# Nodes

A problem to solve 