CMSC 435 Computer Graphics
Rendering Triangles Using Raytracing
Nahim Kamruzzaman
9.23.24
IMPORTANT: PLEASE RUN THE ./raytrace A COUPLE OF TIMES (Like 4 or 5 or 10)

I'm not sure why but sometimes one of the triangle protrudes out or is very big.

This is a program that parses and NFF file and renders triangles
using raytracing and the barycentric intersection method.

For project 1 I used the following sources:

https://cplusplus.com/reference/sstream/istringstream/istringstream/ - to understand how istrngstreams
work for parsing.

Notes from in class and the textbook to create the generate rays and intersect functions
using the barycentric intersection method to check if the ray intersect the triangle at each
pixel.

https://eigen.tuxfamily.org/index.php?title=Main_Page - Instruction on how to use Eigen as well
as documentation on the functionalities eigen provides.

To run this program download the project 1 directory. Go to the directory and run make.
Then run ./raytrace to generate the output.ppm file. You can use a ppmviewer to view what the
output.ppn looks like.

YOU MAY HAVE TO DO ./raytrace a couple times as sometimes a triangle block some of the image.


THIS PROJECT WAS SUBMITTED AT 4:56 PM 9-23-2023