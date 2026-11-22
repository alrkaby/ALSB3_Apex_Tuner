# ALSB3 Apex Tuner (Title ID: APEX00001)

### The Ultimate Intelligent PS4 Performance & Optimization Suite
**Engineered & Developed by: alsb3 dev**

ALSB3 Apex Tuner is a legendary, next-generation native PlayStation 4 Homebrew application engineered in modern C++17 and OpenOrbis Toolchain by **alsb3 dev**. It introduces an automated **Smart Diagnostic & Auto-Tune Engine** that scans your console hardware, benchmarks your storage and network, and automatically applies the optimal profile for peak gaming performance.

---

## 🌟 Key Features

1. **Intelligent Auto-Scan & Auto-Tuning Engine (`L1 + R1`)**:
   - **Hardware Detection**: Automatically detects whether your console is a PS4 FAT (CUH-1xxx), Slim (CUH-2xxx), or Pro (CUH-7xxx).
   - **Storage Fragmentation Benchmark**: Inspects `app.db` B-Tree page health and determines if defragmentation is required.
   - **Network Ping Probe**: Tests multiple gaming DNS backbones (Cloudflare, Google, Quad9) and automatically selects the fastest DNS with lowest ping.
   - **Automatic Profile Deployment**: Applies the ideal fan thermal threshold, network MTU, TCP parameters, and boost mode tailored to your specific hardware.

2. **The "GOD MODE" Engine (`R1 + X`)**:
   - Single-button full-system overdrive:
     - SQLite `app.db` VACUUM & REINDEX.
     - Deep junk and crash dump purge.
     - Low-latency gaming DNS and zero-fragmentation MTU (1500).
     - GPU/CPU clock overdrive and 60 FPS patch injection.
     - Engages the 1-hour intelligent RAM watchdog and launches your selected game.

3. **Hourly Intelligent RAM Watchdog (Anti-Stutter & Anti-Lag Engine)**:
   - Runs a precision background watchdog thread.
   - Every **60 minutes**, purges standby file system caches and compacts kernel memory.
   - **Crucial Protection**: Strictly exempts the running game process memory, guaranteeing zero frame drops, zero stutter, and zero crashes.

4. **Luxury 60 FPS UI by alsb3 dev**:
   - Dark Obsidian with Neon Cyan & Apex Gold accents.
   - Live hardware telemetry: APU/SOC temperature, free RAM, free storage, and diagnostic badges.
   - Developer signatures and watermark: **alsb3 dev**.

---

## 🎮 DualShock 4 Controller Controls

| Button | Function |
|---|---|
| **`L1 + R1`** | **🧠 ALSB3 Smart Auto-Scan & Auto-Tune (Examines & Applies Optimal Settings)** |
| **`R1 + X`** | **🔥 GOD MODE (Complete System Defrag + 60FPS + Overdrive + Game Launch)** |
| **`X`** | Engage **Apex Game Focus** on selected game (Standby RAM Purge + Hourly Guard) |
| **Square** | Defragment & Optimize System Database (`app.db` VACUUM) |
| **Triangle** | Deep Purge (Crash Dumps, Cache & Orphaned Downloads) |
| **Options** | Apply Model-Specific Thermal Guard (Active Fan Cooling) |
| **Circle (O)** | Refresh Installed Game Library |

---

## 👑 Credits & Author
- **Lead Developer**: **alsb3 dev**
- **Architecture**: C++17, OpenOrbis Toolchain, SDL2 Hardware-Accelerated Rendering.
