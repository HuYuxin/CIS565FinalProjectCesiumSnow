Perlin noise, 2D, 3D and 4D, classic and simplex,
in a GLSL fragment shader
=================================================


"Classic noise" is the old favourite which everyone uses.

float noise(vec2 P)
float noise(vec3 P)
float noise(vec4 P)

The implementation is pretty straightforward, and I wish to
extend my thanks to Simon Green at Nvidia who gave me a good
start with his own implementation of classic Perlin noise,
and to Bill Licea-Kane at ATI for his help with a few GLSL
quirks and texture wrapping details.


"Simplex noise" is a new and improved creation from Ken Perlin.

float snoise(vec2 P)
float snoise(vec3 P)
float snoise(vec4 P)

I re-implemented it from scratch from Perlin's verbal desciption,
because I found his code to be extremely hard to read, and
for the most part it was impossible to port it to GLSL anyway.
The 2D and 4D cases were implemented from the verbal description
only, I haven't seen any example code from anyone there.
I ended up doing the gradient lookup in a different manner
to fit into GLSL 1.10. Perlin's reference implementation makes
heavy use of bit-wise operations on integers, which is not
implemented in GLSL 1.10. My version simply uses the same
gradient hash table as the classic noise algorithm.

Simplex noise has one very big advantage: it extends to 4D and
higher with only marginal slowdown.
This is because it evaluates only N+1 points for N dimensions,
while classic noise evaluates 2^N points for N dimensions.
In 4D and higher, simplex noise is a big win. For 2D and 3D it
is not a whole lot faster - it might even be marginally slower
for 2D in this GLSL implementation, depending on your hardware.

Simplex noise also looks a lot better, because it doesn't have
the clearly noticeable axis-aligned structure of classic noise.
(This is because the underlying grid is tetrahedral, not cubical.)

It's also a lot simpler to compute the derivative of simplex noise.
There is no interpolation involved, just a summation of polynomial
components, so it's just a matter of taking the derivative of those
polynomials, evaluate them separately and then add them together.

Once you've tried simplex noise, you'll never be quite happy with
classic noise again. It's a whole lot better in every respect.

High end hardware of late 2004 is reported to attain speeds of
around 100 million noise samples per second with this noise(vec3),
so direct rendering of several noise values per pixel is possible,
but a render to texture with less than one update per frame of
noise components that change slowly is highly recommended.
This procedural noise does not come cheap.

On a low-end but GLSL-capable GeForce FX 5600 XT, I get around
2.5 million samples of noise per second with noise(vec3). That
is only about as fast as CPU noise on a 2-3 GHz CPU. 2D noise
is about three to four times as fast, though.


Stefan Gustavson (stegu@itn.liu.se) 2004-12-05
