# Games_And_Cheats
Differents games and cheats that I made with C ; i made cheats by reversing and disassemble the object file mostly with Ghidra (you can use another reversing tools too).

Allowed: You can hack this game with contribution, in order to improve this game or just for the fun, if you want too ;)
Not-allowed: You are not allowed each of these games into malware AND/OR running them on an electronic device that you do not own.

Advice: Have fun and take your time, when you hacking this game on your computer ;)

## First Game: SNAKE (work only on linux, but soon i will adapt it to windows or others OS)
I made a snake in C and I used the polya methodology for the pre-development of this game.

### Cheat 1: Adding a big Score each time the apple was eaten by the snake
I disassemble the object file with Ghidra and patch the instruction corresponding to the score increment.

### Cheat 2: No-clip
I disassemble the object file with Ghidra and patch several instructions, to get behind the game's wall without triggering a "Game Over".

## Second Game: PACMAN (work only on linux, but soon i will adapt it to windows or others OS)

### Cheat 1: Invincible pacman life
I disassemble the object file with Ghidra and patch the instruction corresponding to the life variable and life decrement, which make pacman immortal and invincible.

### Cheat 2: Infinite time score
I disassemble the object file with Ghidra and patch several instructions with NOP (do nothing), on ​​the if conditional of touching a coin/pellets with pacman, which increases the score every time.

## Third Game: SPACE INVADER (work only on linux, but soon i will adapt it to windows or others OS)

### Cheat 1: Shoot
I disassemble the object file with Ghidra and patch several instructions with NOP (do nothing), so i can make impossible for the enemies to shoot the player, 
also to give the possible for the player to get through all the protections.

### cheat 2: Instant Win
I disassemble the object file with Ghidra and patch several instructions, which mean, every time you start the game, you win.

## Fourth Game: FINAL ADVENTURE (Experimental RPG Game)
Please be aware that this game is experimental and not stable.
Be careful, the softwares and the cheats need to be put into the final adventure directory.

### Cheat 1: God like
I disassemble the object file with Ghidra and put max stats (health, mana, etc.), for each class (warrior, mage, ...) in the game. 

### Cheat 2: Soften the target
I disassemble the object file with Ghidra, in order to instantly kill the enemy.
