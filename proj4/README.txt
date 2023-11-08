CMSC 435 Computer Graphics
Mesh Fairing
Nahim Kamruzzaman
11.07.23

This is a program that reads an object file, performs mesh fairing using Laplacian smoothing,
and outputs an obj file of the smoothed mesh. 



For Project 4, I used the following sources:

Notes from in class and the textbook that explains Laplacian smoothing as well as the fundamental
ideas of mesh fairing.

https://en.wikipedia.org/wiki/Laplacian_smoothing - Equation for Laplacian smoothing

https://eigen.tuxfamily.org/index.php?title=Main_Page - Instruction on how to use Eigen as well
as documentation on the functionalities eigen provides.



To run this program:
-Download the project 4 directory. 
-Go to the directory and run make.
-Then run something like

./smoothing bunny.obj sbunny.obj 1 50 

to use bunny.obj as an input and output sbunny.obj which is the meshed version.
You can use the viewer of your choice to view the images.


THIS PROJECT WAS SUBMITTED AT 11:22 PM 11-07-2023