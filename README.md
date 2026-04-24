# Underground Fight Club – CS-UY 3113

**Author:** Johann Varghese  
**Course:** CS-UY 3113  
**Assignment:** Final Project  
**Date Due:** 04/25/2026

---

## Description

UG Fightclub is a top-down rap battle adventure game built using Raylib in C++.

You play as an up-and-coming rapper trying to make a name for yourself at an underground club. Talk to NPCs, win a gambling game, get your drip, battle Offset, survive a dark maze, and face the final boss — Playboi Carti.

---

## Controls

| Key | Action |
|-----|--------|
| A / D / W / S | Move (top-down) |
| ENTER | Talk to NPCs / Advance dialogue |
| Arrow Keys | Navigate Tic Tac Toe |
| A / D | Move left/right (battle scenes) |
| W | Jump (battle scenes) |
| SPACE | Attack (battle scenes) |
| 1 | Debug: jump to LevelA (Club) |
| 2 | Debug: jump to LevelB (Maze) |
| 3 | Debug: jump to LevelC (Offset Battle) |
| 4 | Debug: jump to LevelD (Carti Battle) |

---

## Levels

| Scene | Description |
|-------|-------------|
| Menu | Title screen with instructions |
| LevelA | Underground club — talk to NPCs, gamble, get your drip |
| LevelB | Dark cave maze — darkness shader, find Carti |
| LevelC | Side-view battle vs Offset — jump over projectiles |
| LevelD | Final battle vs Playboi Carti — dodge bats and WAEH attacks |

---

## Features Implemented

- 4 fully playable levels + title screen as separate Scene objects
- Top-down WASD movement with tile-based collision (LevelA, LevelB)
- Side-scrolling battle system with jump, gravity, and projectiles (LevelC, LevelD)
- Full NPC dialogue system with quest chain (6-step quest in LevelA)
- Tic Tac Toe minigame (rigged — player always wins)
- Player cosmetic change mid-game (drip upgrade)
- Darkness/torch shader in maze (inverse-square attenuation — Requirement 5)
- 3 types of enemy projectiles: normal, high (stand still), player shots
- Animated sprite sheets (7x7, 5x5 grids) with 2D frame cropping
- Background music per level (4 unique tracks)
- Sound effects: Migo vocals on Offset attacks, Carti vocals on Carti attacks
- HP bar and cash counter HUD
- Camera follows player with smooth lerp (top-down levels)
- Fixed camera for battle scenes
- Fixed timestep physics loop

---

## How to Run

```bash
make
./raylib_app
```
