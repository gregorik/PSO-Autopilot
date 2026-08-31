<div align="center">

<img src="Docs/PSO logo2.png" alt="PSO Autopilot Logo" width="500"/>

# ✈️ PSO Autopilot (Core)

**Memory-Safe Asynchronous Shader Compilation & PSO Warmup for Unreal Engine 5**

[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](LICENSE)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.5%20%7C%205.6%20%7C%205.7%20%7C%205.8-313131?logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![Platforms](https://img.shields.io/badge/Platform-Win64%20%7C%20Mac%20%7C%20Linux%20%7C%20Android%20%7C%20iOS-0078D4?logo=windows&logoColor=white)](#installation)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](#)
[![Discord](https://img.shields.io/badge/Discord-Join%20Community-5865F2?logo=discord&logoColor=white)](https://discord.gg/nqYQ5mtmHb)

[📺 Watch Video (v0.1)](https://youtu.be/KS5MhbmniEE) • [📺 Updated Showcase (v1.1)](https://www.youtube.com/watch?v=eYjHRwzfteQ) • [📺 Stress Test (v1.3)](https://www.youtube.com/watch?v=PPTxKIusY9I) • [📺 Test Runs](https://www.youtube.com/watch?v=YlBrR46wOUc)

</div>

---

![PSOProm](https://github.com/user-attachments/assets/d1c8acef-b2f0-4050-8ea0-365a9dbd1686)

> *This repository deals with advanced bypasses of standard Unreal C++ bottlenecks. 🟢 Currently available for B2B consulting and remote contract/Co-Dev integration (CET Timezone). [Contact form.](https://gregorigin.com/contact.html)*

---

## 💥 The Solution to Unreal Engine 5 OOM Crashes

Generic "Shader Compilation" plugins on assorted marketplaces attempt to brute-force load all project assets at once. For medium-to-large projects (10 GB – 100+ GB), this naive approach causes catastrophic RAM spikes, completely freezes loading screens, and triggers Out-Of-Memory (OOM) crashes on consoles, mobile devices, and mid-tier PCs.

**PSO Autopilot eliminates this bottleneck.**

### 🧠 Core Architecture: Memory-Safe Chunking
Instead of loading thousands of materials simultaneously, **PSO Autopilot Core** uses Unreal's `FStreamableManager` to **asynchronously stream** material assets in strictly controlled batches (e.g., 100 assets per batch). 

Once a batch is submitted for compilation via the Engine's native `FShaderPipelineCache`, the plugin explicitly drops all asset references and triggers non-blocking Garbage Collection sweeps. Your RAM footprint remains completely flat throughout the entire warmup phase.

```mermaid
flowchart LR
    A[Discovered Assets] --> B[Slice Batch e.g. 100]
    B --> C[FStreamableManager Async Load]
    C --> D[Background Driver Compilation]
    D --> E[Wait on PSO Queue / Bounded Timeout]
    E --> F[Drop Handles & Force GC]
    F -->|Next Batch| B
    F -->|All Complete| G[OnWarmupComplete Event]
```

📖 **Detailed Manual:** [https://gregorigin.com/PSO_Autopilot/](https://gregorigin.com/PSO_Autopilot/)  
💎 **Pro Version on Fab:** [PSO Autopilot Pro](https://www.fab.com/listings/95a798ac-3b74-4dae-8d8a-3e86a8d1646b) *(Time-sliced game thread yielding, smart cache skipping, setup wizard, JIT micro-warmups, adaptive frame pacing, and more)*  
💬 **Discord Support:** [https://discord.gg/nqYQ5mtmHb](https://discord.gg/nqYQ5mtmHb)

---

## 📊 Feature Comparison

| Feature | Core GitHub Edition (0.3+ MIT) | [Pro Fab Edition](https://www.fab.com/listings/95a798ac-3b74-4dae-8d8a-3e86a8d1646b) (1.4+ Commercial) |
|:---|:---:|:---:|
| 📦 **License** | Open Source (MIT) | Commercial Marketplace |
| 🚚 **Distribution** | Source code | Source + Epic-Vetted Binaries |
| ⚙️ **Engine Compatibility** | UE 5.5, 5.6, 5.7, 5.8+ | UE 5.5, 5.6, 5.7, 5.8+ |
| 📱 **Platforms** | Win64, Mac, Linux, Android, iOS | Win64, Mac, Linux, Android, iOS |
| 🧠 **Memory-Safe Chunking** | ✅ Included | ✅ Included |
| 🛡️ **OOM Crash Mitigation** | ✅ Included | ✅ Included |
| 🔄 **Asynchronous Batch Streaming** | ✅ Included | ✅ Included |
| 🧹 **Non-Blocking GC Sweeping** | ✅ Included | ✅ Included |
| ⏱️ **Sub-Second Loading Transitions** | ✅ Included | ✅ Included |
| 📊 **BlueprintPure Telemetry Getters** | ✅ Included | ✅ Included (`FPSOAutopilotStatus` Struct + Getters) |
| ⏳ **Time-Sliced Game Thread Yielding** | ❌ n/a | ✅ Included (60fps buttery loading UI) |
| ⏭️ **Zero-Redundancy Smart Cache Skip** | ❌ n/a | ✅ Included (Instant 0.05s repeat boot) |
| 🐎 **Engine PSO Frame Pacing** | ❌ n/a | ✅ Included |
| 🪄 **Setup Wizard & 13 Auto-Fix Actions**| ❌ n/a | ✅ Included (Window > Tools > Setup Wizard) |
| ⚡ **Modern Async BP Action Node** | ❌ n/a | ✅ Included (`Start Loading Flow (Async)`) |
| 🔄 **Generate / Regenerate Demo Content**| ❌ n/a | ✅ Included (Idempotent 1-click generator) |
| 🎆 **Niagara Systems & VFX Warmup** | ❌ n/a | ✅ Included |
| 🛑 **Class-Based Scan Exclusion Filter** | ❌ n/a | ✅ Included |
| 🎯 **Explicit Warmup Roots & Preloads** | ❌ n/a | ✅ Included |
| ⚡ **JIT Gameplay Micro-Warmups** | ❌ n/a | ✅ Included (Warmup bosses/actors at runtime) |
| 🌐 **Spatial / World Partition Sweeps** | ❌ n/a | ✅ Included (Async player proximity warmup) |
| 🔗 **Package Dependency Expansion** | ❌ n/a | ✅ Included |
| 💽 **Destination Level Package Preloading** | ❌ n/a | ✅ Included |
| 🧱 **Asset-Specific PSO Precache** | ❌ n/a | ✅ Included |
| 🕵️ **Bloat Profiler Dashboard** | ❌ n/a | ✅ Included (Top 10 heaviest PSOs) |
| 🧪 **3-Level Torture Test Demo** | Basic UI Sample | Full 3-Level Showcase Suite |

---

## 🛠️ Quick Start & Usage

### 1. Installation
1. Clone or copy `PSOAutopilotCore` into your project's `Plugins/` folder:
   ```bash
   cd YourProject/Plugins
   git clone https://github.com/gregorik/PSO-Autopilot.git PSOAutopilotCore
   ```
2. Regenerate your project files and compile.

### 2. Configuration
In Unreal Editor, navigate to **Edit > Project Settings > PSO Autopilot Core**:
- **Directories To Scan**: Specify folders containing materials to warm up (e.g., `/Game`, `/Game/Characters`, `/Game/VFX`).
- **Batch Size**: Number of assets to stream concurrently (Default: `100`).
- **Garbage Collect Between Batches**: Keep enabled (`true`) to keep RAM flat.

### 3. Blueprint Integration
In your Loading Screen Widget or Game Instance:
1. Call **Get Game Instance Subsystem** → Select **PSO Autopilot Core Subsystem**.
2. Bind an event to **On Progress Updated** (`OverallProgress`, `CurrentStatusMessage`) to drive your progress bar and status text.
3. Bind an event to **On Warmup Complete** to dismiss the loading screen and transition to your game level.
4. Call **Start Warmup**.

```
[Get GameInstance Subsystem (PSOAutopilotCoreSubsystem)]
     │
     ├──> [Bind Event to OnProgressUpdated] ──> Update UI (ProgressBar, Text)
     ├──> [Bind Event to OnWarmupComplete]  ──> OpenLevel / Close Loading Screen
     └──> [Start Warmup]
```

### 4. C++ Integration
```cpp
#include "PSOAutopilotCoreSubsystem.h"
#include "Engine/GameInstance.h"

void AMyHUD::BeginWarmup()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UPSOAutopilotCoreSubsystem* PSOSubsystem = GI->GetSubsystem<UPSOAutopilotCoreSubsystem>())
        {
            PSOSubsystem->OnProgressUpdated.AddDynamic(this, &AMyHUD::HandleProgressUpdated);
            PSOSubsystem->OnWarmupComplete.AddDynamic(this, &AMyHUD::HandleWarmupComplete);
            PSOSubsystem->StartWarmup();
        }
    }
}
```

---

## 📡 Blueprint Telemetry API

`UPSOAutopilotCoreSubsystem` exposes clean BlueprintPure helpers:
- `IsWarmingUp()` — Returns `true` if a warmup cycle is in progress.
- `GetWarmupState()` — Returns current phase (`Idle`, `Scanning`, `LoadingBatch`, `ProcessingBatch`, `UnloadingBatch`, `WaitingForGC`, `Finished`).
- `GetOverallProgress()` — Float between `0.0` and `1.0`.
- `GetTotalAssetsToProcess()` — Total asset count discovered during scanning.
- `GetTotalAssetsProcessed()` — Count of assets warmed so far.
- `GetCurrentStatusMessage()` — Status string (e.g., `"Scanning for Assets..."`, `"Loading Batch of 100 Assets..."`, `"Compiling Shaders..."`).

---

## 📄 License & Credits

Developed by **Andras Gregori (GregOrigin)**.  
Licensed under the [MIT License](LICENSE).
