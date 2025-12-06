# User Manual: Kill Cam Plugin

## Overview
This plugin provides a robust solution for "Sniper Elite" or "Skyrim" style arrow follow cameras. It handles the physics of the arrow, the camera movement, and the slow-motion logic.

## 1. Getting Started
1.  **Enable Plugin:** Go to Edit -> Plugins and ensure `KillCamPlugin` is enabled.
2.  **Create Arrow:** Right-click in Content Browser -> Blueprint Class -> Search `BaseKillCamArrow`. Name it `BP_MyArrow`.
3.  **Add Mesh:** Open `BP_MyArrow`. Add a Static Mesh Component (e.g., your arrow model) under the root sphere.
4.  **Configure:** Click `ArrowMovementComponent` in the components list. Adjust `Gravity Scale` (try 0.5) and `Initial Speed` (when spawning).

## 2. Spawning the Arrow (The Archer Component)
The recommended way to fire arrows is using the **Archer Component**. It handles spawn transforms, velocity, and ownership automatically.

1.  **Add Component:** In your Character Blueprint (e.g., `BP_Archer`), add the `Archer` component.
2.  **On Input "Fire":**
    *   Call `FireArrow` on the Archer Component.
    *   **Arrow Class:** Select your `BP_MyArrow`.
    *   **Spawn Location:** Get your socket location (e.g., `GetSocketLocation` on your Bow Mesh).
    *   **Launch Direction:** Get your Camera's Forward Vector (or the Bow's Forward Vector).
    *   **Speed:** Set the initial speed (e.g., 3000).
3.  **Done!** The arrow spawns with the correct velocity and ownership settings.

*(Advanced)* **Manual Spawning:**
If you prefer manual control:
1.  Spawn Actor from Class (`BP_MyArrow`).
2.  Set Owner and Instigator to `Self`.
3.  Set Velocity on the `ArrowMovementComponent` manually immediately after spawn.

## 3. Setting Up Targets
To make the predictive system work, your enemies need to be identified.
1.  **Tagging:** Select your Enemy Actor. Search "Tags". Add a tag called `Enemy`.
2.  **Plugin Config:** On the arrow's `KillCamComponent`, set `Target Tag` to `Enemy`.

## 4. Environment (Wind & Gravity)
To add global effects like Wind:
1.  Drag `ArrowEnvironmentManager` into your level.
2.  In Details, set `Global Wind` (e.g., X=500 for a strong breeze).
3.  All arrows in the level will now drift!

## 5. Destructible Targets
The plugin includes `AArcTarget` actors (Vases, Statutes, etc).
1.  Create a Blueprint inheriting from `AArcTarget`.
2.  Assign a Geometry Collection to the `GeometryCollection` component.
3.  When hit by an arrow, they will `Shatter`.

## 6. Debugging
If the camera isn't triggering:
1.  Select the Arrow Blueprint.
2.  Select `KillCamComponent`.
3.  Check `bDebugForceAlwaysTrigger`.
4.  Play. The camera should now *always* follow. If not, check if your Arrow is moving.
5.  Check `bDrawDebugPrediction` to see the line/sphere trace visualization.

## 7. Hit Stop (Crunch)
To make impacts feel heavy:
1.  Select `ArrowMovementComponent`.
2.  Check `bEnableHitStop`.
3.  Set `Hit Stop Duration` to `0.05` or `0.1`.
4.  The game will freeze briefly when the arrow hits.

## FAQ

**Q: The camera clips through walls.**
A: Enable `bEnableCameraCollision` on the arrow's details.

**Q: The arrow wiggles too much.**
A: Reduce `InitialOscillationAmplitude` on the Movement Component.

**Q: My arrow falls too fast.**
A: Lower `Gravity Scale` on the Movement Component or `Global Gravity Scalar` in the Environment Manager.
