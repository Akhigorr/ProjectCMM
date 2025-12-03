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
