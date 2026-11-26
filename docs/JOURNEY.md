# Project journey

The original work happened in an Eclipse CDT workspace, one project folder per attempt, without version control. The history of this repository (May 4–11, 2025) was **reconstructed afterwards** from those folders: one commit per version, dated from the files' last modification time. Only the C sources were kept. IDE metadata, build outputs and the proprietary font were left out.

## v0.1: hand-written 3D projection (May 4)

The first idea was already a planet. Without any 3D library, the prototype places nodes on a sphere with a **Fibonacci spiral** (golden angle), rotates them with X/Y rotation matrices driven by the mouse, and projects them to the screen with a simple perspective division. It is a sketch: it depends on a `graph.h` that was never written.

## v0.2: 2D network and force-directed layout (May 5)

Back to 2D with SDL2 to get the graph itself right: 60 vertices (10 domes, 30 research stations, 20 depots), routes with distance, condition and capacity, a **force-directed layout** (springs on edges, repulsion between nodes), drag and drop of nodes, circles drawn with the midpoint (Bresenham) algorithm, text with SDL_ttf, adjacency matrix and **Dijkstra** from the main dome.

## v0.3: Mars in OpenGL (May 5–6)

The planet comes back, this time with OpenGL through an SDL2 context: Mars is a sphere built from quad strips, nodes sit on its surface, routes are drawn as **great-circle arcs**, and edge weights become **geodesic distances** (`acos(a·b) × R`). This modeling choice (distances measured along the surface, not in a straight line) is kept in every later version.

## v0.4: macOS port (May 8–9)

Porting to macOS on Apple Silicon: OpenGL and GLUT frameworks, `GL_SILENCE_DEPRECATION`, arm64 build. Routes connect every node to its nearest dome, a BFS checks that the domes are connected, and route conditions are drawn at random (50 % functional, 30 % damaged, 20 % destroyed).

## v1.0: missions and interface (May 9–11)

Everything comes together: a stack and matrix utilities, missions 1, 2 and 4, an inspection tour for the bonus, an overlay where the user selects a start and an end node, Dijkstra recomputed on demand with the path highlighted in 3D, TTF text rendered into OpenGL textures, and a 60 FPS frame limiter. This is the version submitted for the course.

## Known limitations of v1.0

These are the starting point of the refactoring:

- The generated graph has 19 vertices and no cycle, while the brief asks for at least 50 vertices and several circuits.
- Missions 1, 2 and 4 are implemented but not called from `main`.
- Mission 1 compares adjacency matrix entries with the `Condition` enum, while the matrix stores `1` for "route exists" and `0` otherwise, so missing routes are treated as functional.
- Mission 4 sorts `remainingEdges` but then iterates over `edges`.
- `Node.id` is never initialized but used as an array index in the inspection solver (undefined behavior).
- Missions 5 (maximum flow) and 6 (coloring) are missing, and the bonus tour is a DFS rather than an Eulerian circuit.

## Next: v2.0

- Adjacency list graph, seeded generator (≥ 50 vertices, with cycles)
- All six missions and the bonus, unit-tested without a window
- Renderer ported from SDL2/OpenGL to **raylib**, with mouse picking and live rerouting
- Continuous integration with sanitizers
