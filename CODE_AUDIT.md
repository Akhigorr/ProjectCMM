# Code Audit Report

**Date:** 2025-02-12
**Target:** KillCamPlugin Source Code
**Auditor:** Jules (Senior UE C++ Engineer)

## Executive Summary

The codebase is generally well-structured and adheres to many Unreal Engine 5 modern practices (e.g., usage of `TObjectPtr`, `TWeakObjectPtr`). However, there are several instances of **potential runtime crashes** related to accessing `GetWorld()` without validation. While `GetWorld()` is typically valid during gameplay `Tick` and `BeginPlay`, it can return `nullptr` during actor destruction, level transitions, or in specific editor contexts, leading to immediate hard crashes when dereferenced.

No severe Garbage Collection (GC) risks (dangling raw pointers) or Thread Safety violations were found.

---

## 1. Null Pointer Vulnerabilities (Critical)

**Risk:** Immediate Crash (Access Violation)
**Description:** Calling `GetWorld()->Function()` without checking if `GetWorld()` returns a valid pointer.

### File: `KillCamComponent.cpp`
*   **Location:** `TickComponent` function.
*   **Issue:** `GetWorld()->GetTimerManager()` is called directly.
*   **Snippet:**
    ```cpp
    if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_StopCam))
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_StopCam, this, &UKillCamComponent::StopKillCam, PostImpactDelay, false);
    }
    ```
*   **Recommendation:** Wrap in `if (UWorld* World = GetWorld()) { ... }`.

### File: `RealisticArrowMovementComponent.cpp`
*   **Location:** `PerformHitStop` function.
*   **Issue:** `GetWorld()->GetTimerManager()` is called directly.
*   **Snippet:**
    ```cpp
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_HitStop, this, &URealisticArrowMovementComponent::StopHitStop, Delay, false);
    ```
*   **Recommendation:** Wrap in `if (UWorld* World = GetWorld())`.

### File: `ArcTarget_Switch.cpp`
*   **Location:** `HandleHit` function.
*   **Issue:** `GetWorld()->GetTimerManager()` is called directly.
*   **Snippet:**
    ```cpp
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_HideLinked, this, &AArcTarget_Switch::HideLinkedTarget, RevealDuration, false);
    ```
*   **Recommendation:** Check `GetWorld()` before accessing TimerManager.

### File: `ArcTarget_Nest.cpp`
*   **Location:** `OnChildDestroyed` function.
*   **Issue:** `GetWorld()->GetTimerManager()` is called directly.
*   **Snippet:**
    ```cpp
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Respawn, this, &AArcTarget_Nest::RespawnChild, RegenTime, false);
    ```
*   **Recommendation:** Check `GetWorld()` before accessing TimerManager.

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
| **Null Pointers** | **FAIL** | 4 instances of unsafe `GetWorld()` dereferencing. |
| **Thread Safety** | **PASS** | No async gameplay logic detected. |
| **Container Safety** | **PASS** | Iteration logic appears safe. |
| **Constructor Safety** | **PASS** | Constructors do not access World or Singletons. |
| **Casting & Types** | **PASS** | Standard `Cast<T>` and `IsA` used correctly. |
