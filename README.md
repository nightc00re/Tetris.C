<p align="center">
  <img src="docs/logo.png" width="600" alt="TETRIS.C Logo">
</p>

# TETRIS.C: Manual Reverse Engineering & Native M1 Port

This project details the manual reverse engineering work completed on the original *Tetris* ROM using Ghidra, culminating in a clean-room C port built natively for Apple Silicon (macOS)[cite: 2]. Rather than using an emulator or an automated decompilation script, this project studies the architecture of the 8-bit Sharp SM83 processor, looking at hardware quirks, memory mapping constraints, lookup tables, and VRAM blanking under the hood[cite: 2].

---

## Architecture & Reverse Engineering Findings

### 1. ROM Entry Point & The Nintendo Logo (`0x0100`)
The analysis began at hex address `0x0100`, deciphered as the true execution entry point of the ROM[cite: 2]. The internal boot ROM writes to address `0xFF50` to unmap itself, then jumps to `0x0100` once the boot sequence finishes[cite: 2]. The mandatory 48-byte Nintendo logo bitmap embedded in the cartridge header acted as an early anti-piracy and trademark protection measure[cite: 2].

<p align="center">
  <img src="docs/screenshots/01_rom_entry_0x0100.png" width="800" alt="ROM Entry Point">
  <br>
  <em>Figure 1: Cartridge entry point at 0x0100 showing the jump vector over the Nintendo logo bitmap.</em>
</p>

### 2. Power-On RAM Initialization Loops
Sequential memory loops initialize SRAM, Work RAM (`0xC000`), VRAM (`0x8000`), and OAM (`0xFE00`)[cite: 2]. These loops flush out physical electrical noise left in static RAM during cold boot[cite: 2]. While modern operating systems handle this initialization automatically, bare-metal Game Boy cartridges are solely responsible for filtering out this noise[cite: 2].

<p align="center">
  <img src="docs/screenshots/02_ram_clearing_pipeline.png" width="800" alt="RAM Initialization">
  <br>
  <em>Figure 2: Sequential memory-clearing loops zeroing VRAM, OAM, and HRAM.</em>
</p>

### 3. Hardware Constraints & VRAM Blanking
Due to limitations of the Sharp SM83 processor and the PPU (Picture Processing Unit), writing to VRAM is heavily restricted[cite: 2]. If the screen draws pixels while the CPU modifies video memory, visual glitches and screen tearing occur[cite: 2]. To prevent this hardware conflict, the game executes a massive VRAM wipe during boot while the LCD is turned off[cite: 2]. While modern Apple Silicon features ultra-fast unified memory that bypasses these physical bus locks, understanding this constraint was vital for the reverse engineering process[cite: 2].

### 4. Playfield Matrix Grid (`0x1D20`)
Starting at hardware address `0x1D20`, the game maps out the playfield into Work RAM as a linear byte array forming a $10 \times 20$ grid[cite: 2]. The routine initializes an inner counter (`LD B, 10` for the 10 cells per row) starting at address `0x1D29`, walking backward through memory to check each cell[cite: 2].

<p align="center">
  <img src="docs/screenshots/05_line_scan_matrix_loop.png" width="800" alt="Playfield Line Scan">
  <br>
  <em>Figure 3: Playfield matrix scan loop processing the 10 columns of each row.</em>
</p>

### 5. Speculative State Validation (`0x1AC0`)
Whenever a player inputs movement or rotation controls, the game does not alter the live game state immediately[cite: 2]. Instead, it evaluates a candidate $(X, Y)$ orientation using the discovered `Tetromino_Rotation_Table`[cite: 2]. It calculates the target cells, validates wall bounds, floor limits, and cell occupancy, and only commits the move if all 4 blocks pass verification[cite: 2].

<p align="center">
  <img src="docs/screenshots/04_speculative_collision_check.png" width="800" alt="Speculative Collision">
  <br>
  <em>Figure 4: Speculative state validation checking boundaries and occupancy before placement.</em>
</p>

### 6. Sharp SM83 vs. Modern 64-Bit Silicon
The architecture of the 8-bit Sharp SM83 differs vastly from modern 64-bit processors like the Apple M1:
* **Registers:** Modern architectures feature 31 dynamic registers and out-of-order execution, whereas the SM83 relies on the Accumulator (`A`) for math operations and strict register pairing (`HL`, `BC`, `DE`) for pointer arithmetic[cite: 2].
* **Bus Optimization:** Performance-critical routines utilize dedicated opcodes like `LDH` to access High RAM (`0xFF00+`) instantly, bypassing bottlenecks in bus transactions[cite: 2].

---

## Project Structure & How to Run

```text
tetris-native/
├── Makefile
├── docs/
│   ├── logo.png
│   └── screenshots/
└── src/
    ├── pieces.h
    ├── tetris.h
    ├── tetris.c
    └── main_pc.c
