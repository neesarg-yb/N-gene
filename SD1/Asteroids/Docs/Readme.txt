Known Issues:
- One asteroid doesn't show up sometimes (1 out of 10 times).
	* I believe it is because possible asteroid-spawn outside the screen with zero velocity.
- SetOrtho() of Renderer needs to be fixed.
- Need to move some .hpp/.cpp files from Engine/ to its subfolders

How to Use:
- It is an Asteroids game similar to the demo game provided by Squirrel.
- It works with Keyboard and Xbox Controller.
	* Keyboard: 
		E,S,F or UP,LEFT,RIGHT keys for movement
		SPACE to shoot bullets
		I,O to create and delete asteroids
		N to spawn new Ship
		T to enter slow-motion mode
		P to pause the game
	* Xbox Controller:
		Left_Joystick for movement
		Button_A to shoot
- Juice implemented (1 total)
	* Player have 3 lives; and if he dies 3 times, game will restart after 5 seconds.
