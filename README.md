# Assembler and Emulator for Simple Assembly
 A two pass assembler and an emulator for assembly code written in simple assembly implemented in C++.

# Instruction Set
This document provides a reference for the instruction set used in the emulator.

## Opcode Table

| Mnemonic | Opcode | Operand | Formal Specification | Description |
|----------|--------|---------|----------------------|-------------|
| data     |        | value   |                      | Reserve a memory location, initialized to the value specified |
| ldc      | 0      | value   | B := A; A := value;  | Load accumulator with the value specified |
| adc      | 1      | value   | A := A + value;      | Add the value specified to the accumulator |
| ldl      | 2      | offset  | B := A; A := memory[SP + offset]; | Load local |
| stl      | 3      | offset  | memory[SP + offset] := A; A := B; | Store local |
| ldnl     | 4      | offset  | A := memory[A + offset]; | Load non-local |
| stnl     | 5      | offset  | memory[A + offset] := B; | Store non-local |
| add      | 6      |         | A := B + A;         | Addition |
| sub      | 7      |         | A := B - A;         | Subtraction |
| shl      | 8      |         | A := B << A;        | Shift left |
| shr      | 9      |         | A := B >> A;        | Shift right |
| adj      | 10     | value   | SP := SP + value;   | Adjust SP |
| a2sp     | 11     |         | SP := A; A := B     | Transfer A to SP; |
| sp2a     | 12     |         | B := A; A := SP;    | Transfer SP to A |
| call     | 13     | offset  | B := A; A := PC; PC := PC + offset; | Call procedure |
| return   | 14     |         | PC := A; A := B;    | Return from procedure |
| brz      | 15     | offset  | if A == 0 then PC := PC + offset; | If accumulator is zero, branch to specified offset |
| brlz     | 16     | offset  | if A < 0 then PC := PC + offset; | If accumulator is less than zero, branch to specified offset |
| br       | 17     | offset  | PC := PC + offset;  | Branch to specified offset |
| HALT     | 18     |         |                      | Stop the emulator. This is not a 'real' instruction, but needed to tell your emulator when to finish. |
| SET      |        | value   |                      | Set the label on this line to the specified value (rather than the PC). This is an optional extension, for which additional marks are available. |

## Usage
This table provides an overview of the operations available in the emulator's instruction set. Each instruction consists of a mnemonic, opcode, operand, and a formal specification that describes its functionality.


