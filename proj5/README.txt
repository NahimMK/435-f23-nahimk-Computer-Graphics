CMSC 435 Computer Graphics
Boids Fishtank Simulation
Nahim Kamruzzaman
11.25.23

A program that writes an output file that performs a simple crowd boid simulation
inside a fish tank. THE SIMULATION DOES NOT INCLUDE FOOD, ONLY FISH
ATTEMPTED TO USE KDTREE BUT COULDN'T FIGURE IT OUT
ATTEMPT IS IN COMMENTS, PARTIAL CREDIT?


For Project 5, I used the following sources:

Notes from in class and the textbook that explains boids simulation and Reynolds suggestion.

https://medium.com/fragmentblog/simulating-flocking-with-the-boids-algorithm-92aef51b9e00 
and
https://en.wikipedia.org/wiki/Boids -
For more information on boids algorithm.

To run this program:
-Download the project 5 directory. 
-Do a make clean.
-Go to the directory and run make.
-Then run something like

./fishtank sample.in sample.out

This will produce a sample.out which you can view in viewer. My equation for the fish
velocity, force, and position may be off resulting in slower fish but collision detection
seems to work well. Movement is slow but smooth and there are 600 frames in 20 seconds.


THIS PROJECT WAS SUBMITTED AT 1:30 AM 11-25-2023