# Code Audit Report

**Date:** 2025-02-12
**Target:** KillCamPlugin Source Code
**Auditor:** Jules (Senior UE C++ Engineer)

## Executive Summary

The codebase is generally well-structured and adheres to many Unreal Engine 5 modern practices (e.g., usage of `TObjectPtr`, `TWeakObjectPtr`).

Initially, 4 critical Null Pointer Vulnerabilities were identified where `GetWorld()` was accessed without validation. **These have now been fixed.**

A secondary "Deep Dive" audit was performed focusing on Performance, State Resilience, Time Dilation, and Physics. **All identified high-priority issues have been fixed.**

---

## 1. Null Pointer Vulnerabilities (Resolved)

**Risk:** Immediate Crash (Access Violation)
**Description:** Calling `GetWorld()->Function()` without checking if `GetWorld()` returns a valid pointer.

### File: `KillCamComponent.cpp`
*   **Location:** `TickComponent` function.
*   **Status:** **FIXED**. Wrapped in `if (UWorld* World = GetWorld())`.

### File: `RealisticArrowMovementComponent.cpp`
*   **Location:** `PerformHitStop` function.
*   **Status:** **FIXED**. Wrapped in `if (UWorld* World = GetWorld())`.

### File: `ArcTarget_Switch.cpp`
*   **Location:** `HandleHit` function.
*   **Status:** **FIXED**. Wrapped in `if (UWorld* World = GetWorld())`.

### File: `ArcTarget_Nest.cpp`
*   **Location:** `OnChildDestroyed` function.
*   **Status:** **FIXED**. Wrapped in `if (UWorld* World = GetWorld())`.

---

## 2. Best Practice / Stability Notes

**Risk:** Logic Errors / Level Streaming Issues
**Description:** Observations that do not strictly violate the crash checklist but are worth noting for robust architecture.

### File: `ArcTarget_Switch.h` & `ArcTarget_Nest.h`
*   **Issue:** `LinkedTarget` and `ChildTarget` are `TObjectPtr<AArcTarget>` (Hard References).
*   **Details:** While GC-safe (marked with `UPROPERTY`), hard references between actors can cause issues if the referenced actor is in a different streaming level that is unloaded, potentially keeping it in memory or causing reference issues.
*   **Recommendation:** Consider using `TSoftObjectPtr<AArcTarget>` or `TWeakObjectPtr<AArcTarget>` if these relationships span across disparate parts of the world, though for a tightly coupled plugin system, `TObjectPtr` is often acceptable.

### File: `KillCamComponent.cpp`
*   **Issue:** `TickComponent` relies on `OwnerActor` (WeakPtr).
*   **Details:** The code correctly checks `if (!OwnerActor.IsValid()) { StopKillCam(); return; }`. This is a **Good Practice** example found in the code.

---

## 3. Checklist Status

| Category | Status | Notes |
| :--- | :---: | :--- |
| **Garbage Collection** | **PASS** | No raw `UObject*` detected. `TObjectPtr` used correctly. |
| **Null Pointers** | **PASS** | All unsafe `GetWorld()` dereferences have been patched. |
| **Thread Safety** | **PASS** | No async gameplay logic detected. |
| **Container Safety** | **PASS** | Iteration logic appears safe. |
| **Constructor Safety** | **PASS** | Constructors do not access World or Singletons. |
| **Casting & Types** | **PASS** | Standard `Cast<T>` and `IsA` used correctly. |

---

## 4. Deep Dive Findings (Resolved)

### A. Performance & Optimization
*   **Issue:** `TickComponent` was calling `TriggerLookAhead()` every frame.
*   **Status:** **FIXED**.
*   **Fix:** Added `PredictionInterval` (default 0.1s) to throttle prediction checks to 10Hz, significantly reducing CPU load.

### B. Time Dilation Conflicts (Architecture)
*   **Issue:** Components were modifying `GlobalTimeDilation` directly, leading to conflicts.
*   **Status:** **FIXED**.
*   **Fix:** Implemented `RegisterTimeDilationRequest` and `UnregisterTimeDilationRequest` in `UKillCamWorldSubsystem`. Updated `KillCamComponent` and `RealisticArrowMovementComponent` to use this managed system, ensuring the lowest dilation value is applied safely.

### C. Physics Tunneling
*   **Issue:** Fast arrows risked tunneling through objects.
*   **Status:** **FIXED**.
*   **Fix:** Enabled `SetUseCCD(true)` on the `ABaseKillCamArrow` collision component.
