# RFID Legacy Firmware: Baseline Completion and Architecture Study

## Purpose

This repository develops the existing STM32F103C8T6 RFID access-control project in place. The aim is to complete and improve the legacy project baseline through firmware edits, module restructuring, defect investigation, peripheral integration, and build support while keeping its overall legacy architecture recognizable.

Keeping the legacy architecture makes it possible to evaluate the original design and the effects of incremental changes before using those findings to plan a later AUTOSAR-oriented refactor. This repository documents a learning and engineering process; it does not claim automotive qualification, AUTOSAR compliance, or a completed AUTOSAR redesign.

## Engineering objectives

- Complete and investigate the existing RFID, LCD, servo, system tick, SD card, and serial-output paths.
- Improve module boundaries and interfaces while preserving the project's overall legacy architecture and system-level intent.
- Use comments, wrappers, and focused code changes to make control flow and peripheral behavior easier to review.
- Experiment with self-written MCAL modules against the existing board support code.
- Track memory, error handling, integration, and verification observations to inform the later architecture transition.
- Apply V-model thinking by linking requirements, implementation changes, and verification evidence.

CAN integration and the AUTOSAR-oriented redesign remain future goals. They should be recorded as completed only after implementation and verification in the appropriate project stage.

## Scope and change discipline

The scope includes firmware source edits, module-level restructuring, completion of unfinished legacy features, and relevant build, linker, integration, and project documentation updates. Changes should preserve the overall legacy architecture while allowing implementation details and module APIs to improve. Generated outputs, local data, and editor state remain outside version control; project configuration is tracked when needed to understand or verify the baseline.

Each release uses a `Vx.y — topic` commit title. `version-history.md` is the committed record of what changed from the previous version; the workspace `README.md` remains local and is not committed.

## Portfolio context

The repository can support a CV or interview discussion about analyzing, improving, and completing a legacy embedded project, then using evidence from that work to guide a future architecture transition. Describe only changes that have actually been implemented and verified.
