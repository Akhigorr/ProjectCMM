# Kill Cam Plugin: User Manual

Welcome to the **Kill Cam Plugin**! This manual covers every feature of the system, from setting up realistic arrow physics to designing complex puzzles with the 12 included target types.

---

## 1. Quick Start

### Setting up the Arrow
1.  Navigate to the `KillCamPlugin` content folder (or C++ classes).
2.  Create a Blueprint inheriting from **`ABaseKillCamArrow`**.
3.  Add your Arrow Mesh to the **SceneRoot**.
4.  **Important:** Ensure your Mesh has **No Collision** (collision is handled by the root Sphere).
5.  Drag the actor into the level or spawn it via your Character.

### Setting up a Target
1.  Create a Blueprint inheriting from one of the **`AArcTarget`** subclasses (e.g., `AArcTarget_StaticSphere`).
2.  Select the **GeometryCollection** component and assign your Chaos Geometry Collection asset (the destructible mesh).
3.  Drag the target into the level.
4.  Shoot the arrow at the target!

---

## 2. Realistic Arrow Physics

The arrow uses a custom physics engine (`URealisticArrowMovementComponent`) designed for "AAA" feel.

### Key Settings (Details Panel)
*   **Aerodynamics > Quadratic Drag Coefficient:**
    *   controls air resistance.
    *   `0.0001`: Light drag (Sniper feel).
    *   `0.001`: Heavy drag (Recurve bow feel).
*   **Aerodynamics > Fletching Rotation Speed:**
    *   How fast the arrow spins (Degrees/Sec). Purely visual but crucial for stability.
*   **Impact > Ricochet Max Angle:**
    *   If the arrow hits a wall at a shallow angle (e.g., > 70 degrees), it will **bounce** instead of sticking.
    *   Uncheck **Enable Bounce** to make arrows always stick.
*   **Impact > Hit Stop:**
    *   **Enable Hit Stop**: Freezes the game for a split second (e.g., `0.05s`) when the arrow hits *anything*. This gives a satisfying "Crunch" or "Impact" feel.

### The "Archer's Paradox" (Wiggle)
Real arrows bend when fired. This plugin calculates the math for you.
1.  In your Arrow Blueprint, go to **Event Tick**.
2.  Get the `ArrowMovementComponent` -> Get **`CurrentOscillationValue`**.
3.  Pass this float to your Arrow Material (via Dynamic Material Instance).
4.  In the Material, use "World Position Offset" to bend the mesh left/right based on this value.

---

## 3. The Kill Cam System

The camera system is built into the arrow (`UKillCamComponent`) and can work in two modes.

### Modes
*   **Realtime:** The camera follows the arrow immediately upon spawning. Good for "Sniper Elite" style shots where you control the projectile.
*   **Predictive:** The camera **only** activates if the plugin calculates that you are about to hit an enemy.
    *   Uses `PredictProjectilePath` to simulate the arc 2 seconds into the future.
    *   If a hit is detected, the camera snaps to the arrow and Slow Motion engages.

### Camera Framing
Select the `ABaseKillCamArrow` in the level (or BP) and find **Framing Mode**:
*   **Standard Rear:** Classic 3rd person follow.
*   **Cinematic Side:** A dramatic side-angle view.
*   **Top Down:** Good for tactical views.
*   **Custom:** Use the SpringArm component to set your own distance/offset.

### Obstacle Avoidance
If the arrow flies through a narrow window, you don't want the camera clipping through the wall.
*   Check **Enable Camera Collision**.
*   Adjust **Camera Probe Size** (default 12.0) to determine how sensitive the camera is to walls.

---

## 4. Target Encyclopedia

The plugin includes 12 specialized target types for puzzle design.

### 1. Static Sphere (`AArcTarget_StaticSphere`)
*   **Behavior:** A standard target. Shatters when hit.
*   **Usage:** Filler targets, basic practice.

### 2. Orbiter (`AArcTarget_Orbiter`)
*   **Behavior:** Rotates around a specific Actor in the level.
*   **Setup:**
    *   Place a "Pivot" actor (e.g., a simple Cube) in the level.
    *   In the Orbiter Target, set **Pivot Actor** to that Cube.
    *   Adjust **Radius** and **Rotation Speed**.

### 3. Spline Mover (`AArcTarget_SplineMover`)
*   **Behavior:** Moves back and forth along a curved path.
*   **Setup:**
    *   Create an Actor with a **Spline Component**.
    *   In the Target, set **Spline Path Actor** to that actor.
    *   Check **Ping Pong** to make it reverse at the end.

### 4. Shielded (`AArcTarget_Shielded`)
*   **Behavior:** Armored from the front. Only shatters if hit from **behind**.
*   **Usage:** Place against walls so players must ricochet shots or use "Black Hole" arrows to hit the back.

### 5. Switch (`AArcTarget_Switch`)
*   **Behavior:** Does NOT shatter. Instead, it reveals (unhides) another target for a short time.
*   **Setup:**
    *   Place a Hidden Target nearby.
    *   In the Switch, set **Linked Target** to the hidden one.
    *   Set **Reveal Duration** (e.g., 5.0 seconds).

### 6. Chime (`AArcTarget_Chime`)
*   **Behavior:** Broadcasts an Event with a specific ID when hit.
*   **Usage:** Musical puzzles. Set IDs 1, 2, 3. GameMode checks if hit in order.

### 7. Ghost (`AArcTarget_Ghost`)
*   **Behavior:** Invisible and Intangible (No Collision) by default.
*   **Usage:** The player must have a mechanic (like a "Lens of Truth") that calls `SetGhostVisibility(true)` via the `IArcTarget_GhostInterface`. Only then can it be hit.

### 8. Mirror (`AArcTarget_Mirror`)
*   **Behavior:** Deflects direct shots (bounces arrow away). Only shatters if hit by a **Ricochet** (an arrow that has already bounced once).

### 9. Donut (`AArcTarget_Donut`)
*   **Behavior:** Has a "Center" bullseye and an "Outer" ring.
*   **Logic:** Hitting the center triggers a special event (`OnCenterHit`) and double score.

### 10. Nest (`AArcTarget_Nest`)
*   **Behavior:** Protects a "Child" target. If the Child is destroyed, the Nest waits `RegenTime` seconds and then **Respawns** the child.
*   **Usage:** Infinite wave spawners or regenerative shields.

### 11. Mimic (`AArcTarget_Mimic`)
*   **Behavior:** Tries to dodge the arrow.
*   **Logic:** When an arrow enters its **Detection Radius**, the target quickly moves sideways (`Dodge Distance`) to avoid being hit.

### 12. Black Hole (`AArcTarget_BlackHole`)
*   **Behavior:** Sucks arrows towards its center using a **Radial Force**.
*   **Difficulty:** High. You must aim slightly away to account for the gravitational pull.

---

## 5. World Environment & Audio

You can control the physics for an entire level using the **Arrow Environment Manager**.

### Setup
1.  Drag an **`AArrowEnvironmentManager`** into your level.
2.  **Global Wind:** Set a vector (e.g., X=500) to blow all arrows sideways.
3.  **Global Gravity:** Set to `0.5` for "Moon Gravity" arrows.
4.  **Global Drag:** Set to `2.0` for a "Thick Fog" or underwater level.

### Audio Ducking
You want the sound to dampen (duck) when the Kill Cam starts (Slow Mo).
1.  Create a **Sound Mix** asset in UE5.
2.  Set it to lower the volume of the "Music" and "SFX" Sound Classes.
3.  Assign this Sound Mix to the **Kill Cam Sound Mix** property on the Environment Manager actor.
4.  The plugin automatically Pushes/Pops this mix when the camera engages/disengages.

---

## 6. Debugging

If things aren't working:
1.  **Force Kill Cam:** Select your arrow and check **Debug Force Always Trigger**. This will activate the camera instantly on fire, even if you miss.
2.  **Visualize Prediction:** Check **Draw Debug Prediction** on the arrow component to see the line/arc the system is checking.
3.  **Output Log:** Filter by `LogKillCam` to see detailed logs about impact angles, target detection, and physics states.
