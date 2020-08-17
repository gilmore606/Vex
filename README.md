# Vex

Vex is a fantasy console from an alternate history 1980s where vector graphics remained supreme.  It includes a COBOL-esque programming language (vex)
which compiles to a VM bytecode which drives a virtual vector-based graphics chip (and a virtual FM synthesis audio chip).  The project goal is "to 
compile Asteroids in less than 200 lines", or more generally, to provide a platform for very easy and rapid development of vector arcade games.

The compiler is currently being written in Kotlin.  The VM is being written in C++ with GLFW.

DISCLAIMER FOR CONTRIBUTORS: I don't actually know C++!  My C++ code is terrible and probably insults the compiler.  Please help me learn better C++!

![screenshot](https://raw.githubusercontent.com/gilmore606/Vex/master/vexlogo.png)

# Updates

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


