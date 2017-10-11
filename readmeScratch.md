Monte Carlo Path Tracer
======================

**University of Pennsylvania, CIS 561: Advanced Computer Graphics, Semester Project**

Name: Meghana Seshadri 

< Insert an epic render right here> 

## Project Overview 

This path tracer was implemented on a weekly basis as part of a course semester project over the course of 8 weeks. Path tracers implement a realistic lighting algorithm that simulates light bouncing in a scene. Monte Carlo is an integration method that is used to simulate global illumination.

In a scene, with a light source(s) and objects, direct illumination is when light emitted from light sources bounce off of surfaces of objects once before reaching the eye. Global illumination is essentially indirect illumination, where light rays can bounce off a surface multiple times before reaching the eye. Calculating all possible light paths within a scene is a difficult task, which is why Monte Carlo integration is used to effectively estimate all the light coming from all possible directions. It's a statistical method that approximates how much light is redirected towards a point P by other objects in a scene, by casting rays from point P in random directions and evaluating their contributions. 

[Click here for more info on path tracing and monte carlo estimation](https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing)


### Operating Instructions

- Download the code in QT Creator, build, and run. A GUI should appear like the one below:

![](./renders/gui_interface.png) 

Functionality includes the following: 
- `File -> Load Scene`: This will open a file browser where you can locate the .json scene file that you wish to render. Look at the scene_files folder for example scene files. 
- `File -> Render Scene`: This will render the scene displayed in the viewer according to all the settings on the GUI.
- `Samples Count`: This is the square root of the samples you wish to render. So putting in "10" will be 100 samples.
- `Recursion Limit`: This sets the depth level for number of bounces per ray
- `Progressive Preview`: Having this checked will show the image as it gets rendered in the viewer
- `Integrator type`: This drop down chooses which integrator to run with
- `Max Primitives In Node`: This controls how many primitives to have in a node during BVH construction
- `Make BVH`: Toggle this to utilize BVH tree construction

*Note: Please turn OFF BVH tree construction in the GUI prior to rendering with Photon Mapping.*


### Weekly Breakdowns
Take a look below the `Render` section for a weekly implementation breakdown and more information on the various features.


### Features

The following features were implemented:
- 
- 


## Renders 
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



## Weekly Breakdowns

### Week 1: Sampling 

Implement functions in order to generate sample points on the surfaces of objects. Sampling the surfaces of different shapes is very important in a path tracer, some of the reasons being:

- To cast rays in random directions within a hemisphere
- To sample rays to area lights by sampling points on the surfaces of these lights


### Week 2: Naive Path Tracing 

Write functions to generate random ray samples within a hemisphere to compute the lighting a surface intersection receives. Since this is a naive implementation, the images produced will be rather grainy in appearance, but in future weeks will combat this with ray sampling functions. 


The Light Transport Equation
--------------
#### L<sub>o</sub>(p, &#969;<sub>o</sub>) = L<sub>e</sub>(p, &#969;<sub>o</sub>) + &#8747;<sub><sub>S</sub></sub> f(p, &#969;<sub>o</sub>, &#969;<sub>i</sub>) L<sub>i</sub>(p, &#969;<sub>i</sub>) V(p', p) |dot(&#969;<sub>i</sub>, N)| _d_&#969;<sub>i</sub>

* __L<sub>o</sub>__ is the light that exits point _p_ along ray &#969;<sub>o</sub>.
* __L<sub>e</sub>__ is the light inherently emitted by the surface at point _p_
along ray &#969;<sub>o</sub>.
* __&#8747;<sub><sub>S</sub></sub>__ is the integral over the sphere of ray
directions from which light can reach point _p_. &#969;<sub>o</sub> and
&#969;<sub>i</sub> are within this domain. In general, we tend to only care
about one half of this sphere, determining the relevant half based on the
reflectance or transmittance of the BSDF.
* __f__ is the Bidirectional Scattering Distribution Function of the material at
point _p_, which evaluates the proportion of energy received from
&#969;<sub>i</sub> at point _p_ that is reflected along &#969;<sub>o</sub>.
* __L<sub>i</sub>__ is the light energy that reaches point _p_ from the ray
&#969;<sub>i</sub>. This is the recursive term of the LTE.
* __V__ is a simple visibility test that determines if the surface point _p_' from
which &#969;<sub>i</sub> originates is visible to _p_. It returns 1 if there is
no obstruction, and 0 is there is something between _p_ and _p_'. This is really
only included in the LTE when one generates &#969;<sub>i</sub> by randomly
choosing a point of origin in the scene rather than generating a ray and finding
its intersection with the scene.
* The __absolute-value dot product__ term accounts for Lambert's Law of Cosines.



`Shape::ComputeTBN` (5 points)
----------------
We have provided you with implementations of the function `Shape::Intersect`,
which computes the intersection of a ray with some surface, such as a sphere or
square plane. Each of the classes that inherits from `Shape` invokes
`ComputeTBN`, a function that computes the world-space geometric surface normal,
tangent, and bitangent at the given point of intersection. However, in the code
we have provided, this function only computes the world-space geometric surface
normal. For every `ComputeTBN`, you must add code that computes the world-space
tangent and bitangent. Note that unlike transforming a surface normal,
transforming a tangent vector from object space into world space __does not__
use the inverse transpose of the object's model matrix, but rather just its
model matrix. If you consider transforming any plane from one space to another
space, you can see that any vector within the plane (i.e. a tangent vector)
must be transformed by the matrix used to transform the whole plane.

You might be wondering why these additional vectors are needed in the first
place; it's because we'll need them to create matrices that transform the rays
&#969;<sub>o</sub> and &#969;<sub>i</sub> from world space into tangent space
and back when evaluating BSDFs. By tangent space, we mean a coordinate system
in which the surface normal at the point of intersection is treated as (0, 0, 1)
and a tangent and bitangent at the intersection are treated as (1, 0, 0) and
(0, 1, 0) respectively. These matrices are analogous to the orientation
component of a camera's View matrix and its inverse.

`Primitive` Class
-------------
The path tracer base code uses a class called `Primitive` to encapsulate
geometry attributes such as shape, material, and light source. `Primitives` can
intersect with rays, and they can generate `BSDF`s for `Intersection`s based
on the `Material` attached to the `Primitive`. They can also return the light
emitted by the `AreaLight` attached to them, if one exists. `Primitives` hold
the following data:
* A pointer to a `Shape` describing the geometric surface of this `Primitive`
* A pointer to a `Material` describing the light scattering properties of this
`Primitive`
* A pointer to an `AreaLight` describing the energy emitted by the surface
of the `Shape` held by this `Primitive`.

`BSDF` Class (30 points)
----------
In `scene/materials/bsdf.h`, you will find the declaration of a `BSDF` class.
This class represents a generic Bidirectional Scattering Distribution Function,
and is designed to hold a collection of several `BxDF`s, which represent
specific scattering functions. For example, a Lambertian BRDF or a Specular
Transmission BTDF would be defined by classes that inherit from BxDF. When a
BSDF is asked to evaluate itself (i.e. invoke the `f` component of the Light
Transport Equation), it evaluates the `f` of all the BxDFs it contains. You
must implement the following functions for the `BxDF` class:
* `f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags)`
* `Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
  float *pdf, BxDFType type, BxDFType *sampledType)`
* `Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags)`
* The portion of the `BSDF` constructor's initialization list that instantiates
its `worldToTangent` and `tangentToWorld` matrices, which are composed of the
input `Intersection`'s `tangent`, `bitangent`, and `normalGeometric`. The
`worldToTangent` matrix has these vectors as its rows, and the `tangentToWorld`
has them as its columns (in the order listed, so it would be tangent, then
  bitangent, then normal). Remember that the constructor for a GLM matrix uses
  the input vectors as the columns of the resultant matrix.

We have provided comments above each of these functions in `bsdf.h` which
describe what these functions are supposed to do. Please make sure you read
these comments carefully before you begin implementing these functions.

`BxDF` Class (15 points)
-------------
You will find the declaration of the `BxDF` class in `scene/materials/bsdf.h`.
As explained previously, `BxDF` is an abstract class used to evaluate the
result of a specific kind of BSDF, such as Lambertian reflection or specular
transmission. You must implement the `Sample_f` and `Pdf` functions of this
class so that the former randomly generates a ray direction `wi` from a uniform
distribution within the hemisphere and that the latter computes the PDF of this
distribution. Note that `Sample_f` also returns the result of invoking `f` on
the input `wo` and the generated `wi`, but `f` will be defined by specific
subclasses of `BxDF`.

`LambertBRDF` Class (15 points)
--------
In `scene/materials/lambertbrdf.h` you will find the declaration of a class
`LambertBRDF`, which will be used to evaluate the scattering of energy by
perfectly Lambertian surfaces. For this class, you must implement the `f`,
`Sample_f`, and `Pdf` functions. Unlike the generic `BxDF` class, the
`LambertBRDF` class will generate cosine-weighted samples within the hemisphere
in its `Sample_f` and will return the PDF associated with this distribution in
`Pdf`. Most importantly, `f` must return the proportion of energy from `wi` that
is reflected along `wo` for a Lambertian surface. This proportion will include
the energy scattering coefficient of the BRDF, `R`.

`MatteMaterial` Class
---------
We have provided one class that inherits from the abstrace base class
`Material`, `MatteMaterial`. This class describes a material with diffuse
reflectance properties similar to those of, say, matte paint. This class
implements a function called `ProduceBSDF`, which is invoked via the `Primitive`
class during its `Intersect` function to assign a `BSDF` to the `Intersection`
generated by this `Intersect` function. `ProduceBSDF` looks at the texture and
normal maps of the `MatteMaterial` and combines them with the base reflectance
color and `sigma` term of the material to produce either an Oren-Nayar BRDF
(for extra credit if you choose) or a Lambertian BRDF, as described above.

`Integrator` Class
------------
We have provided you with the abstract class `Integrator` in
`integrators/integrator.h`. This class is analogous to the `RenderTask` class
you wrote for your ray tracer. It has a function `Render` which iterates
over all of the pixels in this `Integrator`'s image tile and generates a
collection of samples within a pixel, then ray casts through each and evaluates
the energy that is emitted from the scene back to the camera's pixel. This
function invokes another function `Li` to evaluate this energy, but the base
`Integrator` class provides no implementation.

`NaiveIntegrator` class (25 points)
------------
You will implement the body of the `NaiveIntegrator` class's `Li` function,
which recursively evaluates the energy emitted from the scene along some ray
back to the camera. This function must find the intersection of the input ray
with the scene and evaluate the result of the light transport equation at the
point of intersection. Below is a list of functions you will find useful while
implementing `Li`:
* `Scene::Intersect`
* `Intersection::Le`
* `Intersection::ProduceBSDF`
* `Intersection::SpawnRay`
* `BSDF::Sample_f`

Note that if `Li` is invoked with a depth value of 0 or if the intersection with
the scene hits an object with no `Material` attached, then `Li` should only
evaluate and return the light emitted directly from the intersection.

`DiffuseAreaLight` Class (10 points)
-----------
You will find this class under `scene/lights/diffusearealight.h`, and it
represents an area light that emits its energy equally in all directions from
its surface. For this class, you must implement one function, `L`, which returns
the light emitted from a point on the light's surface along a direction `w`,
which is leaving the surface. This function is invoked in `Intersection::Le`,
which returns the light emitted by a point of intersection on a surface.

----------------------------------------------------------------------------------------------------------------

### Week 3: Direct Lighting 

You will implement two separate components for your path tracer: a new Integrator class to estimate the direct lighting in a scene by sampling points on light surfaces, and a collection of BxDFs to handle specular reflective and transmissive materials. You may implement and test these features in any order you see fit.

We have provided you with updated and new files for your existing path tracer base code. Please copy your existing files into the assignment_package folder, but DO NOT replace any of the files we have provided with this assignment. For some of your files, you may have to add function implementations to various files to make your project compile again. However, just to re-iterate, it is vital that you DO NOT replace any of the files we have given you for this assignment.


MirroredMaterial Class

We have provided you with a MirroredMaterial class that represents perfectly specular reflective surfaces, and will generate a single SpecularBRDF any time it invokes GenerateBSDF. You can load the PT_mirrorBox.json scene file to test your specular BRDF implementation.

Updating BSDF::Sample_f (5 points)

Add code to BSDF::Sample_f that checks if the randomly chosen BxDF is specular, and skips the portions of BSDF::Sample_f that iterate through all BxDFs and computes their fs and Pdfs if the chosen BxDF is specular. This skip check must be done because specular materials have a delta distribution for their PDFs and as such we make certain assumptions about how they are integrated into the light transport equation that should not be made for other materials. We will discuss this more in depth in class.

SpecularBRDF Class (8 points)

You must implement the Sample_f function of the SpecularBRDF class. Since this BRDF is perfectly specular, Sample_f should generate ωi by reflecting ωo about the surface normal. Remember, BxDF::Sample_f is performed in tangent space, so the surface normal is always (0, 0, 1), making reflection about the normal very simple. Note that we have already "implemented" f and Pdf for you; these both return 0. This is because we will assume that ωi has a zero percent chance of being randomly set to the exact mirror of ωo by any other BxDF's Sample_f, hence a PDF of zero.

You can test if your SpecularBRDF implementation works by rendering PT_mirrorBox.json. Your image should look like this, given default sample and recursion settings with the Naive Integrator:


SpecularBTDF Class (12 Points)

Like for the SpecularBRDF class, you must implement the Sample_f function of the SpecularBTDF class. This function is a little more involved, however, as you not only must generate ωi by refracting ωo, but you must also check for total internal reflection and return black if it would occur. Remember, this class only handles transmission, so it should not compute the color that would be returned by total internal reflection if it were to be simulated. You must also make sure to check whether your ray is entering or leaving the object with which it has intersected; you can do this by comparing the direction of ωo to the direction of your normal (remember, you are in tangent space so this is pretty easy). If your ray is leaving a transmissive object, you should compute its index of refraction as etaB / etaA rather than the other way around.

You can test if your SpecularBTDF implementation works by rendering PT_transmitBox.json. Your image should look like this, given default sample and recursion settings with the Naive Integrator:


FresnelDielectric Class (8 points)

In fresnel.h you will find several classes declared, the first of which is simply Fresnel. The other classes, FresnelNoOp, FresnelNoReflect, and FresnelDielectric, all inherit from this abstract class. Each of these classes is designed to compute the Fresnel reflection coefficient at a given point of intersection on a surface. For surfaces that are less physically accurate, such as entirely reflective and entirely transmissive ones, we use the FresnelNoOp and FresnelNoReflect classes to compute the reflection coefficient; they always return a value of 1 and a value of 0 respectively. For more physically accurate materials, such as the provided GlassMaterial, a FresnelDielectric will be used to evaluate the Fresnel reflection coefficient. As such, you must implement the Evaluate function of the FresnelDielectric class so that it correctly computes how reflective a surface point on a dielectric material should be given its indices of refraction and the angle between the incident ray and the surface normal.

You can test if your FresnelDielectric implementation works by rendering PT_glassBallBox.json. Your image should look like this, given default sample and recursion settings with the Naive Integrator:


DirectLightingIntegrator Class (30 points)

You will write another Integrator class's Li function, this time so that it performs light source importance sampling and evaluates the light energy that a given point receives directly from light sources. That means that this Integrator will never recursively call Li. Much of the code in this Integrator is the same as the code you wrote for NaiveIntegrator, but rather than calling a BSDF's Sample_f to generate a ωi, you will randomly select a light source from scene.lights and call its Sample_Li function. Once you have done this, you can evaluate the remaining components of the Light Transport Equation (you already have Li). Sample_Li invokes a few additional functions that you will have to implement; we will detail them below. One final note: you must divide the PDF you obtain from Sample_Li by the number of light sources in your scene to account for the fact that you are choosing a light at random uniformly from the set of all lights in the scene. Mathematically, this just boosts the energy obtained from any one light by a factor of N, where N is the number of lights in the scene.

DiffuseAreaLight Updates (10 points)

You must implement the Sample_Li function we have added to the class declaration of DiffuseAreaLight. This function must perform four actions:

Get an Intersection on the surface of its Shape by invoking shape->Sample.
Check if the resultant PDF is zero or that the reference Intersection and the resultant Intersection are the same point in space, and return black if this is the case.
Set ωi to the normalized vector from the reference Intersection's point to the Shape's intersection point.
Return the light emitted along ωi from our intersection point.
Shape::Sample Implementation (10 points)

The Shape class actually declares two different Sample functions, but the one you must implement in the base Shape class is the one that takes in three inputs rather than two. This function must invoke the two-input Sample, to be implemented by subclasses, and generate a ωi from the resulting Intersection. From there, it must convert the PDF obtained from the other Sample function from a PDF with respect to surface area to a PDF with respect to solid angle at the reference point of intersection. As always, be careful of dividing by zero! Should this occur, you should set your PDF to zero.

Shape Subclass Sample Implementations (12 points)

For the SquarePlane and Disc classes only, you must implement the two-input Sample functions. These Samples must perform the following actions:

Generate a world-space point on the surface of the shape.
Create an Intersection to return.
Set the point and normal of this Intersection to the correct values.
Set the output PDF to the correct value, which would be a uniform PDF with respect to surface area.
Given the requirements above, you must also implement the Area functions of the SquarePlane and Disc classes.


Extra credit (30 points maximum)

In addition to the features listed below, you may choose to implement any feature you can think of as extra credit, provided you propose the idea to the course staff through Piazza first.

Sphere::Sample (8 points)

Implement the Sample function for the Sphere class. This is harder than it may seem at first glance because we want to be as efficient as possible, so we cannot generate samples on the back-side of the sphere from the point of view of the scene intersection point. Instead, we must compute the subtended solid angle that the sphere has with respect to the scene point, and generate a ray within this solid angle, then determine its intersection point with the sphere.

Conductive Materials' Fresnel Reflectance (8 points)

Implement the Evaluate function for a FresnelConductor class to compute the Fresnel reflectance coefficients for metallic surfaces. Unlike the FresnelDielectric class, FresnelConductor evaluates an entire color for its reflectance coefficient rather than a single float.

----------------------------------------------------------------------------------------------------------------

### Week 4: Full Lighting Integrator 

You will once again implement two separate components for your path tracer: a new Integrator class that combines the sampling techniques of your NaiveIntegrator and DirectLightingIntegrator classes, and a set of BxDF classes used to represent glossy surfaces using microfacet distribution models. As in previous assignments, you may implement and test these features in any order you see fit.


DiffuseAreaLight::Pdf_Li (5 points)

Implement the Pdf_Li function declared in diffusearealight.h so that it returns the PDF of the light's Shape given an Intersection from which the light is being viewed and a viewing direction wi.

FullLightingIntegrator Class (40 points)

One of the main components of this homework assignment is to write an integrator that incorporates multiple importance sampling to more effectively render all light paths within a scene. In FullLightingIntegrator::Li, you will generate both a direct lighting energy sample as in your DirectLightingIntegrator and a BSDF energy sample as in your NaiveIntegrator. You will then weight each of these energy samples according to the Power Heuristic and average them together to produce an overall sample color. Note that, as in your NaiveIntegrator, you will recursively call Li when you evaluate the LTE for your BSDF sample.

Balance Heuristic and Power Heuristic (10 points)

At the bottom of directlightingintegrator.cpp, you will find definitions of two functions: BalanceHeuristic and PowerHeuristic. Implement both of these functions so they can be used in your implementation of FullLightingIntegrator::Li. For your final renders, we ask that you provide at least one example of a scene rendered with the balance heuristic and one example of the same scene rendered with the power heuristic. For best render results, we recommend you use the power heuristic for the rest of your images.

MicrofacetBRDF Class (25 points)

Implement the f, Pdf, and Sample_f functions of the MicrofacetBRDF class so they follow the implementation of a Torrance-Sparrow microfacet BRDF. The MirrorMaterial and PlasticMaterial classes make use of this BRDF to create glossy surfaces. Note that this BRDF class makes use of a MicrofacetDistribution pointer for some of the functions mentioned above. We have provided you with the implementation of a Trowbridge-Reitz microfacet distribution for use with your MicrofacetBRDF class functions; as noted in the extra credit section, you may implement additional microfacet distributions if you wish. Refer to microfacet.h for brief explanations of the functions of this class.

Russian Roulette Ray Termination (15 points)

In your FullLightingIntegrator, add a check to Li that terminates recursion based on the Russian Roulette heuristic. This will necessitate adding one more input to Li: a Color3f that tracks the compounded energy of the ray as it travels along its path. If your integrator has performed at least three recursions, then you should compare this compound energy to a uniform random variable and terminate the path if the energy's highest component is less than the random number. We recommend setting your recursion limit to a number much higher than 5 to test your ray termination heuristic.

Extra credit (30 points maximum)

In addition to the features listed below, you may choose to implement any feature you can think of as extra credit, provided you propose the idea to the course staff through Piazza first.

You must provide renders of your implemented features to receive credit for them.

MicrofacetBTDF Class (8 points)

Implement a BTDF that uses a microfacet distribution to generate its sample rays. You will also have to add a new Material class or modify an existing one, e.g. TransmissiveMaterial, to support this BxDF. You will also have to add code to the JSONLoader class in order to load and render a scene that uses this new material. Refer to how we implemented the updated MirrorMaterial for ideas.

Additional MicrofacetDistribution Classes (5 points each)

Implement additional microfacet distribution classes for use in your MicrofacetBRDF class, such as a Blinn distribution, a Phong distribution, or a Beckmann distribution.

Custom scene (5 points)

Now that you have implemented several different BSDFs and have written an efficient global illumination integrator, you should try to create a customized JSON scene of your own to render. We encourage everyone to be creative so you all have unique material for your demo reels. Note that a custom scene that includes one of the extra credit features listed above is a requirement of those features implicitly; you must create an entirely novel scene (i.e. not a variation on the Cornell box) to receive this extra credit.

----------------------------------------------------------------------------------------------------------------


### Week 5: Global Illumination 


Removing your old Li code

To begin, we recommend taking all of the code that you wrote for FullLightingIntegrator::Li in the previous assignment and either commenting it out or moving it somewhere else. You will use some of this code again, but the overall implementation of Li will be significantly altered.

An iterative implementation of FullLightingIntegrator::Li (15 points)

In order to more easily track certain pieces of information, such as the cumulative throughput of your ray path and whether or not your ray just bounced off a specular surface, we are going to implement the FullLightingIntegrator's Li function using a while loop rather than having it invoke itself recursively. To begin, your Li should instantiate an accumulated ray color that begins as black, and an accumulated ray throughput color that begins as white. The throughput will be used to determine when your ray path terminates via the Russian Roulette heuristic. Next, simply declare a while loop that compares some current depth value to 0, assuming that depth began as the maximum depth value. Within this loop, we will add a check that breaks the loop early if the Russian Roulette conditions are satisfied. Just after this while loop, your Li code should return the accumulated ray color, which will be computed within the loop.

Computing the direct lighting component (20 points)

Using almost the same code as you wrote for the previous assignment, use multiple importance sampling to evaluate the direct lighting your ray intersection receives from a randomly chosen light source. This value will be one term; if your BSDF-generated ray does not hit the light source you randomly chose, then its contribution will be black. This term should NOT include any global illumination light, or any light from any other light source. As in your DirectLightingIntegrator, make sure to scale the direct lighting contribution by the number of light sources in your scene; since you are sampling each light source at a frequency inversely proportional to the number of lights in the scene, you must scale each light's contribution upward to counteract its otherwise lessened contribution.

Similarly to your DirectLightingIntegrator, we are going to treat the direct lighting contribution of specular surfaces as black. While the term we'd obtain from the BSDF-based sample would not actually be black, we perform a slightly less costly BSDF contribution computation below, so we're just going to ignore the direct lighting term entirely when it comes to specular materials.

Computing the ray bounce and global illumination (15 points)

Separately from your direct lighting term, you should use an entirely new 2D uniform random variable to generate a new BSDF-based ωi using Sample_f. You will multiply the color obtained from Sample_f with your ray throughput, along with the absolute-dot-product term and the 1/pdf term. This effectively compounds the inherent material colors of all surfaces this ray has bounced from so far, so that when we incorporate the lighting this particular ray bounce receives directly, it is attenuated by all of the surfaces our ray has previously hit. To this end, once you have your updated throughput, multiply it with the direct lighting term from above and add the result to your accumulated ray color. Finally, make sure to update the ray that your while loop is using to originate from your current intersection and travel in the direction of the ωi that you just computed.

Correctly accounting for direct lighting (10 points)

Since your path tracer computes the direct lighting a given intersection receives as its own term, your path tracer must not include too much light. This means that every ray which already computed the direct lighting term should not incorporate the Le term of the light transport equation into its light contribution. In other words, unless a particular ray came directly from the camera or from a perfectly specular surface, Le should be ignored.

Russian Roulette Ray Termination (10 points)

This component should be identical to your implementation from the previous assignment. After you compute the direct lighting and indirect lighting terms, compare the maximum RGB component of your throughput to a uniform random number and stop your while loop if said component is smaller than the random number. Most importantly, to keep your path tracer mathematically correct, you must then divide your throughput by the maximum component of your throughput; this scales up the contribution of this ray path proportionally to how likely the path is to terminate so that we are still accounting for the contribution that paths that were terminated earlier would have had.

Custom scenes (30 points)

Since you are primarily re-implementing code from last week, you will have time available to design and render scenes of your own design. Unlike last week, these scenes are a required component of this assignment. Please create and render at least three unique scenes. They should not be variations on the Cornell box! We want you to have unique material for your demo reel (and some first-hand experience working with the JSON format we have made for scenes). We expect each scene to be rendered with a minimum of 20x20 samples per pixel and with a hard recursion limit of 8 bounces (though your Russian Roulette may stop it earlier). Additionally, each image should be a minimum of 512x512 pixels, but at least one larger image is recommended for better demo reel quality.

We ask that your custom scenes showcase the more visually impressive components of your path tracer, such as a scene that includes a microfacet mirror surface and several glass materials of various indices of refraction. Additionally, texture maps, normal maps, and microfacet roughness maps are encouraged. You may try to render scenes containing triangle meshes, but bear in mind that they will take a while if your meshes have a high number of faces.

----------------------------------------------------------------------------------------------------------------

### Week 6: Acceleration Structures 

Overview

Thus far, we have been rendering scenes with relatively little geometry. However, if we want to be able to produce more interesting images, we'll need to be able to render scenes with large numbers of polygons, e.g. triangle meshes, without taking forever. To that end, we will implement a Bounding Volume Hierarchy to contain all Primitives in the scene so that we can perform ray-object intersection more efficiently.


General Requirements

For this assignment, all we ask is that you implement a BVH acceleration structure. Whether or not you use PBRT's full implementation is up to you, but we encourage you to design the BVH your own way. PBRT's structure is highly optimized, and their code is particularly opaque and difficult to debug, and you will benefit more as a coder if you try to translate the BVH concept into code. All we ask is that your program and BVH fulfill the requirements below.

Bounds3f class functions (20 points)

We have provided you a 3D axis-aligned bounding box class Bounds3f in scene/bounds.h. Please implement the three functions that have been declared at the bottom of the class: Apply, SurfaceArea, and Intersect.

Apply should transform the eight corners of the bounding box by the input Transform's matrix, then compute a new axis-aligned bounding box that encompasses these corners (which may no longer be axis aligned, especially if the Transform contained a rotation). This function sets the invoking Bounds3f to be this bounding box, and returns the resultant bounds.
SurfaceArea, as its name implies, computes and returns the surface area of the invoking Bounds3f. This will be used when you apply the surface area heuristic to choose a place at which to split yourBVH.
Intersect computes the intersection of the input ray with this bounding box. Unlike the Intersect functions of the Shape classes, this does not output an entire Intersection data package, but just outputs the t parameterization that the intersection would have with the ray. This is useful for determining which bounding box is intersected first by a given ray. Additionally, negative t values are valid if and only if the ray's origin lies within the bounding box.
In addition to these functions, we have already provided you with a few utility functions that you may or may not use, such as MaximumExtent, which returns the index of the axis along which the bounding box has the largest side length, or Union, which creates a Bounds3f that encompasses all inputs.

Shape class WorldBound functions (10 points)

We have updated the Shape class to include a WorldBound function, which computes the axis-aligned bounding box of the shape after it has been transformed into world space. Please implement this function for each of the classes which inherit from the Shape class. You will find Bounds3f's Apply function useful when implementing all of the various Shapes' WorldBound functions. You will need to compute the world-space bounding boxes of all your scenes' Primitives in order to construct your BVH.

BVHAccel class (50 points)

We have provided you with a class that represents a bounding volume hierarchy, BVHAccel. Most of the details of implementation are left for you to decide, but you will need to implement at least two of the functions we have provided: BVHAccel's constructor, and its Intersect function. The constructor is where you should build the hierarchy of bounding box nodes; the only detail we require of this construction is that you use the Surface Area Heuristic to determine where to split your geometry. We have provided you with the intermediate structs that PBRT uses in its implementation of its BVH, but we do not require you to use any of them.

A branch of BVH construction should produce a leaf node when there would be at most maxPrimsInNode Primitives within that node, or when the cost of subdividing that node via the surface area heuristic would be greater than the cost of simply testing for intersection against all primitives in that node. Remember that the SAH cost of a subdivision is estimated as (surface area of left box * number of primitives in left box + surface area of right box * number of primitives in right box ) / (surface area of encompassing box). Likewise the cost of intersecting a node is simply number of primitives in the node.

Timing your BVH's construction (5 points)

Use a QTime to measure how many milliseconds it takes your BVH to be constructed. QTime::start() sets the QTime's internal time counter to the present time, and QTime::elapsed() returns how many milliseconds have gone by since start() was invoked. Your program should print this time via cout, and there should be some sort of print message labeling this number as the build time of the BVH.

Please record this time in your README along with your render times and renders.

----------------------------------------------------------------------------------------------------------------

### Week 7: Thin Lens Camera + Extra Light Sources

Overview

As the semester draws to its close, we embark on the final additions to your path tracers. The first component of the final project will be to implement at least one of the "basic" features listed in the writeup below, along with a lens-based camera model capable of producing depth of field. We will not provide any base code for this assignment; it is up to you to create any classes you will need to complete your feature(s). These features will be due a week into the final project, but don't let that stop you from working on the larger features due at the end of the semester!


Lens-based camera (REQUIRED) (10 points)

Implement the thin-lens camera model as described in chapter 6.2.3 of PBRT. You may wish to make this class inherit from the base Camera class we have provided. You will also have to alter the Scene class to include an instance of this new camera type. Render at least one scene with this new camera in order to exhibit its depth-of-field effect.


Other light sources

Implement at least two of the light sources described in Chapter 12 of PBRT, aside from DiffuseAreaLight, which has already been provided to you. If you choose to implement additional light types, each one will be worth 10 extra credit points.



----------------------------------------------------------------------------------------------------------------


### Week 8: Photon Mapper

Implement a basic photon mapper that stores a set of global illumination photons and a set of caustic photons. 

Rather than storing a set of direct lighting photons, your renderer will simply evaluate the direct lighting at a
point using multiple importance sampling as in your path tracer. 

If you wish, you may try to make your renderer more efficient by also including a radiance
photon map, which stores the average of a set of surrounding photons. 

Finally, you will have to incorporate a volumetric data structure (e.g. a voxel grid or
kd-tree) in order to better store your photons and evaluate where they land.

----------------------------------------------------------------------------------------------------------------




