# Version History

## Repository scope

This workspace is intended to complete and improve the legacy RFID project baseline through documented source changes, module-level restructuring, and project integration work. Changes may include code edits, defect fixes, peripheral and SD card completion, and build/configuration updates. The work will preserve the project's overall legacy architecture and its original system-level design intent so that its limitations and trade-offs can be evaluated before a future transition to an AUTOSAR-oriented architecture. This scope does not claim AUTOSAR compliance or a completed AUTOSAR redesign.

## V1.0 — Legacy Project baseline (scope clarification)

- Clarifies that the baseline is an active engineering workspace, not a read-only source review.
- Allows code changes and module restructuring while retaining the overall legacy architecture for comparison with a later AUTOSAR transition.
- Keeps generated build outputs, local data, and editor state out of the repository.

## V1.1 — Legacy baseline code and module update (candidate)

- Splits RFID card handling into helper steps for card detection, payload reading, validation, and mode updates; introduces named constants for buffer and block sizes and terminates the read payload explicitly.
- Reworks SD card CSV handling around bounded static record/text storage, explicit status-returning parsing helpers, sample CSV generation, and file/line error checks.
- Adds distinct LCD and USB-TTL display APIs and updates the application flow to initialize USB-TTL; LCD, RFID, and SD card initialization calls remain commented out in `main.c` in this revision.
- Revises barrier and servo control states, separates movement updates, and assigns the SD card chip-select to PA3 while RC522 retains PA4.
- Makes RFID user record strings read-only and reorganizes make dependencies under `Object/deps`, with build progress messages and a report of untracked new files.
- Updates project overview and build/linker notes for the expanded baseline scope.

This V1.1 entry describes the current local candidate. Verify it before publishing the commit. Future releases should use a `Vx.y — topic` commit title and add a concise, evidence-based entry here.
