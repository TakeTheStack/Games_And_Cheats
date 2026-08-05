# Games_And_Cheats
Differents games and cheats that I made with C ; i made cheate by reversing and disassemble the object file with Ghidra (or another tools).

## First Game: SNAKE (work only on linux, but soon i will adapt it to windows or others OS)
I made a snake in C and I used the polya methodology for the pre-development of this game.

### Cheat 1: Adding a big Score each time the apple was eaten by the snake
I disassemble the object file with Ghidra and patch the instruction corresponding to the score increment.

### Cheat 2: No-clip (under development)
I want to be able to get behind the game's wall without triggering a "Game Over".

## Second Game: PACMAN (work only on linux, but soon i will adapt it to windows or others OS)

### Cheat 1: Invincible pacman life
I disassemble the object file with Ghidra and patch the instruction corresponding to the life variable and life decrement, which make pacman immortal and invincible.
