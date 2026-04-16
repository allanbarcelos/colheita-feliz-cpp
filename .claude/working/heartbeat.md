---
type: working-thread
topic: "Session heartbeat (crash-recovery snapshot)"
last_updated: 2026-04-15 14:24:14
status: active
task_status: in-progress
source: prompt-submit-hook
scope: repo
pid: 1431
fingerprint: 8c5183dc3fba40db
---

> **Auto-heartbeat** (fingerprint-gated, fires on user prompt AND after file writes).
> Updates whenever project state changes. Fast-path exits when nothing moves.
> `task_status: in-progress` — if this file exists on SessionStart, the previous
> session did NOT exit cleanly (no Stop or PreCompact fired). Use "Recent user prompts"
> below + git diff to resume the task.

## Recent user prompts (last 3 — literal task wording)

```
 create mode 100644 src/Desenho.h
 create mode 100644 src/Iso.h
 create mode 100644 src/Tipos.h
```

## Git / Project State (last change)

Branch: `main`

### Changed files
```
 M src/Constantes.h
 M src/Crops.h
 M src/Tipos.h
 M src/main.cpp
?? compile_commands.json
```

### Diff stat
```
 src/Constantes.h |  3 ++
 src/Crops.h      | 53 ++++++++++++++++++++++++++----------
 src/Tipos.h      |  2 ++
 src/main.cpp     | 83 ++++++++++++++++++++++++++++++++++++++++++++++++++++----
 4 files changed, 121 insertions(+), 20 deletions(-)
```

## Recent working files in this dir

- Bastos/ColheitaFeliz/.claude/working/heartbeat.md
