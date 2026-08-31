# Changelog

All notable changes to PSO Autopilot (Core OSS) are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.3.0] - 2026-08-31

### 🚀 Features & Enhancements
- **Multi-Platform Support Expanded:** Added `Android` and `IOS` to `PlatformAllowList` alongside `Win64`, `Mac`, and `Linux` so mobile projects can compile and leverage memory-safe chunking.
- **Engine Support:** Verified compatibility across Unreal Engine **5.5, 5.6, 5.7, and 5.8**.
- **BlueprintPure Telemetry Getters:** Exposed real-time warmup telemetry functions on `UPSOAutopilotCoreSubsystem`:
  - `IsWarmingUp()` — Check if warmup is actively running.
  - `GetWarmupState()` — Returns current state machine phase (`Scanning`, `LoadingBatch`, `ProcessingBatch`, `UnloadingBatch`, `WaitingForGC`, `Finished`).
  - `GetOverallProgress()` — Normalized 0.0 to 1.0 progress float.
  - `GetTotalAssetsToProcess()` — Total asset count discovered.
  - `GetTotalAssetsProcessed()` — Current number of processed assets.
  - `GetCurrentStatusMessage()` — Human-readable status string.
- **Robust Path Sanitization:** Scan directories configured in Project Settings now have whitespace and trailing slashes stripped automatically, preventing silent `FARFilter` query misses. Defaults to `/Game` if empty.

### 📚 Documentation
- **Updated Feature Comparison:** Reflected the latest capabilities of PSO Autopilot Pro (v1.4.0) on Fab, detailing Time-Sliced Frame Pacing, Smart Cache Skipping, Setup Wizard & Auto-Repair, Modern Async Actions (`Start PSO Autopilot Loading Flow (Async)`), and Telemetry Structs.
- **Synchronized HTML Manual:** Overhauled `Docs/index.html` with dark theme styling, clarifying the Core memory-safe architecture versus Pro capabilities.

---

## [0.2.0] - 2026-08-02

### 🐛 Bug Fixes & Refinements
- **Scope Correction:** Removed inert `MaxProcessingTimeMsPerFrame` setting. Clarified that Core yields between batches, while intra-frame time-slicing is part of the Pro edition.
- **Hang Protection:** Bounded the engine's `FShaderPipelineCache::NumPrecompilesRemaining()` drain wait to 30 seconds to prevent loading screens from hanging indefinitely if the queue stalls.
- **Batch Extraction Performance:** Optimized batch slicing from quadratic array shifting to single-move slicing.
- **Delegate Frequency:** Throttled progress broadcast to once per batch rather than per-asset to eliminate Blueprint dynamic multicast overhead.
- **Dependency Cleanup:** Dropped unnecessary Niagara plugin dependency from the descriptor.

---

## [0.1.0] - 2026-04-01

### 🎯 Initial Release
- Memory-safe asynchronous shader compilation and material warmup system for Unreal Engine 5.
- Asynchronous batch streaming via `FStreamableManager`.
- Non-blocking garbage collection sweeps between batches to keep RAM usage flat.
- `APSOAutopilotCoreDemoManager` and `UPSOAutopilotCoreDemoWidget` sample HUD.
