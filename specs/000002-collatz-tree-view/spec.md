# Specification: Collatz Tree View in the GUI

Feature ID: 000002-collatz-tree-view
Project: collatz-vibecoding-melhorado
Date: 2026-10-12

## Summary
The GUI currently only prints text sequences. The author wants the window to draw the inverted
Collatz tree of the entered numbers, exactly in spirit of the supplied reference `arvore-collatz.png`:
the start values sit at the top, every path descends through Collatz steps, paths that reach a common
value share one node, and everything converges to a single trunk ending at `1` at the bottom. The
drawing must use only wxWidgets' own facilities so the project stays portable across Linux, macOS and
Windows.

## Scope
In scope:
- A pure computation that builds the fused Collatz graph (a DAG) for a set of start values.
- A wxWidgets panel that renders that graph as the inverted tree described above.
- Reuse of the existing input field and Generate action; navigation adequate to read large trees.
- Unit tests for the graph construction.

Out of scope:
- The `collatz` CLI's standard-input behaviour (acknowledged as an unrelated, deferred issue).
- Animation, record search, 3D rendering, and arbitrary-precision arithmetic.
- Any direct native drawing call (GTK, Win32/GDI, Cocoa); only wxWidgets APIs are permitted.

## Requirements
- R1: For the current series of start values the GUI draws an inverted tree: start values toward the
  top, each path descending one Collatz step per level, all paths converging to a single `1` at the
  bottom.
- R2: When two or more paths visit the same value they share one node (fusion); each distinct value
  is drawn exactly once.
- R3: Every node is labelled with its numeric value; every edge links a value to its Collatz
  successor and spans exactly one level.
- R4: A node's level is its distance to `1` in Collatz steps, so fused values align on the same row,
  `1` sits at the bottom, and edges never skip or cross levels.
- R5: Rendering uses only wxWidgets APIs (a `wxPanel`/`wxScrolledWindow`, `wxDC`/`wxGraphicsContext`,
  `wxFont`) with no direct native toolkit call, and must compile on Linux, macOS and Windows.
- R6: The view auto-fits the tree to the window and supports zoom and pan so large trees stay
  readable; pressing Generate replaces the previous tree.
- R7: A node that triggers the 64-bit overflow guard or the step cap is drawn with a distinct marker
  and its path stops there (no wrapped value is drawn).
- R8: Start values are visually distinguished from intermediate nodes so the inputs remain
  identifiable even when they do not all share the top row.
- R9: Invalid tokens are reported and excluded; the tree is built from valid values only.
- R10: The graph construction is a pure function in `collatz_core`, independent of wxWidgets, and
  covered by tests.

## Edge Cases and Failure Modes
- EC1: One start value -> one chain from that value down to `1`.
- EC2: `1` as the only input -> a single node labelled `1`.
- EC3: Duplicate inputs -> no duplicate nodes or chains.
- EC4: Empty or blank input -> an on-canvas hint; no tree is drawn and the previous tree is cleared.
- EC5: An invalid token -> reported in the window, the other valid values are still drawn.
- EC6: An overflow or truncated node -> distinct marker; unaffected paths still draw fully.
- EC7: A tree taller or wider than the window -> auto-fit on first draw, then scroll/zoom with no
  clipped content and no unreadable overlap.
- EC8: A very large union of paths -> node count bounded by the per-path step cap; layout remains
  responsive.

## Interface and Data Impact
- API: new pure types in the core library (`collatz::CollatzTree`, `collatz::TreeNode`,
  `collatz::TreeEdge`) and a builder `collatz::build_tree(starts, options)`; no wxWidgets dependency.
- Database: None; nothing is persisted.
- Inter-service: None; single self-contained process.
- GUI: the multiline text output is replaced by a drawing panel; the input control and the Generate
  button remain, and a short text summary stays available.
- CLI: unchanged.

## Success Criteria
- AC1: Entering the eight reference values reproduces the sample structure: start labels at the top,
  shared nodes where paths meet, a single trunk descending to `1` at the bottom.
- AC2: Each distinct value is drawn once and every node shows its value.
- AC3: The GUI sources include only `wx/...` headers; a search finds no GTK, Win32 or Cocoa include.
- AC4: Input `{2}` yields the two-node chain `2 -> 1`.
- AC5: Input `{1}` yields a single node labelled `1`.
- AC6: A node whose `3n+1` would overflow is marked and does not wrap.
- AC7: Catch2 tests for graph construction (fusion, level assignment, de-duplication, start flags,
  overflow) pass with 0 failures.
- AC8: The GUI builds, launches and draws on Linux; no platform-specific code is required.

## Cross-references
- None. Single-repository change; follow-up to feature `000001-collatz-project-scaffold`.

## Open Questions
- I cannot read image input, so `arvore-collatz.png` was not inspected visually; it was analysed at
  the pixel level (523x679, 8-bit grayscale) and read as an inverted, fused tree. Please confirm the
  intended layout: are the start values meant to occupy one common top row, or to sit at their true
  distance-to-`1` height (my R4 recommends the latter, with R8 highlighting the inputs)?
- Navigation: is wheel-zoom plus drag-pan wanted, or only auto-fit and scrollbars?
- Styling: monochrome like the sample, or themed colour (for example parity-coloured edges)?
- Should the numeric text listing remain beside the drawing, or be dropped entirely?
- Session context baseline (`B = 48,000 bytes`) is recorded in feature 000001's `spec.md`.
