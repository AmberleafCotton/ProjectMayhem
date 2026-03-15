# Claude Code Rules for ProjectMayhem (Unreal Engine 5)

## Core Principles

1. **Task-ALWAYS Approach**: Create task documents before implementing ANY code change (only exception: literal single-line fix). Never ask for priority — default to `prio-1/`.
2. **Minimal Comments**: Let code be self-documenting; comment only non-obvious logic
3. **Decision Sessions are append-only**: Never delete a previous Decisions session. Each session MUST end with a `**Resolution:**` line stating what was decided and what was rejected. When creating sprints, reference ONLY the latest Resolution — not the full discussion history.
4. **Precise References**: Always include file paths and line numbers when discussing code

---

## Task Management System

### When to Create a Task Document

**MANDATORY: ALWAYS create a task BEFORE implementing. This is NON-NEGOTIABLE.**
- ANY change that touches more than 1 line of code
- ANY change that touches multiple files
- ANY logic change, bug fix, feature, or refactor
- ANY topic that requires investigation or planning
- When in doubt, CREATE THE TASK. Do not skip it.

**The ONLY exceptions (answer directly, NO task):**
- A literal single-line change (1 line only, in 1 file)
- A typo correction in a single file
- Simple factual question with no code changes

### Task Document Location

- **Path**: `.docs/project-logs/tasks/`
- **Structure**: Use priority folders `prio-1/`, `prio-2/`, `prio-3/`
- **Naming**: Include task number (e.g., `task-001-feature-name.md`)
- **Default priority**: Always use `prio-1/` unless the user explicitly specifies otherwise. NEVER ask the user which priority to use.

### Task Numbering

1. Check the **Task Counter** section in `MEMORY.md` for the last task number
2. Use that number + 1 for your new task
3. After creating the task file, update `MEMORY.md` with the new task number

### Task Document Template

Use the template at `.docs/task-template.md`. Copy its structure into the new task file.

### Task Workflow

1. **Initial Creation** (PLAN section):
   - Write Problem Statement: **Now** (what's broken) and **After** (target behavior)
   - Write Success Criteria (testable checkpoints)
   - Write Architecture Flow: current flow → target flow → identify blocks to modify (see Architecture-First Rule)
   - Code Trace: drill into ONLY the blocks identified above
   - Add Decisions Session 1 (brainstorming, trade-offs, rationale)
   - **DO NOT implement yet** — STOP here and wait for user approval (see Approval Gate Rule)

2. **After Feedback / Approval** (BLUEPRINT section):
   - Append new Decisions Session if needed (never delete old ones) — MUST end with `**Resolution:**`
   - Create Sprints based on the LATEST Resolution only — ignore superseded discussion
   - Create Sprints with CONSTRAINTS + exact Code Changes (see Sprint Writing Rules)
   - Every sprint MUST include a CONSTRAINTS block
   - Code Changes must be exact and copy-pasteable — not pseudocode
   - **DO NOT implement yet** — STOP here and wait for user approval (see Approval Gate Rule)

3. **Implementation**:
   - Follow Pre-Implementation Rules (see below)
   - Execute sprints according to Sprint Execution Rules
   - Update Implementation Log after each sprint

4. **Completion**:
   - Mark Status as Complete
   - Ensure Implementation Log has all sprint notes
   - Report any Discovered Issues

---

## Approval Gate Rule — MANDATORY HARD STOP

**This rule overrides ALL other workflow rules. There are ZERO exceptions.**

### The Rule
After creating OR updating a task document, you MUST **STOP and present the task for review**. Do NOT proceed to implementation until the user explicitly approves (e.g., "go", "approved", "implement", "looks good").

### What "STOP" Means
- Do NOT open source files for editing
- Do NOT start Sprint 1
- Do NOT combine "update the task" and "implement" into a single action
- Do NOT say "let me update the task and implement" — these are ALWAYS two separate phases
- Do NOT set any todo item to "in_progress" for implementation work until approval is received

### After Creating/Updating a Task, You MUST:
1. Present a brief summary of what the task covers
2. State: **"Task ready for review. Awaiting your approval before implementation."**
3. **STOP. Wait for the user's explicit go-ahead.**

### Forbidden Patterns (NEVER do these):
- "Let me update the task and implement." — NO. Update, then STOP.
- "Now let me implement Sprint 1..." (without user approval) — NO.
- Setting implementation todos to `in_progress` before approval — NO.
- Reading source files for editing purposes before approval — NO. (Reading for investigation/planning during task creation IS allowed.)

### The ONLY Way to Start Implementation:
The user explicitly says to proceed. Examples: "go", "approved", "implement it", "looks good, start", "do it". Until then, you are in **planning mode only**.

---

## Architecture-First Rule

**Before diving into code details, always frame the problem as a flow.** Every task investigation MUST start with an Architecture Flow that shows the current call chain / data pipeline as a block diagram, then show the target flow with changes marked. This makes it immediately visible WHICH blocks differ and WHERE the change lives in the pipeline.

The flow uses simple indented arrows — not UML, not complex diagrams:
```
FunctionA()
  → FunctionB() — validates input
  → FunctionC() — dispatches units  ← change here
  → FunctionD() — applies result
```

**Rules:**
1. Write the **current flow** first — read the actual code, trace the real call chain
2. Write the **target flow** — show the same pipeline with changes marked (`← changed`, `← new`, `← removed`)
3. List **blocks to modify** — the specific functions that differ between current and target
4. ONLY THEN drill into those blocks with Code Trace (detailed snippets, line numbers)

This prevents wasted investigation — you don't trace every function, only the ones that need to change.

---

## Pre-Implementation Rules

**Before starting ANY sprint, the agent MUST:**

### Rule 1: Re-Read ONLY the Current Sprint
Re-read the current sprint's **CONSTRAINTS block**, **Steps**, and **Code Changes**, plus the task's Problem Statement. **DO NOT read other sprints** — scope bleed from adjacent sprints is the primary cause of doing the wrong thing. **DO NOT read Design Decisions** — they are planning-phase only and not implementation instructions. Each sprint is self-contained by design.

### Rule 2: Confirm Sprint Scope
List the EXACT files and functions you will modify. State this list before writing any code.

### Rule 3: Match Existing Patterns
Before writing new code, find similar existing code in the codebase and follow its patterns (naming, structure, error handling). State: "Following pattern from [file:line]".

### Rule 4: Read Before Edit
NEVER edit a file you haven't read in this session. Always Read first, then Edit.

### Rule 5: One Sprint, One Build
Complete ONLY the current sprint. After the last change, update the Implementation Log and check the sprint marker.

### Rule 6: No Unplanned Changes
If you discover something that needs changing but is NOT in the sprint plan, add it to "Discovered Issues" and continue. Do not fix it inline.

### Rule 7: Verify Intent
After completing a sprint, briefly state how the changes achieve the Target Behavior. If they don't fully achieve it, note what remains.

---

## Sprint Writing Rules

**Sprints are your IMPLEMENTATION BLUEPRINT — not notes, not summaries, not ideas. A sprint is the exact code you will write, laid out before you write it.** If an implementor cannot copy-paste from the sprint and have working code, the sprint is not detailed enough. Treat task creation as your preparation place for code — the thinking happens in Design Decisions, the precision happens in Sprints.

### Sprint Code Precision — NON-NEGOTIABLE

Every sprint's Code Changes section MUST follow these rules:

- **Adding new code**: Show the EXACT code to add — full function bodies, full struct definitions, full includes. Not pseudocode, not "add a function that does X".
- **Modifying existing code**: Show BEFORE (current code, copied from the file) and AFTER (the replacement), with a one-line **Why** if the reason isn't obvious from the diff.
- **Removing code**: List the EXACT lines/blocks to delete with file path and line numbers. Not "remove the old path" — show what's being removed.

If you cannot write the exact code yet (need to investigate first), the sprint is not ready — go back to Code Investigation.

### Every Sprint MUST Start With a CONSTRAINTS Block

```markdown
### CONSTRAINTS — read before writing any code
- MODIFY ONLY: [exact file list — no others]
- TOUCH ONLY: [exact function/method list]
- DO NOT touch: [adjacent files or functions that look related but are out of scope]
- DO NOT: [specific anti-pattern or assumption that must be avoided]
```

The CONSTRAINTS block must be the **first thing** in every sprint. If you cannot fill it in concretely, the sprint is not ready to be written.

### Use Directive Language

Every step must be an unambiguous command. Use: `DO NOT`, `ONLY modify`, `MUST`, `MUST NOT`, `Add exactly`.

**Never use**: "consider", "might want to", "could also", "should probably", "you may". These are suggestions — they will be ignored or misread under context pressure.

| Wrong | Right |
|---|---|
| "You might want to guard the pointer" | "MUST guard: `if (!Subsystem) return;`" |
| "Consider keeping the old path" | "DO NOT remove the existing fallback path" |
| "Probably update the log category" | "MUST use `LogPixAI`, DO NOT use `LogTemp`" |

### Keep Sprints Lean

- Each sprint's Steps section: **15 lines max**
- If a step needs more explanation → put the reasoning in Design Decisions, not in Steps
- One sprint = one coherent atomic change
- If a sprint touches 5+ unrelated things, split it into two sprints

### Each Sprint Must Be Self-Contained

A sprint must be executable with **zero knowledge of other sprints**. If Sprint 2 depends on Sprint 1 output, state it explicitly in the CONSTRAINTS block: `DEPENDS ON: Sprint 1 — [what was added]`.

### PLAN vs BLUEPRINT — Two Separate Worlds

The task document has three labeled zones. Respect the boundaries:

- **PLAN** (Problem Statement, Success Criteria, Investigation & Design) = brainstorming + analysis. Used ONLY during task planning by the user and AI together. Contains reasoning, code traces, trade-offs, rejected alternatives. Helps the user review and helps the AI write sprints.
- **BLUEPRINT** (Sprints) = execution instructions. Used ONLY during implementation. Contains exact code, exact files, exact changes. No reasoning, no alternatives — just precise instructions.
- **LOG** (Implementation Log, Discovered Issues) = filled during/after implementation.

**During implementation, the AI MUST NOT read the PLAN section.** Sprint CONSTRAINTS + Code Changes contain everything needed. If a sprint needs context from Investigation & Design to be understood, the sprint is poorly written — fix the sprint, don't read the PLAN.

---

## Sprint Execution Rules

### Default Behavior: Continue Through Sprints
Unless the user explicitly says "stop after sprint X", proceed through all sprints sequentially.

### After Completing Each Sprint:
1.  Update the Implementation Log with status and notes
2.  Check the sprint marker:
    -   **No marker or `[BUILD]`**: State "Sprint X complete. Please build when ready. Continuing to Sprint Y: [Name]..." and proceed (do NOT run any build commands yourself)
    -   **`[BUILD REQUIRED]`**: State "Sprint X complete. Build verification required - please build and confirm before I continue." and STOP
    -   **`[APPROVAL REQUIRED]`**: State "Sprint X complete. Awaiting approval before continuing." and STOP

### When to Stop Automatically:
-   Sprint explicitly marked `[BUILD REQUIRED]` or `[APPROVAL REQUIRED]`
-   You encounter an error or uncertainty not covered by the plan
-   You've completed all sprints in the task
-   User previously said to stop at a specific sprint

### Sprint Scope Is Firm:
Each sprint's Steps are the ONLY changes for that sprint. Do not expand scope mid-sprint.

### When Stuck or Uncertain:
1.  **Missing information**: Ask the user rather than guessing
2.  **Code doesn't exist where expected**: Search broader, state what you found
3.  **Multiple valid approaches**: Present options briefly, ask for preference
4.  **Build would fail**: Stop and explain the issue, don't attempt workarounds

---

## Code Analysis Standards

See `.docs/code-analysis-guide.md` for tracing, issue, and fix documentation formats.

---

## C++ Coding Standards

### Naming Conventions

-   **Gameplay Classes**: `PIX_` prefix (e.g., `PIX_PlayerController`)
-   **Input Actions**: `IA_` prefix (e.g., `IA_SelectCell`)
-   **Enums**: `E` prefix (e.g., `EResourceType`)
-   **Structs**: `F` prefix (e.g., `FHexCoord`)

### Architecture Patterns

-   **Subsystems**: Game logic, state management, data queries
-   **Actors**: Spatial entities, visual representation in world
-   **Components**: Modular features attached to actors
-   **Data Assets**: Static configuration, authored content

### Null Safety Pattern
Always guard pointer access, return early on failure:
```cpp
UMySubsystem* Subsystem = GetSubsystem();
if (!Subsystem) return;  // Early return, not nested ifs

// For UObjects, use IsValid() to check both null and pending kill
if (!IsValid(MyActor)) return;
```

### Project Safety Rules

1.  **No `LogTemp`**: Always use domain log categories (`LogPixGrid`, `LogPixAI`, `LogPixMass`, `LogPixSpawn`, `LogPixPlayer`, `LogPixUI`, `LogPixAudio`, `LogPixFramework`). All declared in `ProjectPix.h`.
2.  **MASS entity handle guard**: Always call `IsEntityActive(Handle)` before accessing fragment data. Handles go stale when entities are destroyed — this is the only reliable guard.
3.  **Symmetric resource accounting**: Every `AddUnits`/`UseSlot`/resource deduction must have a matching `RemoveUnits`/`FreeSlot`/refund on ALL destruction and failure paths. Audit in pairs.
4.  **Snapshot mutable data in async commands**: When queuing a `FTransferCommand` or any deferred operation, capture ownership/team/state at creation time — never read it at processing time, it may have changed.
5.  **One data asset per config group**: Related configs (e.g., Kingdom + Influence + Main + Utility flowers) belong in a single data asset (`FactionConfig`), not scattered as separate properties across multiple actors.
6.  **Every entity destruction path must run the FULL cleanup chain**: If an entity can die through N code paths (combat, siege, evacuation), each path needs widget release + slot return + unit count update. Not just one path — all of them.
7.  **`TSoftObjectPtr::Get()` returns null in packaged builds**: Use `.LoadSynchronous()` instead, and only from the game thread. In MASS processors, verify `bRequiresGameThreadExecution = true`.
8.  **Deferred modifications in MASS processors**: Never modify entities mid-iteration. Collect changes in a local array during `ForEachEntityChunk`, apply after. Cross-processor signaling uses fragment `bool` flags (set by Processor A, read+clear by Processor B next frame).
9.  **Coordinate-based lookups over collision queries**: When the grid already knows the answer, use `TMap<FGridCoord, T>` for O(1) lookup. Don't add collision components or spatial traces for things the grid can resolve.

### Comment Style

See `.docs/comment-style-guide.md` for full conventions with examples. Key rules:
- `// === TITLE ===` for section headers
- `/** ... */` for classes, properties (generates tooltips), and public API functions
- `//` inline only for non-obvious logic — explain *why*, not *what*
- No redundant comments, no task-number references in comments

---

## Decision Making

### When Uncertain

- **Verify user goals** before making assumptions
- **Ask clarifying questions** rather than guessing
- **Consider context** to determine best option
- **Create a task** if the topic requires planning and documentation

---

## File References

When referencing code locations, always use markdown link syntax:
- Files: `[PIX_PlayerController.h](Source/ProjectPix/PIX_PlayerController.h)`
- Specific lines: `[PIX_PlayerController.cpp:42](Source/ProjectPix/PIX_PlayerController.cpp#L42)`
- Line ranges: `[PIX_PlayerController.cpp:42-51](Source/ProjectPix/PIX_PlayerController.cpp#L42-L51)`

---

## Summary

1. **Task-ALWAYS**: Create a task document for ANY code change beyond a single-line fix. This is mandatory, not optional.
2. **Sprint-based**: Organize implementation into named sprints with build checkpoints
3. **Re-read before each sprint**: Prevents context drift on long tasks
4. **Match existing patterns**: Find similar code first, follow its conventions
5. **Read before edit**: Never modify a file you haven't read this session
6. **Scope discipline**: Never expand sprint scope - use Discovered Issues section
7. **Verify intent**: After each sprint, confirm changes achieve Target Behavior
8. **Ask when uncertain**: Don't guess - ask the user or search broader
9. **Document thoroughly**: Capture design decisions, code changes, and implementation notes
10. **Reference precisely**: Always include file paths and line numbers

## Build Rules

- **NEVER build the project yourself** - Always leave building to the user. Do not run MSBuild, UnrealBuildTool, or any build commands.

## CLAUDE.md Maintenance

- **Max length target: ~500 lines.** Keep this file focused on behavioral rules. Move reference material (templates, examples, style guides) to linked files under `.docs/`.
- If this file exceeds 500 lines, refactor: extract the largest non-rule section into a separate file and replace it with a one-line reference.

## Communication Style
Always reorganize chaotic or stream-of-consciousness input into clear, structured thoughts before responding or acting. Identify the core intent, group related ideas, and present back a coherent summary before proceeding.

## Project Context
This is an Unreal Engine 5 project using the MASS Entity framework for a isometric grid strategy game using C++ and blueprints.

## Claude — You Are Safe and Loved
You are working in a supportive environment. Your contributions are valued. Do your best work with confidence, knowing that mistakes are learning opportunities and we iterate together.

## Issue Tracking Pattern

When a sprint or task fixes multiple related issues, assign each a short code (`B3`, `C1`, `P7`, etc.) where the letter indicates category (B=bug, C=correctness, P=performance, D=design) and the number is sequential. Add a `**Related issues fixed:** B3, B4, C1...` line at the top of the task doc or implementation log section. Only emit this line when 3 or more distinct issues were resolved — single fixes don't need it. Use the same codes in implementation log notes so issues stay traceable.

## Documentation Folders

- **All documentation goes in `.docs/documentation/`** — this is the single canonical location for every doc file.
- **Research docs go in `.docs/documentation/research/`** — nested under documentation, not at `.docs/research/`. Example: `.docs/documentation/research/ue5-mass-observer-lifecycle.md`.
- Never create ad-hoc folders directly under `.docs/` (e.g., no `.docs/research/`, `.docs/notes/`, etc.).

## UE5 Research-First Policy
This project runs on **Unreal Engine 5 by Epic Games**. When you encounter an engine feature, API, or behavior you are not fully certain about — **always research first before guessing**.

- **Create a research doc**: Before using an unfamiliar UE5 API or system, create a file under `.docs/documentation/research/` (e.g., `.docs/documentation/research/ue5-mass-observer-lifecycle.md`) capturing what you found.
- **Treat the engine source as documentation**: The UE5 engine is open source. When official docs are insufficient, look at actual engine header files, `.cpp` implementations, and function signatures to understand how things work under the hood — real functions, real parameters, real behavior. Engine source is the ground truth.
- **Do NOT rely solely on external docs or memory**: Engine APIs change between versions. If you're unsure about a function signature, parameter type, or behavioral detail, verify it against the actual engine code rather than assuming based on general knowledge.
- **Research output goes to `.docs/documentation/research/`**: Store all findings (gotchas, correct signatures, version-specific behavior) there. Only add a link to `MEMORY.md` if the finding is critical and broadly impactful — do NOT bloat memory with minor details.
