Path Tracer: Nights of the Final Project II - The Significant Parts
======================

**University of Pennsylvania, CIS 561: Advanced Computer Graphics, Final Project
Part II**


Name: Meghana Seshadri 

PennKey: msesh


**NOTE: Please turn OFF BVH tree construction prior to rendering!**


Renders 
------------
1. M 

100 per pixel
Recursion depth: 5
512x512 pixels
Render time: 4 hours
Photon mapping (25000 photons)

![](./renders/M_8samp_15sampbottom_25000photons_19404022render.png) 

2. Cornell Box 

64 samples per pixel
Recursion depth: 5
1024x1024 pixels
Render time: 3 hours
Photon Mapping (25000 photons)

![](./renders/potentials/photonmapsubmission/cornel_8samp_25000photons.png) 

3. Cornell Box 2 Lights

25 samples per pixel
Recursion depth: 5
512x512 pixels
Render time: 1 hour
Photon mapping (100000 photons)

![](./renders/potentials/photonmapsubmission/cornel2lights_5samp_100000photons.png) 



Overview
------------
In addition to the "basic" features required by part I of the final project, you
must implement one of three complex rendering engines as an addition to your
path tracer. They are all worth 150 points, but they are listed in ascending
order of approximate difficulty. Choose the feature you feel most comfortable
attempting!

Useful Reading
---------
The following PBRT chapters are relevant to the features listed for this
assignment:
* Chapter 15: Volume Rendering
* Chapter 16: Bidirectional Methods

Submitting final renders (15 points)
-------------
The deadline for code completion and submission is Monday, April 24 at midnight.
However, you may take up until Tuesday, April 25 at midnight to submit final
renders from your path tracer. On Wednesday, April 26 we will run through a
slide show of everyone's renders, allocating at most 2 minutes per student.
Please provide any captions you deem important to your renders, including
listing the features each render is showcasing. __We expect at least one of your
rendered images to be demo-reel worthy, meaning a minimum of 1024x1024 pixels
and a high level of convergence (at least 20x20 samples per pixel) along with
measured render times.__ If you wish, you may discuss your renders and features
during your 2 minutes.

Possible features (135 points)
--------------
Choose one of the following renderer components to implement in your path tracer
engine. As stated above, they are listed in ascending order of approximate
difficulty:
* #### Photon Mapper
Implement a basic photon mapper that stores a set of global illumination
photons and a set of caustic photons. Rather than storing a set of direct
lighting photons, your renderer will simply evaluate the direct lighting at a
point using multiple importance sampling as in your path tracer. If you wish,
you may try to make your renderer more efficient by also including a radiance
photon map, which stores the average of a set of surrounding photons. Finally,
you will have to incorporate a volumetric data structure (e.g. a voxel grid or
kd-tree) in order to better store your photons and evaluate where they land.
* #### Path Tracing with Participating Media (Volumetric Rendering)
Implement an integrator in your path tracer capable of handling participating
media. We require that it handle _at least_ homogeneous media, but we encourage
you to implement heterogeneous media as well. For interesting renders, try
incorporating transmissive spheres to focus light beams through the medium.
* #### Bidirectional Path Tracer
Implement an integrator that performs bidirectional path tracing. This type of
path tracing is especially effective in scenes where the light sources are
highly occluded, such as a scene where a light is enclosed by a transmissive
object. In these scenarios, the direct lighting computation at each path vertex
will almost never produce light energy, making the scene illuminated almost
entirely by global illumination. By tracing rays from both the light sources and
the camera, we ensure that our ray paths eventually reach the light source,
even without a direct lighting pass. For your bidirectional engine, we only
require naive integration; incorporating multiple importance sampling into such
a renderer is actually more complicated than one might expect.

Submitting your project
--------------
Along with your project code, make sure that you fill out this `README.md` file
with your name and PennKey, along with your test renders.

Rather than uploading a zip file to Canvas, you will simply submit a link to
the committed version of your code you wish us to grade. If you click on the
__Commits__ tab of your repository on Github, you will be brought to a list of
commits you've made. Simply click on the one you wish for us to grade, then copy
and paste the URL of the page into the Canvas submission form.
