This is the start when turning computer on

# Set Up

You don't need to follow in exact order, but it is there for convenience in time:

1. Open up Sublime Text  (it should already be open)
2. Open up GitBash
3. Open up Docker
4. Open up GitHub
5. In Docker and GitBash: Run `cd Documents/Git-Hub/starstruck-convex` (press tab to have it finish for you)
6. In Docker: Run `make windocker` (once Docker is ready)

When you are ready to send information to the robot

* In Docker, run `make`
* In GitBash, run `make flash` 

This will send it over to the robot, unless errors put up on Docker. If they pop up follow what they say.

After you finish Programming

when pushing files into GitHub run the following steps in GitBash:

1. `git add .`
2. `git commit -m "Type what you did today"``
3. `git push origin master`

To see if anything is new before going to GitHub run:
```
Docker Problem and solution
```
1. If docker isn't responding run 'exit' and retry.
2. If it doesn't let you run 'make windocker' run 'exit' and retry
3. If this doesn't work run 'docker-machine stop'

```
git log
```

This will pull up al hi chase  times you have pushed something up and who pushed it up to 

once you are done lo hi chase king at it hit the `q` button


```
make shell
```

Only run (make shell while connected with and A-A cable from computer to Robot.

in gitBash run (make shell)

this should pull up putty.

```
apollo
```

This is in Putty

run (apollo and it will ll pull up the shell)

(this should pull up one of two things 1-ERROR, 2- pull up a shell)

```
If finished
```

At the veary end DO OT click the X button at the top right hand side of the page. DO 1 of the following:

 1 Press (CTRL d) on Docker and GitBash.

 2 Or type (exit) on Docker and GitBash.

This will remove the from the screen.

You are now done Pro ramming for the time being
```
Helpful #'s
```
Mark Garner (cyberbrains) (970) 310-9507

Brother Bennet "(678) 683-9280"

Gary Olaveson "(208) 201-0210"

jasonwgarner@gmail.com
```
Master/MERGING
```
If stuck in mater/MERGING do this:
  1 git merge --abort
  2 git fetch --all
  3 git reset --hard origin/master

To avoid