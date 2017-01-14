/*
	This is the start when turning computer on
		Set Up

		You don't need to follow in exact order, but it is there for convenience in time
		1- Open up Sublime Text  (it should already be open)
		2- Open up GitBash
		3- Open up Docker
		4- Open up GitHub
		5- In Docker and GitBash: Run cd Documents/Git-Hub/starstruck-convex (when typing click tab to have it finish for you)
		6- In Docker: Run make windocker (once Docker is ready)

	When you are ready to send information to the robot
		Run (make) in Docker
			Then run (make flash)
				this should send it over to the robot.

	After you finish Programming

		when pushing files into GitHub run the following steps in GitBash:
			1- git add .
			2- git commit -m "Type what you did today"
			3- git push origin master

		To see if anything is new before going to GitHub run:
			git log
		once you are done looking at it hit the q button

	While connected by an A-A cord (A-A is when its USB to USB)

		In GitBash run:
			make shell (this should pull up putty)
				(once in putty run) apollo (this should pull up one of two things: 1- ERROR, 2- pull up a shell)

	At the veary end DO NOT click the X button at the top right hand side of the page
		Run (CTRL d) on Docker and GitBash.
			This will remove them from the screen.
				You are now done Programming for the time being
*/