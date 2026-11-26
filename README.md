# Mars Supply Network

> 3D logistics simulator for a Mars colony, written in C: graph algorithms on a planet-sized supply network, with a real-time graphical interface.

**Status:** refactoring in progress. The `main` branch currently holds the version submitted for the course (`v1.0-school-submission`, SDL2 + OpenGL). A cleaned-up version with all missions, tests and a **raylib** renderer is under way.

## The scenario

A solar storm has damaged the routes between the domes, research stations and supply depots of a Mars colony. As Chief Logistics Officer, you use graph algorithms to assess the network, reroute supplies and plan drone inspections.

- **Vertices:** domes, research stations, supply depots, placed on the surface of a sphere
- **Edges:** supply routes with a distance (geodesic, along the planet's surface), a condition (functional / damaged / destroyed) and a capacity (drones per hour)

## Missions and algorithms

| Mission | Graph problem | Algorithm | Status |
|---|---|---|---|
| 1. Route assessment | Reachability from the main dome | DFS | v1.0 (to be wired into the UI) |
| 2. Supply zones | Connected components | DFS | v1.0, to be reworked |
| 3. Optimal supply paths | Single-source shortest paths, rerouting | Dijkstra | interactive in 3D |
| 4. Critical route shielding | Minimum spanning tree | Reverse-delete | v1.0, to be fixed |
| 5. Robot fleet allocation | Maximum flow with priorities | Edmonds-Karp | planned |
| 6. Inspection scheduling | Graph coloring | DSatur | planned |
| Bonus. Drone tour | Eulerian path / Chinese postman | Hierholzer | partial (DFS tour) |

## Build and run (current version)

Dependencies: SDL2, SDL2_ttf, OpenGL, GLUT.

```bash
# macOS
brew install sdl2 sdl2_ttf
# Debian / Ubuntu
sudo apt install libsdl2-dev libsdl2-ttf-dev freeglut3-dev libglu1-mesa-dev
```

```bash
make
```

```bash
make run
```

A TrueType font is needed at `data/arial.ttf` (not distributed, see [data/README.md](data/README.md)).

Controls: drag with the mouse to rotate the planet, mouse wheel to zoom, pick a start and an end node in the panel to highlight the shortest path.

## Project history

The project went through five versions in one week, each one tagged in this repository:

| Tag | Stack | Milestone |
|---|---|---|
| `v0.1-sdl2-projection` | SDL2 | Hand-written 3D projection, nodes on a Fibonacci sphere |
| `v0.2-sdl2-force-layout` | SDL2 + SDL_ttf | 2D network of 60 vertices, force-directed layout, Dijkstra |
| `v0.3-opengl-planet` | SDL2 + OpenGL + GLUT | Mars as a 3D sphere, great-circle routes, geodesic distances |
| `v0.4-macos-port` | same | macOS (arm64) port |
| `v1.0-school-submission` | same | Missions, interactive shortest path, inspection tour |

Details in [docs/JOURNEY.md](docs/JOURNEY.md).

## Credits

Course project for **Graph Theory and Applications (INF2032)** at ESIEA, spring 2025.

Developed by Coflyd, with the guidance of a private tutor Yves during the original version (some v1.0 files keep their original `Author: yves` header). Refactoring and new features by Coflyd.
