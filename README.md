# IMPORTANT

This repo is no longer active, instead I have moved to gitlab. Therefore I will no longer be updating this repository.

To get the latest changes, go to: https://gitlab.com/Protofall/CG-Project-2018

-----------------------------------------

# OpenGL Project

These are the controls for this openGL program

Controls (Keyboard/Mouse mode):
- Use the mouse to look around (When cursor is focused on the window), just move your cursor in the direction you want to look in (Every time it moves, the cursor is automatically reset to the center of the window, so if you want to get your cursor back then you need to Either end the program with esc or Alt + Tab out and make sure not to hover over the program)
- Use WASD to move forward, left, back and right respectively
- Press G to swap between keyboard/mouse and XBOX 360 controls
- Press ESC to end the program

Controls (XBOX 360 controller):
- Use the left thumbstick to move around
- Use the right thumbstick to look around
- Press the Start Button to swap between XBOX 360 and keyboard/mouse controls
- Press the Back button to end the program

Shared controls (Same for keyboard and controller):
- Press Y to reset the player's position back to the start
- Press X to fire the blue cannon (Shooting it is just for show, you can't "shoot" anything. This action also has a sound effect)

Note that mouse/keyboard controls are locked when in XBOX 360 mode and vica versa. However Pressing G or ESC on keyboard or Back or Start on the XBOX 360 controller always works. Note that the controller support might not work on your system if you don't have the latest drivers

The config file is automatically loaded in. If no file is found or the variables are invalid then it will default to the maximum screen resolution


### Possible packages needed to run this

sudo apt-get install libglfw3-dev libglfw3
sudo apt-get install libglew-dev
sudo apt-get install libglm-dev

sudo apt-get install freeglut3
sudo apt-get install freeglut3-dev
sudo apt-get install build-essential

// Not sure if all of these are needed or not. Might only need the first 3 commands

I have no idea if the makefile even works, I only ever successfully got it working on Ubuntu 16.04 LTS and some Fedora distribution. I think "glew" was the culprit


### Credits

A few resources were given to us by our lecturer such as the toilet and spaceship models. I found the following resources somewhere online
- Skybox (However the moon was editted by Airofoil)
- Megabuster model + sound effect
- Minecraft old hurt sound
- Pokemon wall bump sound
- other???

Everything in the "myTextures" folder was either entirely made by me, or a recreation of someone else's work (The factory block)

As for license on this thing, idk you're free to use any of the stuff I made, but its not made that well
