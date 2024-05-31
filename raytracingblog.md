# Balls and More: Experiments With Ray Tracers


## Yapping and background
Last semester I worked with Neural Radiance Fields for my project with Prof. Sundaresan Raman (as it happens with most SOPs at BITS, the project didn't really go anywhere and I should be getting an A regardless :) )

I treated the graphics rendering part of the NeRF pipeline as a blackbox and the neural architecture itself as just a function approximator for an abstract plenoptic function (link).

Until some time ago when I came across Peter Shirley's (link) Ray Tracing Weekend Series and made a rudimentary ray tracer of my own.

## Viewing the image and the PPM format

The first thing we will do is to set up how we are going to view the images. To do this, we will write values to a file in the PPM (Portable Pixmap) format. To have a taster write the following in a .ppm file.

```
P3
3 2
255
255 0 0 0 255 0 0 0 255
255 255 0 255 255 255 0 0 0