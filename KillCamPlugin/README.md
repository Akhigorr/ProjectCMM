# Kill Cam Plugin (AAA)

A high-performance, single-player Kill Cam system for Unreal Engine 5.5+, designed for projectile-based games (arrows, bullets, magic).

## Features

*   **Dual Modes:**
    *   **Realtime Follow:** Camera rides the projectile immediately.
    *   **Predictive Trigger:** Camera only engages if a hit is predicted (via physics simulation or tracing).
*   **Predictive Look-Ahead:** Uses `PredictProjectilePath` (Physics) or Sphere Tracing to detect targets before impact.
*   **Dynamic Time Dilation:** Smooth slow-motion effects when the kill cam engages.
*   **Post-Process Hooks:** Exposes real-time data (`DistanceToTarget`, `SpeedRatio`) to Blueprints for driving materials, vignettes, or chromatic aberration.
*   **Zero-Setup Actor:** Includes `ABaseKillCamArrow` with pre-configured Spring Arm and Camera components.

## Installation

1.  Copy the `KillCamPlugin` folder into your project's `Plugins/` directory.
2.  Regenerate Visual Studio project files.
3.  Enable the plugin in **Edit > Plugins**.

## Usage

### Method 1: Using the Base Class
1.  Create a new Blueprint inheriting from `ABaseKillCamArrow`.
2.  Add your projectile mesh (e.g., Arrow Mesh) to the component hierarchy.
3.  Adjust the **SpringArm** and **Camera** settings in the Details panel to get your desired shot framing.

### Method 2: Adding to Existing Actors
1.  Add the `KillCam` component to your existing Projectile Actor.
2.  Ensure your actor has a `SpringArm` and `Camera` component if you want the "Ride along" effect.
3.  (Optional) Call `KillCamComponent->TriggerLookAhead()` manually in your Blueprint (e.g., just after firing).

### Configuration

Select the `KillCam` component to tweak these settings:

*   **Kill Cam Mode:** `Realtime` (always on) or `Predictive` (smart trigger).
*   **Look Ahead Method:** `Trace` (fast, linear) or `Physics` (accurate for arcs/gravity).
*   **Target Tag:** The Actor Tag (e.g., "Enemy") that triggers the camera.
*   **Target Time Dilation:** How slow time should get (e.g., `0.1` for 10% speed).
*   **Prediction Radius:** How wide the "hit box" for the camera trigger is.
*   **Debug Force Always Trigger:** Check this to force the Kill Cam to activate immediately (bypass prediction) for testing.

### Camera Control & Framing

The plugin can automatically manage the camera view for you.

*   **Auto Switch View:** If checked, the Kill Cam Component will call `SetViewTargetWithBlend` to switch to the arrow when it fires/triggers.
*   **Post Impact Delay:** How long (seconds) to stay on the arrow after it hits/stops before returning to the player.
*   **Blend Times:** Control how fast the camera cuts or smoothes to the arrow (`BlendToCamTime`, `BlendBackTime`).

**Framing Presets (on ABaseKillCamArrow):**
Instead of manually tweaking the SpringArm, select `FramingMode` in the Details panel:
*   `Standard Rear`: Classic 3rd person follow.
*   `Cinematic Side`: Cool side-angle shot.
*   `Top Down`: High angle view.
*   `Custom`: Use your own SpringArm settings.

**Smart Obstacle Avoidance:**
*   `Enable Camera Collision`: If true, the camera will pull in closer if a wall is in the way.
*   `Camera Probe Size`: Size of the collision check (default 12.0).

### Realistic Physics Configuration

The `URealisticArrowMovementComponent` provides AAA flight mechanics. Tweaking these values is crucial for feel:

*   **Aerodynamics:**
    *   `Quadratic Drag Coefficient`: Controls air resistance based on speed squared. (Try `0.0001` for subtle drag, `0.001` for heavy arrows).
    *   `Fletching Rotation Speed`: Degrees per second the arrow spins. (Try `360.0`). *Optimization Note: The component automatically finds and spins the first Mesh component it sees.*
    *   `Wind Vector`: Global wind force applied to the arrow.

*   **Impact:**
    *   `Ricochet Max Angle`: Impacts shallower than this angle will bounce. (e.g., `70` allows glancing hits). Use `Enable Bounce` to toggle.
    *   `Penetration Depth`: How deep the arrow sticks into the target (in units).
    *   **Hit Stop:** Toggle `Enable Hit Stop` to freeze the game for a split second (e.g., `0.05s`) on impact for that "Crunch" feel.

**Easy Stats Configuration:**
You can use the `ApplyBallisticStats` function to set multiple properties at once using the `FArrowBallisticStats` struct. This is useful for creating Data Tables of different arrow types (e.g., Light, Heavy, Magic).

*   **Archer's Paradox (Wiggle):**
    *   Arrows naturally oscillate when fired. The component calculates this but **you must visualize it**.
    *   Use the `CurrentOscillationValue` property in Blueprint to drive a Material Parameter (World Position Offset).
    *   **Blueprint Setup:**
        1. In `Tick`, Get `ArrowMovementComponent`.
        2. Read `CurrentOscillationValue`.
        3. Set Scalar Parameter Value on your Arrow Mesh's Dynamic Material Instance.
        4. In Material: Use `World Position Offset` to bend the mesh Left/Right based on this scalar.

### Post-Process & Effects
Bind to the `OnKillCamUpdate` event in your Blueprint to drive visual effects:

```blueprint
Event OnKillCamUpdate(float DistanceToTarget, float SpeedRatio)
{
    // Example: Increase Vignette intensity as distance gets closer to 0
    // Example: Add Radial Blur based on SpeedRatio
}
```

## Future Feature Roadmap

Here are suggestions for extending this plugin for a true "AAA" polish:

1.  **Cinematic Transition Manager:**
    *   Instead of hard cuts, blend between the Player Camera and the Arrow Camera using `SetViewTargetWithBlend`.
    *   Add "Swoop" curves to the camera movement.

2.  **Replay System Integration:**
    *   Record the last 5 seconds of gameplay.
    *   If a kill occurs, pause the game and replay the shot from the arrow's perspective (true Kill Cam) instead of live following.

3.  **Audio Filter Manager:**
    *   Audio Mix Modifier that muffles ambient sound and highlights the "Whoosh" of the arrow during slow motion.
    *   Heartbeat sound effect that speeds up as `DistanceToTarget` decreases.

4.  **Multi-Target logic:**
    *   Logic to handle piercing shots (killing multiple enemies in one line).
    *   "Best Target" selection if multiple enemies are in the prediction cone.

5.  **Impact Camera Shake:**
    *   A specific camera shake class that plays only when the arrow finally impacts the target.
