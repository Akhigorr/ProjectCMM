# Kill Cam Plugin: The Complete Guide

**Version:** 1.0
**Target Engine:** Unreal Engine 5.5+
**Platform:** Win64

---

## 1. Overview

The **Kill Cam Plugin** is a high-performance system designed for projectile-based games (archery, magic, ballistics). It provides three core pillars:
1.  **AAA Camera System:** Smoothly follows projectiles with optional "Predictive" slow-motion triggers.
2.  **Realistic Physics:** A custom movement component simulating drag, wind, spin, and ricochets.
3.  **Gameplay Targets:** A suite of 12 puzzle-ready target actors with destruction and scoring built-in.

---

## 2. Integration (Getting Started)

### Step 1: Plugin Activation
1.  Place the `KillCamPlugin` folder in your project's `Plugins/` directory.
2.  Open your project.
3.  Go to **Edit > Plugins**. Ensure `KillCamPlugin` is enabled.
4.  Restart the Editor.

### Step 2: Creating your Arrow
1.  Right-click in the Content Browser -> **Blueprint Class**.
2.  Search for and select **`BaseKillCamArrow`**. Name it `BP_MyArrow`.
3.  Open `BP_MyArrow`.
4.  **Add Mesh:** Attach a Static Mesh (your arrow model) to the **SceneRoot**.
    *   *Crucial:* Set the Mesh Collision Preset to **NoCollision**. The root Sphere component handles all physics.
    *   *Visuals:* Align the mesh so it points along the **Forward (+X)** axis.

### Step 3: Firing the Arrow (The Archer Component)
Do not use `SpawnActor` manually if you want robust ownership and velocity handling. Use the helper component.

1.  Open your Character Blueprint (e.g., `BP_Player`).
2.  Add the **`Archer`** component (`UArcherComponent`).
3.  In your Input Event (e.g., "Fire"):
    *   Call **`FireArrow`** on the Archer Component.
    *   **Arrow Class:** Select `BP_MyArrow`.
    *   **Spawn Location:** Get the world location of your Bow/Hand socket.
    *   **Launch Direction:** Get `FollowCamera -> GetForwardVector`.
    *   **Speed:** Set to `3000` (approx 30m/s).

---

## 3. The Kill Cam System

The logic lives in the `KillCam` component on your Arrow.

### Modes (`KillCamMode`)
*   **Realtime:** The camera snaps to the arrow immediately upon firing. Best for long-range sniping where the player steers the shot.
*   **Predictive:** The camera **only** activates if the plugin calculates a hit is imminent.
    *   It runs a physics simulation 2 seconds into the future every tick.
    *   If it detects an Actor with the **Target Tag** (default: "Enemy"), it triggers Slow Motion and switches the camera.

### Framing & Feel
*   **Target Time Dilation:** Controls the slow-mo speed (e.g., `0.1` = 10% speed).
*   **Post Impact Delay:** How long (seconds) the camera lingers on the target after the arrow hits.
*   **Framing Mode:**
    *   `Standard Rear`: Classic 3rd person chase.
    *   `Cinematic Side`: A dramatic low-angle side view.
    *   `Top Down`: Tactical view.
*   **Obstacle Avoidance:** Check `Enable Camera Collision` to prevent the camera from clipping through walls in tight spaces.

---

## 4. Realistic Physics Engine

The `URealisticArrowMovementComponent` replaces the standard Projectile Movement.

### Aerodynamics
*   **Quadratic Drag:** Air resistance increases with speed squared.
    *   `0.0001`: Sniper/Bullet (very flat trajectory).
    *   `0.001`: Hunting Arrow (notable arc).
*   **Wind:** Affected by the global `ArrowEnvironmentManager`.
*   **Fletching Rotation:** Visual spin (Degrees/Sec). The component automatically finds your mesh and spins it.

### Impact & Ricochet
*   **Ricochet Max Angle:** (e.g., `70.0`). If the arrow hits a surface at a shallow angle (> 70 deg from normal), it will bounce instead of stick.
*   **Penetration Depth:** How deep the arrow embeds into the target (visual only).
*   **Hit Stop (The "Crunch"):**
    *   Enable this to freeze the game for `0.05s` on impact. This adds tremendous "weight" to the hit.
    *   *Note:* Works safely even if the game is already in Slow Motion.

### Visuals: Archer's Paradox (Wiggle)
Real arrows oscillate when fired. To visualize this:
1.  In `BP_MyArrow` -> **Event Tick**, get the `ArrowMovementComponent`.
2.  Read the **`CurrentOscillationValue`** float.
3.  Pass this value to a **Scalar Parameter** on your Arrow's Dynamic Material Instance.
4.  In the Material, use **World Position Offset** to bend the mesh Left/Right based on this scalar.

---

## 5. The Target System

The plugin includes `AArcTarget`, a destructible actor class.

### Core Features
*   **Destruction:** Uses UE5 **Chaos Geometry Collections**. When hit, the target shatters.
*   **Scoring:** Each target has a `ScoreValue`. Points are automatically sent to the World Subsystem.
*   **Collision:** Upon shattering, the collision profile switches to **"Destructible"** (debris) to prevent blocking subsequent shots.

### Target Types (Subclasses)
1.  **Static Sphere:** Basic target.
2.  **Orbiter:** Rotates around a Pivot Actor.
3.  **Spline Mover:** Follows a Spline Component path (Ping-Pong or Loop).
4.  **Mimic:** Dodges sideways when a projectile approaches. *Only dodges valid arrows, ignores players/walls.*
5.  **Shielded:** Only takes damage from the **back**. (Use ricochets or gravity shots).
6.  **Black Hole:** Sucks arrows in with radial force.
7.  **Switch:** Does not shatter. Toggles visibility/collision of a `LinkedTarget`.
8.  **Ghost:** Invisible/Intangible until revealed (requires custom gameplay logic to call `SetGhostVisibility`).
9.  **Mirror:** Bounces direct hits. Only shatters from Ricochets.
10. **Nest:** Spawns/Respawns a child target after a delay.
11. **Donut:** Separate hit detection for "Bullseye" vs "Ring".
12. **Chime:** Broadcasts a gameplay tag event when hit (for musical puzzles).

---

## 6. World Environment & Scoring

### Global Physics
Drag an **`AArrowEnvironmentManager`** into your level.
*   **Global Wind:** Applies force to all active arrows.
*   **Gravity Scalar:** Multiplier (e.g., 0.5 for Moon gravity).
*   **Audio Ducking:** Assign a **Sound Mix**. When Kill Cam starts, this mix is Pushed (muffling ambient sound). When it ends, it is Popped.

### Scoring Subsystem
The plugin tracks score globally via `UKillCamWorldSubsystem`.
*   **Get Score:** `GetWorld()->GetSubsystem<UKillCamWorldSubsystem>()->GetTotalScore()`.
*   **Events:** Bind to `OnScoreChanged(Total, Added)` to update your UI.

---

## 7. Best Practices & "Things to Watch For"

### Level Streaming
The plugin is hardened for World Partition and Level Streaming.
*   **Safety:** The Subsystem uses `WeakObjectPtr` for all references. If you unload a level containing arrows or targets, the system will **not crash**.
*   **Cleanup:** If an arrow is destroyed (e.g., level unload) while Slow Motion is active, the Subsystem automatically detects the stale request and restores game speed.
*   **Environment:** The `EnvironmentManager` resets physics to default when it is streamed out.

### Performance
*   **Caching:** The components cache the Subsystem pointer in `BeginPlay`. Do not spawn thousands of arrows per frame, but hundreds are fine.
*   **Prediction:** The `Predictive` Kill Cam mode runs a physics simulation every tick. For massive hordes (500+ arrows), switch to `Realtime` mode or lower the `LookAheadDistance`.

### Architecture
*   **Damage:** The arrow uses `UGameplayStatics::ApplyPointDamage`. Ensure your custom actors override `TakeDamage` if you want them to react to arrows (e.g., Enemy Character health).
*   **Tags:** Ensure your Enemies have the **Actor Tag** specified in the KillCam Component (Default: `Enemy`). Otherwise, prediction will ignore them.

---

## 8. FAQ

**Q: My arrow hits the target but it doesn't break.**
A: Ensure the Target inherits from `AArcTarget`. Ensure the Arrow's `ArrowMovementComponent` has a `HitStop` duration or standard setup (it applies damage on stick). Check that the Target has `Health` > 0.

**Q: The camera clips into the wall during the kill cam.**
A: Enable **`Enable Camera Collision`** on the `BaseKillCamArrow` (Details Panel). Adjust `Camera Probe Size`.

**Q: The "Aim Line" doesn't match where the arrow goes.**
A: If you are drawing your own debug line using `PredictProjectilePath`, it is WRONG. You must use `UKillCamStatics::PredictArrowPath`, which accounts for Drag and Wind.

**Q: The game gets stuck in Slow Motion.**
A: This happens if you modified the code to bypass the Subsystem. Use `RequestTimeDilation` on the Subsystem, never set `GlobalTimeDilation` directly. The plugin's built-in safety checks should prevent this in normal usage.

**Q: Can I use this for bullets?**
A: Yes. Set `Gravity Scale` to 0, increase `Initial Speed`, and reduce `Drag`.

**Q: Does it work in Multiplayer?**
A: **No.** Time Dilation (Slow Mo) is a global effect in Unreal Engine. Pausing/Slowing time for one player slows it for everyone. This plugin is designed for **Single Player** experiences.
