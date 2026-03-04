# PSO Autopilot (Core)

## en-Source Solution to Unreal Engine 5 OOM Crashes

Most "Shader Compilation" plugins on the marketplace brute-force load all of your assets at once. If you have a massive 50GB project, this will cause your RAM to spike, your loading screen to freeze entirely, and eventually result in an Out-Of-Memory (OOM) crash.

**PSO Autopilot Core fixes this.**

### The Core Feature: Memory-Safe Chunking
Instead of loading thousands of materials simultaneously, PSO Autopilot Core uses the FStreamableManager to **asynchronously stream** your Materials in controlled batches (e.g., 100 at a time). After a batch is compiled via the Engine's native FShaderPipelineCache, the plugin explicitly drops references and yields to Unreal's asynchronous Garbage Collection. Your RAM footprint remains completely flat, making this tool safe for massive open-world projects.

---

> ?? **Need AAA Polish? Upgrade to PSO Autopilot Pro**
> 
> The Core (Free) version guarantees your project will never crash from OOM memory spikes during shader compilation. However, because it runs synchronously, your loading screen UI will still stutter or freeze. 
> 
> If you are releasing a commercial game and want a truly seamless player experience, check out **[PSO Autopilot Pro on Fab](#)**.
> 
> **Pro Exclusive Features:**
> - **Seamless Time-Slicing:** Keeps your UI spinners and videos running at a buttery 60fps while compiling.
> - **Zero-Redundancy Boot:** Smart-hashes the driver cache. If a player boots your game a second time, the 2-minute loading screen drops to 0.05 seconds.
> - **Full VFX Support:** Warms up Niagara Systems alongside Materials.

---

## How to Use
1. Copy the PSOAutopilotCore folder into your project's Plugins folder.
2. Regenerate Visual Studio files and compile.
3. Open Unreal Engine. Go to **Edit > Project Settings > PSO Autopilot Core**. Set your target directories and batch sizes.
4. In your Loading Screen UI Blueprint, call Get Game Instance Subsystem -> PSO Autopilot Core Subsystem.  
5. Bind an event to On Progress Updated to drive your loading bar.
6. Call Start Warmup.

*Copyright (c) 2026 GregOrigin. All Rights Reserved.*

