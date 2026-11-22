# OrbisApex (Title ID: APEX00001)

### Ultimate PS4 Apex Gaming Mode & System Tuner for Jailbroken Consoles (GoldHEN)

OrbisApex is a high-performance native PlayStation 4 Homebrew application engineered in modern C++17 and OpenOrbis Toolchain. It transforms your PS4 into a fine-tuned gaming station with a focused **Game Mode**, scheduled memory optimization, and full-system debloating.

---

## Key Features

1. **Apex Turbo Game Mode**:
   - Automatically scans and indexes all installed games from `/system_data/priv/mms/app.db`.
   - Allows selecting any game (e.g. *eFootball PES 2020*, *Bloodborne*, *God of War*) to engage **Apex Focus**.
   - Performs a complete pre-launch memory purge (dropping standby OS caches and killing non-essential background buffers).
   - Launches the game directly into the foreground.

2. **Hourly Background RAM Guard (Anti-Stutter & Anti-Lag Engine)**:
   - Spawns a background watchdog timer.
   - Every **1 hour (60 minutes)**, automatically cleans and reclaims standby memory and dirty file system pages.
   - **Crucial Invariant**: Explicitly protects and exempts the active game's Title ID and execution memory, ensuring your gameplay never stutters, crashes, or drops frames.

3. **Database Acceleration (SQLite VACUUM & REINDEX)**:
   - Safely backs up `app.db` to `app.db.apex_bak`.
   - Executes `VACUUM;` and `REINDEX;` on `app.db` and `notification.db`.
   - Eliminates database B-Tree fragmentation, making the PS4 Home Screen (XMB) load instantly with zero icon lag.

4. **Deep Junk & Cache Purge**:
   - Deep cleans orphaned package chunks in `/data/pkg/`.
   - Cleans corrupted or cancelled downloads in `/user/download/`.
   - Purges accumulated core dumps and error logs in `/system_data/priv/log/` and `/user/home/*/error_log/`.

5. **Thermal Guard & Anti-Throttling**:
   - Adjusts fan speed threshold to 62°C (instead of Sony's default 79°C).
   - Keeps the Jaguar CPU and GCN GPU cool, completely preventing thermal throttling during extended gaming sessions.

6. **Luxury 60 FPS UI**:
   - Dark Obsidian aesthetic with Neon Cyan and Apex Gold accents.
   - Real-time hardware telemetry: SOC temperature, free RAM, free storage, and live Apex Focus status.

---

## DualShock 4 Controller Controls

| Button | Function |
|---|---|
| **D-Pad Up / Down** | Browse installed game library |
| **Cross (X)** | Engage **Apex Game Mode** (Purge RAM & Launch with Hourly Guard) |
| **Square** | Defragment & Optimize System Database (`app.db` VACUUM) |
| **Triangle** | Deep Purge (Crash Dumps, Cache & Orphaned Downloads) |
| **Options** | Toggle Thermal Guard (Set Fan Threshold to 62°C) |
| **Circle (O)** | Refresh Game Library |

---

## Cloud Build & Packaging (`OrbisApex.pkg`)

This repository is pre-configured with a GitHub Actions workflow in `.github/workflows/build.yml`.
Simply push this repository to GitHub, and GitHub Actions will automatically compile the ELF and produce `OrbisApex.pkg` in GitHub Artifacts.
