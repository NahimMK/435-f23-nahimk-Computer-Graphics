CMSC 435 Computer Graphics
Creating a rasterization-based renderer
to render Teapot (3D Object)

Nahim Kamruzzaman
10.22.23

After trying literally everything, all I could get to output was the background with that
little bit of white in the top left pixel.

This is a program that parses an NFF file and renders shapes
using Vertex Processing, Clipping, Rasterization, Fragment Processing, and Blending.

I made my own parser which I tester and know works perfectly. I'm confident my individual
functions are also strong but when I get the the fragment processing or rasterizing part
for some reason the colors of my pixels just don't change.
For project 3 I used the following sources:

https://cplusplus.com/reference/sstream/istringstream/istringstream/ - to understand how istrngstreams
work for parsing.

Notes from in class and the textbook for Blinn-Phong shading, clipping, rasterization, fragment
processing, etc.
Also used book to learn how normalizing my vectors worked and what the calculations would turn out to be to
check as I ran unit tests.

To run this program download the project 3 directory. Go to the directory and run make
Then run 

./renderer

to generate the output.ppm file. You can use a ppmviewer to view what the
output.ppm looks like.

Please look at my functions as I think most make sense but I just couldn't get it 
to change the pixels. Parial credit?


THIS PROJECT WAS SUBMITTED AT 12:46 AM 10-22-2023