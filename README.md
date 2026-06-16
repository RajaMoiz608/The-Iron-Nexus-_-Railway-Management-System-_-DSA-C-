# 🚂 The Iron Nexus — Railway Management System

A console-based Railway Management System built in **C++** that demonstrates core data structures through a real-world simulation. Each module of the system is powered by a different data structure, implemented from scratch without the STL containers.

---

## 📁 Project Structure

```
IronNexus/
├── Train.cpp           # Main program — all modules, menus, and logic
├── Classes.h           # Stack, ActionStack, PriorityQueue (Min-Heap)
├── ColoredTerminal.h   # ANSI color codes and formatted print utilities
└── README.md
```

---

## 🧠 Data Structures Used

| Module | Feature | Data Structure |
|--------|---------|----------------|
| 1 | Train Registry | AVL Tree (self-balancing BST) |
| 2 | Coach Management | Doubly Linked List (dynamic memory) |
| 3 | Rail Network | Weighted Graph + Dijkstra's Algorithm |
| 4 | Seat Booking | Hash Table + BST |
| 5 | Operation Logging | Stack (array-based) |
| Bonus | Undo / Redo | Dual Action Stacks |
| Internal | Shortest Path | Min-Heap Priority Queue |

---

## ⚙️ Modules

### Module 1 — Train Registry (AVL Tree)
Trains are stored in a self-balancing AVL Tree keyed by Train ID. The tree auto-balances on every insert and delete to guarantee O(log n) search.

- Register and decommission trains
- Search by Train ID
- Three traversal modes: In-Order (by ID), Pre-Order (top-down), Post-Order (bottom-up)
- View total trains and tree height

### Module 2 — Coach Management (Doubly Linked List)
Each train carries a doubly linked list of coaches allocated with `new`/`delete` for true dynamic memory management.

- Add coaches at front, at end, or after a specific coach ID
- Remove coaches by ID
- Forward and backward traversal
- Reverse the entire train (for return journeys)

### Module 3 — Rail Network (Weighted Graph + Dijkstra)
Stations and tracks form a weighted undirected graph stored as an adjacency matrix.

- Add and remove stations and tracks
- Display the full network with distances
- Find the shortest path between any two stations using **Dijkstra's Algorithm** powered by a custom Min-Heap Priority Queue

### Module 4 — Seat Booking (Hash Table + BST)
Seats are stored in a hash table for O(1) average-case lookup. A BST maintains sorted order for display.

- Initialize a configurable number of seats
- Book seats by seat number and passenger name
- Cancel bookings
- Check individual seat status
- Display booked seats, available seats, and all seats in sorted order

### Module 5 — Operation Log (Stack)
Every state-changing operation across all modules is pushed onto a stack-based log.

- View the full operation history (most recent first)
- Peek and pop individual entries
- Save the log to `log.txt` and reload it across sessions

### Bonus — Undo / Redo (Dual Action Stacks)
Every undoable action is tracked with a serialized `Action` struct pushed onto an undo stack. Undone actions move to a redo stack.

- Undo the last performed action (add/remove coach, book/cancel seat, register/decommission train, add/remove station or track)
- Redo the last undone action
- Redo stack clears automatically when a new action is performed

---

## 💾 Save & Load

All modules support persistent file storage. Data is serialized to plain text files and reloaded on demand, preserving the full system state across sessions.

---

## 🖥️ How to Compile & Run

**Requirements:** A C++17 compiler (g++ recommended)

```bash
g++ -std=c++17 -o iron_nexus Train.cpp
./iron_nexus
```

On Windows (MinGW):
```bash
g++ -std=c++17 -o iron_nexus.exe Train.cpp
iron_nexus.exe
```

> **Note:** The terminal must support ANSI escape codes for colored output. On Windows, use Windows Terminal or enable Virtual Terminal Processing.

---

## 🗺️ Menu Navigation

```
THE IRON NEXUS  |  Main Menu
  1. Train Registry
  2. Coach Management
  3. Rail Network
  4. Seat Booking
  5. Operation Log
  6. Undo / Redo
  7. Save & Load
  0. Exit
```

Each option opens its own sub-menu. Enter `0` at any sub-menu to return to the main menu.

---

## 🔧 Implementation Notes

- All data structures are implemented **from scratch** — no `std::stack`, `std::queue`, `std::map`, or `std::priority_queue`
- `CoachList` uses raw `new`/`delete` for dynamic memory; the destructor walks the list and frees all nodes
- The AVL Tree performs rotations (LL, RR, LR, RL) on every insert and delete
- Dijkstra's shortest path runs on a custom array-based Min-Heap, not the STL
- The `ColoredTerminal.h` header provides a consistent styled UI using ANSI codes throughout

---

## 📋 Requirements

- C++17 or later
- ANSI-compatible terminal (Linux, macOS, Windows Terminal)
