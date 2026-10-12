# Execution Plan: Collatz Tree View in the GUI

## Objective
Add a pure fused-graph builder to `collatz_core` and a wxWidgets drawing panel that renders the
inverted Collatz tree of the entered numbers, using only portable wxWidgets APIs.

## Proposed Changes
### Affected Files
- [ ] `CMakeLists.txt`: add `tree.cpp` to `collatz_core`, `tree_panel.cpp` to `collatz-gui`, and
      `test_tree.cpp` to the test target.
- [ ] `src/main_gui.cpp`: host the drawing panel, keep the input control and Generate action.
- [ ] `tests/test_collatz.cpp`: unchanged (kept as-is alongside the new tree tests).

### New Files to Create
- [ ] `include/collatz/tree.hpp`: pure graph model (`CollatzTree`, `TreeNode`, `TreeEdge`) and
      `build_tree(starts, options)`.
- [ ] `src/tree.cpp`: fused-graph construction, level assignment, de-duplication, overflow marking.
- [ ] `src/tree_panel.hpp`: `CollatzTreePanel` declaration (wxWidgets drawing panel).
- [ ] `src/tree_panel.cpp`: layout (row by level, horizontal separation), paint via
      `wxGraphicsContext`, zoom and pan.
- [ ] `tests/test_tree.cpp`: Catch2 tests for the graph builder.

## Step-by-Step Implementation Tasklist
Each step names the spec.md identifiers it delivers.

- [ ] **Step 1: Core fused-graph builder (R2, R4, R10, EC1, EC2, EC3, EC6)**
  - [ ] Define `include/collatz/tree.hpp` with node (value, level, is_start, status), edge and tree
        types and a `build_tree` entry point.
  - [ ] Implement `src/tree.cpp`: walk each start to `1`, key nodes by value, merge repeats, record
        `level` as the distance to `1`, flag overflow/truncation, deduplicate inputs.
- [ ] **Step 2: Layout and rendering panel (R1, R3, R5, R6, R8, EC7)**
  - [ ] Declare `CollatzTreePanel`; draw nodes as labelled boxes and edges as straight segments,
        row = `max_level - level`, with a font-metric separation pass per row.
  - [ ] Paint with `wxAutoBufferedPaintDC` + `wxGraphicsContext`, guarded by `wxUSE_GRAPHICS_CONTEXT`
        with a plain `wxDC` fallback; distinct style for start and overflow nodes.
  - [ ] Implement auto-fit on first draw, wheel zoom about the cursor and drag pan.
- [ ] **Step 3: GUI integration and input handling (R6, R9, EC4, EC5)**
  - [ ] Wire the existing input field and Generate action to rebuild the tree; clear on empty input.
  - [ ] Report invalid tokens in the window and still draw the valid ones.
- [ ] **Step 4: Tests (R10, AC7)**
  - [ ] Cover fusion (`{5,10}` shares node `5`), levels (`1 -> 0`, `2 -> 1`, `3 -> 7`), de-duplication,
        start flags and the overflow marker.
  - [ ] Register `test_tree.cpp` in the test executable and confirm `ctest` passes.
- [ ] **Step 5: Verification (AC1 - AC8)**
  - [ ] Build the GUI, launch it, and confirm the reference-tree structure and every success criterion.
  - [ ] Confirm no GTK/Win32/Cocoa include exists in the GUI sources.

## Risk Mitigation & Edge Cases
- **Risk:** Nodes in one row overlap, making labels unreadable -> **Mitigation:** measure each label
  with `wxFont`/`wxDC::GetTextExtent` and run a left-to-right separation pass before centring.
- **Risk:** `wxGraphicsContext` unavailable on a target build -> **Mitigation:** guard with
  `wxUSE_GRAPHICS_CONTEXT` and fall back to `wxDC` drawing that keeps the same geometry.
- **Risk:** Huge unions of paths stall the UI -> **Mitigation:** bound work with the existing step cap,
  draw only visible nodes, and double-buffer the paint.
- **Risk:** Platform font-metric differences change spacing -> **Mitigation:** derive all spacing from
  runtime text extents, never from hard-coded pixel constants.
- **Risk:** The visual intent of the sample image is misread (I cannot view images) -> **Mitigation:**
  the layout decisions are pinned as spec Open Questions for author confirmation before implementation.
