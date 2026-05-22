# ♟️ 2D Graphical Chess Game (Raylib & C++)

A high-performance, interactive 2D Chess game built from scratch using **C++** and the **Raylib** graphics framework. This project heavily utilizes core Object-Oriented Programming (OOP) architectures, dynamic polymorphism, and state-driven game flows.

## 🚀 Key Features & Implementation Detail
- **Interactive UI (Raylib):** Features fluid, real-time board rendering and responsive mouse-click handling for selecting and shifting pieces.
- **Dynamic Polymorphism:** Leverages an abstract base class `Piece` with virtual overrides (`isValidMove()`, `clone()`, `Draw_piece()`) extended by concrete classes for individual pieces (`Pawn`, `Knight`, `Bishop`, `Rook`, `Queen`, `King`).
- **Advanced Move Verification:** Complex logical paths checking for collision states (`isHorizontalPathClear`, `isVerticalPathClear`, `isDiagonalPathClear`).
- **En Passant & Target Metrics:** Active validation framework embedded into the board layout handling conditional piece updates.
- **In-Game Utilities:** Includes responsive keyboard layout bindings for advanced runtime tools:
  - `S` Key: Saves current board layout to a local data file.
  - `U` Key: Dynamic **Undo** framework to revert previous tactical positions.
  - `R` Key: Dynamic **Redo** framework to re-execute reverted configurations.
  - **Arrow Keys:** Navigates step-by-step through game histories in *Replay Mode*.

## 🛠️ Project Architecture
```text
├── BSAI25013-main.cpp       # Application Entry Point & Core Raylib Initialization Window
├── BSAI25013-Board.h/.cpp   # Central Engine Managing App States (MENU, PLAYING, GAME_OVER, REPLAYING)
├── BSAI25013-Cell.h/.cpp    # Matrix Coordinates (8x8 Grid Squares) with Encapsulated Piece Pointers
├── BSAI25013-Piece.h/.cpp   # Polymorphic Abstract Base Class for Common Movement/Color Vectors
├── BSAI25013-Helper.h       # Structural Framework for Layout Records, Turn Toggles, and State Enums
└── BSAI25013-[PieceName]    # Isolated Subclasses containing Strict Rules for Piece Logic
