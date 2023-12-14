CMSC 435 Computer Graphics
Seam Carving
Nahim Kamruzzaman
12.14.23

A program that takes an image and based on user input calculates the number of seams
that need to be removed and removes them and creates a coherent image.


For Project 6, I used the following sources:

Notes from in class and the textbook that explains seam carving as well as how to 
do energy calculations.

https://en.wikipedia.org/wiki/Seam_carving - For more information on seam carving.

https://www.youtube.com/watch?v=6NcIJXTlugc - Seam carving

To run this program:
-Download the project 6 directory. 
-Do a make clean.
-Go to the directory and run make.
-Then run something like

./seamcarve dog.jpg output_dog.jpg 500 400

The format is 

./seamcarve input_image output_image output_width output_height

This will produce a output image which you can view in an image viewer.

There are problems with the vertical carves where the image has only
5 seams that repeat and it doesn't look cohesive. Tried my best to redo it
using my horizontal carving algorithm but the problem was not resolved.


THIS PROJECT WAS SUBMITTED AT 12:35 AM 12-14-23