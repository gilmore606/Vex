# Vex

Vex is a fantasy console from an alternate history 1980s where vector graphics remained supreme.  It includes a COBOL-esque programming language (vex)
which compiles to a VM bytecode which drives a virtual vector-based graphics chip (and a virtual FM synthesis audio chip).  The project goal is "to 
compile Asteroids in less than 200 lines", or more generally, to provide a platform for very easy and rapid development of vector arcade games.

The compiler is currently being written in Kotlin.  The VM is being written in C++ with GLFW.

DISCLAIMER FOR CONTRIBUTORS: I don't actually know C++!  My C++ code is terrible and probably insults the compiler.  Please help me learn better C++!

You can [download an .exe demo](https://raw.githubusercontent.com/gilmore606/Vex/master/vextest.zip) if you want to see it running yourself.

![screenshot](https://raw.githubusercontent.com/gilmore606/Vex/master/vexlogo.png)

# Updates

9/5: Lots of compiler and VM work -- math functions, optimized bytecode, conditionals, input handlers, global variables, and lots more little things I forgot.

8/30: Compiler: added the concept of system functions and system object classes, and function/method call parsing and compiling to go with.  We'll need this to
meaningfully interact with the GPU and APU.

8/28: CPU: the compiler now supports 'if' and 'repeat', which permitted the first benchmark to run -- the VM can execute about 17 million opcodes/second.  Here's a sample
disassembler output for some benchmark code:

	   0:  OP_CONST c0 [1000]
	   3:  OP_SETVAR v3 [_loop886371]
	   6:  OP_JUMPZ v3 [_loop886371] j3 [64]
	j0 11:  OP_CONST c1 [6]
	   14:  OP_CONST c2 [-4]
	   17:  OP_ADDI
	   18:  OP_SETVAR v0 [foo]
	   21:  OP_CONST c3 [V(1.0,-1.0)]
	   24:  OP_SETVAR v1 [x]
	   27:  OP_CONST c4 [2000]
	   30:  OP_SETVAR v2 [_loop371507]
	   33:  OP_JUMPZ v2 [_loop371507] j2 [56]
	j1 38:  OP_VAR v1 [x]
	   41:  OP_CONST c5 [0.9]
	   44:  OP_MULTVF
	   45:  OP_SETVAR v1 [x]
	   48:  OP_DECVAR v2 [_loop371507]
	   51:  OP_JUMPNZ v2 [_loop371507] j1 [38]
	j2 56:  OP_DECVAR v3 [_loop886371]
	   59:  OP_JUMPNZ v3 [_loop886371] j0 [11]
	j3 64:  OP_EXIT


8/25: CPU: we ran our first compiled program today (on start: sound 1)!  The VM bytecode machine exists and the compiler's basic pieces are in place.

8/23: APU: the synth is fully fleshed out, with 2 oscillators, a low/high/bandpass filter, and an LFO + ADSR modulator per voice.  Sound effects are made in the ROM as very short songs -- the shoot song, the explode song, etc.  For fun and ease of auditioning patches I added MIDI input.

8/19: Particles!  Particles are just fire-and-forget sprites that can have acceleration, color shift, scale change, and rotation.

8/18: Collision detection!  Rather than do traditional mathy detection, I just drew all the lines on a buffer using the sprite IDs, and checked for hits on draw.

8/17: GPU: we can render >10,000 lines (or >1,000 sprites) at 60fps with pretty good vector simulation.  APU: we can play 8+ voice MIDI files with 2-osc ADSR synths that kinda sound like a POKEY chip.

8/16: Proportional fonts and sprites.

8/14: The speculative code below partially compiles (to an AST)!  The GPU and APU exist and mostly work!  The binary .vexo format exists and can be written and (partly) read!


# Sample code

Speculative asteroids.vexc :

	settings:
		STARTING_LIVES = 3
		TURN_SPEED = 0.04
		SHOT_SPEED = 0.3
		THRUST = 0.03

	screen attract:

	on draw:

	on startButton:
		goto play

	screen play:

	state:
		intensity (float start 1)
		rocks (listof rock max 50):
			sprite (from SPRITES)
			xy (position)
			rot (rotation)
			rotSpeed (fraction always random(0.4))
			vec (vector)
			size (number)
		player:
			sprite (from SPRITES.ship)
			xy (position start SCREEN_CENTER)
			rot (rotation start 0)
			vec (vector start (0,0))
			alive (state start false)
			lives (number start STARTING_LIVES)
		shots (listof shot max 3):
			sprite (always SPRITES.shot)
			xy (position)
			vec (vector)
		score (number wrap)

	on start:
		SPAWN player
		repeat 4:
			make rock(SPRITES.rockBig, position(random(1.0),random(1.0)), 0, vector(random(1.0), 0.04), 3)

	on update:
		if player is alive:
			if input.left: player.rot by TURN_SPEED
			if input.right: player.rot down by TURN_SPEED
			if input.thrust:
				player.vec += vector(player.rot, THRUST)
				SOUNDS.thrust
		player:
			xy by vec
		each rock:
			rot by rotSpeed
			xy by vec
			if rock hits player and player is alive:  // collisions are written on render
				KILL rock
				KILL player
		each shot:
			xy by vec
			each rock:
				if shot hits rock:
					score += (200 - rock.size * 50)
					KILL rock
					remove shot

	on fire button: 
		if (shots < MAX_SHOTS)):
			make shot(player.xy, vector(player.rot, SHOT_SPEED))
			SOUNDS.shoot

	on hyperspace button:
		SOUNDS.hyperspace
		player.xy = vector(random(1.0), random(1.0))

	to SPAWN player:
		reset xy, rot, vec, sprite
		alive = true

	to KILL rock:
		remove rock
		SOUNDS.killRock
		if size > 1: repeat 2:
			let newsprite = size == 2 ? SPRITES.rockSmall : SPRITES.rockMed
			make rock(newsprite, xy, 0, vec + vector(random(1.0), 0.04), size - 1)

	to KILL player:
		SOUNDS.killPlayer
		sprite = SPRITES.explodingShip
		alive = false
		--lives
		each shot: remove shot
		in 3 seconds:
			if lives > 0: SPAWN player
			else: goto attract

	on draw:
		player:
			if lives < 1: draw text 'GAME OVER' at SCREEN_CENTER
			repeat lives: draw SPRITES.1up at position(0.8+repeat*0.03, 0.1)
			draw at xy,rot
			if alive and input.fire:
				draw SPRITES.exhaust at xy,rot
		each rock: draw at xy,rot
		each shot: draw at xy
		draw text score at position(0.45,0.01)


