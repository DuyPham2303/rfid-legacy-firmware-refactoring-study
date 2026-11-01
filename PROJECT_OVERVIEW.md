# RFID Legacy Firmware: Refactoring Study

## Purpose

This repository preserves and examines an existing RFID access-control firmware project for the STM32F103C8T6. It is the baseline workspace for understanding the original design, documenting its constraints, and making measured improvements before carrying a refactored architecture into a separate project.

The aim is to show an engineering process: understand the existing behavior, identify requirements and technical gaps, make changes that can be explained and reviewed, and use the evidence to guide a later redesign. The legacy code is a learning and comparison baseline, not a claim that it already meets automotive or AUTOSAR requirements.

## Engineering objectives

- Trace the current control flow and peripheral usage, including RFID, LCD, servo, system tick, and SD card support.
- Improve maintainability through explanatory comments, clearer interfaces, and small wrappers where they make existing behavior easier to understand.
- Complete unfinished legacy features in the original design style when that helps expose its constraints.
- Experiment with self-written MCAL modules against the existing board support code.
- Record observations about memory use, module boundaries, testability, and failure handling to inform a later refactor.
- Use V-model thinking to connect requirements, implementation, and verification evidence; study AUTOSAR concepts as a direction for the future architecture.

CAN support and a full AUTOSAR-oriented redesign are future learning goals. They are not represented as completed work in this legacy baseline.

## Scope and change discipline

This repository focuses on reviewable firmware source and the minimum supporting files needed to understand the legacy project. Generated build outputs, local editor state, Wokwi simulation configuration, and local data files are excluded. Relevant C source and headers, startup assembly, linker/build files, and project documentation may be tracked.

Changes to this legacy code should preserve the original behavior unless a change is explicitly documented and needed for an investigation or a deliberately completed legacy feature. Each versioned commit uses a short `Vx.y` title and updates the README with a concise summary of what changed from the preceding version.

## Version history

### V1.0 — Legacy Project baseline

- Establishes the current workspace as the review baseline for the legacy RFID firmware.
- Documents the purpose and boundaries of this repository and its source-focused tracking policy.
- Does not claim functional refactoring or automotive compliance.

## Intended portfolio use

This repository can support a CV or interview discussion about the progression from legacy embedded firmware analysis to a planned, evidence-based refactor. Future entries should describe only work that has actually been implemented and reviewed, and should link changes to their rationale and verification results.
