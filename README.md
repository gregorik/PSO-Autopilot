<p align="center">
<img src="Docs/PSO logo2.png" alt="Logo" width="500"/>
</p>

# PSO Autopilot (Core)

[Watch a video](https://youtu.be/KS5MhbmniEE)

## A Solution to Unreal Engine 5 OOM Crashes

The "Shader Compilation" plugins on assorted marketplaces brute-force load all of your assets at once. If you have a massive 50GB project, this will cause your RAM to spike, your loading screen to freeze entirely, and eventually result in an Out-Of-Memory (OOM) crash.

**PSO Autopilot aims to fix this.**

### The Core Feature: Memory-Safe Chunking
Instead of loading thousands of materials simultaneously, PSO Autopilot Core uses the FStreamableManager to **asynchronously stream** your Materials in controlled batches (e.g., 100 at a time). After a batch is compiled via the Engine's native FShaderPipelineCache, the plugin explicitly drops references and yields to Unreal's asynchronous Garbage Collection. Your RAM footprint remains completely flat, making this tool safe for massive open-world projects.

A detailed manual is available at: https://gregorigin.com/PSO_Autopilot/

---

## How to Use
1. Copy the PSOAutopilotCore folder into your project's Plugins folder.
2. Regenerate Visual Studio files and compile.
3. Open Unreal Engine. Go to **Edit > Project Settings > PSO Autopilot Core**. Set your target directories and batch sizes.
4. In your Loading Screen UI Blueprint, call Get Game Instance Subsystem -> PSO Autopilot Core Subsystem.  
5. Bind an event to On Progress Updated to drive your loading bar.
6. Call Start Warmup.

*Code by Andras Gregori. MIT License.*






