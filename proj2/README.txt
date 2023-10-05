CMSC 435 Computer Graphics
Adding Mirror Reflection and Diffuse and Blinn-Phong Specular Lighting with Shadows
to Rendered Teapot (3D Object) Using Raytracing

Nahim Kamruzzaman
10.4.23

For some reason the bottom of my teapots reflection is see through.
I tried messing around with the shadow bias but I think what it really comes
down to is me missing some shadow rays and intersections.
Partial credit for understanding the problem? Couldn't fix it after a lot of
tries tryign to change the calculations, bias, how I found the nearest intersect, etc.

This is a program that parses an NFF file and renders shapes
using raytracing and the barycentric intersection method (Bargtiel's base code).
It also adds diffuse and Blinn-Phong specular lighting with shadows and
mirror reflections (my additions in the shade and castRay funcitons).

Again I used Adam Bargtiel's base code which parses and renders the scene instead of my project 1
as it said I could in the project description and I made addition to the shade and castRay functions
to add mirror reflections(castRay) and diffuse + Blinn-Phong specular lighting with shadows (shade function)

For project 2 I used the following sources:

https://cplusplus.com/reference/sstream/istringstream/istringstream/ - to understand how istrngstreams
work for parsing.

Notes from in class and the textbook to calulate the colors and the shadow rays, reflection rays,
and how to do the Blinn-Phong lighting and shadows. Also used the calculations in book for the mirror
reflections.

Also used book to learn how normalizing my vectors worked and what the calculations would turn out to be to
check as I ran unit tests.

To run this program download the project 2 directory. Go to the directory and run make.
Then run 

./hide teapot-3.nff output.ppm 

to generate the output.ppm file. You can use a ppmviewer to view what the
output.ppn looks like.


THIS PROJECT WAS SUBMITTED AT 11:50 PM 10-04-2023