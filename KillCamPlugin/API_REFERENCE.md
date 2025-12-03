# Kill Cam Plugin API Reference

This document provides a detailed reference for all Classes, Components, Structs, Enums, and Blueprints Nodes exposed by the Kill Cam Plugin.

## Table of Contents
1. [Components](#components)
    *   [UKillCamComponent](#ukillcamcomponent)
    *   [URealisticArrowMovementComponent](#urealisticarrowmovementcomponent)
2. [Actors](#actors)
    *   [ABaseKillCamArrow](#abasekillcamarrow)
3. [Structs](#structs)
    *   [FArrowBallisticStats](#farrowballisticstats)
4. [Enums](#enums)

---

## Components

### UKillCamComponent
The core logic component responsible for handling the camera transition, slow motion (time dilation), and predictive target locking.

**Inheritance:** `UActorComponent`

#### Properties (Category: Kill Cam | Config)
| Type | Name | Description |
| :--- | :--- | :--- |
| `EKillCamMode` | **KillCamMode** | Determines if the cam follows immediately (`Realtime`) or waits for a predicted hit (`Predictive`). |
| `ELookAheadMethod` | **LookAheadMethod** | The algorithm used for prediction: `Trace` (Line/Sphere) or `Physics` (Projectile Arc). |
| `FName` | **TargetTag** | The Actor Tag (e.g., "Enemy") required on a hit actor to trigger the kill cam in Predictive mode. |

#### Properties (Category: Kill Cam | Visuals)
| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **TargetTimeDilation** | The global time scale to apply when Kill Cam activates (e.g., `0.1` for 10% speed). |
| `UCurveFloat*` | **TimeDilationCurve** | *Optional.* Curve to drive smooth transitions for time dilation. |
| `float` | **CameraLagSpeed** | Controls how smoothly the camera follows the arrow position. Higher is tighter. |

#### Properties (Category: Kill Cam | Prediction)
| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **LookAheadDistance** | Max distance to check ahead for targets. |
| `float` | **PredictionRadius** | Radius of the sphere check/projectile simulation. |

#### Properties (Category: Kill Cam | Debug)
| Type | Name | Description |
| :--- | :--- | :--- |
| `bool` | **bDebugForceAlwaysTrigger** | If true, `TriggerLookAhead` always succeeds. Useful for testing camera flow without enemies. |

#### Properties (Category: Kill Cam | Camera Control)
| Type | Name | Description |
| :--- | :--- | :--- |
| `bool` | **bAutoSwitchView** | If true, automatically calls `SetViewTarget` to switch to the arrow's camera. |
| `float` | **PostImpactDelay** | Time (in seconds) to linger on the arrow after it hits/stops before returning to player. |
| `float` | **BlendToCamTime** | Duration of blend transition **to** the arrow camera. |
| `float` | **BlendBackTime** | Duration of blend transition **back** to the player. |

#### Events (Delegates)
| Event Name | Signature | Description |
| :--- | :--- | :--- |
| **OnKillCamUpdate** | `(float DistanceToTarget, float SpeedRatio)` | Fired every tick. Use to drive Post Process Materials (e.g., Vignette, Blur). |
| **OnKillCamStart** | `()` | Fired when slow motion engages. |
| **OnKillCamEnd** | `()` | Fired when the kill cam sequence finishes. |

#### Functions
| Function | Description |
| :--- | :--- |
| `bool TriggerLookAhead()` | Manually runs the prediction logic. Returns true if a target was found (or Debug Force is on). |
| `void StartKillCam()` | Forces the Kill Cam to start (Slow mo + Camera Switch). |
| `void StopKillCam()` | Ends the Kill Cam (Restores time + Camera). |

---

### URealisticArrowMovementComponent
An advanced projectile physics engine that replaces standard movement to add AAA arrow behaviors.

**Inheritance:** `UProjectileMovementComponent`

#### Functions
| Function | Description |
| :--- | :--- |
| `void ApplyBallisticStats(FArrowBallisticStats Stats)` | Overwrites physics settings (Drag, Gravity, etc.) from a single struct. Useful for presets. |

#### Properties (Category: Arrow Physics | Aerodynamics)
| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **QuadraticDragCoefficient** | Air resistance based on Velocity squared. Makes arrows slow down naturally. |
| `float` | **FletchingRotationSpeed** | Spin rate (Degrees/Second). Rotates the arrow mesh for stability visuals. |
| `FVector` | **WindVector** | Constant global wind force applied to the arrow. |

#### Properties (Category: Arrow Physics | Impact)
| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **RicochetMaxAngle** | Impacts shallower than this angle (degrees from normal) will bounce. |
| `float` | **PenetrationDepth** | Distance the arrow embeds into the target mesh upon sticking. |
| `bool` | **bEnableBounce** | If false, arrows always stick (ignoring Ricochet angle). |
| `bool` | **bEnableHitStop** | If true, freezes the game for a split second on impact ("Crunch" feel). |
| `float` | **HitStopDuration** | Duration of the hit stop freeze (real-time seconds). |

#### Properties (Category: Arrow Physics | Oscillation)
| Type | Name | Description |
| :--- | :--- | :--- |
| `float` | **InitialOscillationAmplitude** | Strength of the "Archer's Paradox" wiggle when fired. |
| `float` | **OscillationFrequency** | Speed of the wiggle. |
| `float` | **OscillationDamping** | How fast the wiggle fades out. |
| `float` | **CurrentOscillationValue** | **[Read Only]** Bind this to World Position Offset in your Material to visualize the bending. |

---

## Actors

### ABaseKillCamArrow
A "batteries-included" actor that assembles the components and provides camera framing presets.

#### Components
*   `RootComponent` (Scene)
*   `ArrowMovementComponent` (`URealisticArrowMovementComponent`)
*   `KillCamComponent` (`UKillCamComponent`)
*   `CameraBoom` (`USpringArmComponent`)
*   `FollowCamera` (`UCameraComponent`)

#### Properties (Category: Kill Cam | Framing)
| Type | Name | Description |
| :--- | :--- | :--- |
| `EKillCamFramingMode` | **FramingMode** | Preset for camera position: `StandardRear`, `CinematicSide`, `TopDown`, or `Custom`. |
| `float` | **KillCamFOV** | Field of View for the Kill Cam. |
| `bool` | **bEnableCameraCollision** | Checks for wall collisions (Obstacle Avoidance). |
| `float` | **CameraProbeSize** | Collision sphere size for the camera boom. |

#### Functions
| Function | Description |
| :--- | :--- |
| `void ApplyFramingPreset()` | Re-applies the SpringArm settings based on the selected `FramingMode`. Called automatically on Construction/BeginPlay. |

---

## Structs

### FArrowBallisticStats
A data structure for easily passing around arrow configurations (e.g., from a Data Table).

| Type | Name | Default |
| :--- | :--- | :--- |
| `float` | **QuadraticDragCoefficient** | `0.0001` |
| `float` | **GravityScale** | `1.0` |
| `float` | **FletchingRotationSpeed** | `360.0` |
| `float` | **PenetrationDepth** | `15.0` |
| `bool` | **bEnableBounce** | `true` |

---

## Enums

### EKillCamMode
*   `Realtime`: Camera follows immediately.
*   `Predictive`: Camera waits for a confirmed future hit.

### ELookAheadMethod
*   `Trace`: Simple Line/Sphere trace.
*   `Physics`: Full projectile path simulation (gravity/arcs).

### EKillCamFramingMode
*   `StandardRear`: Classic 3rd person chase cam.
*   `CinematicSide`: Offset to the side for a dramatic angle.
*   `TopDown`: High angle view.
*   `Custom`: Uses manually set SpringArm values.
