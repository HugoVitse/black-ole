<h1>black++ole</h1>
<p style="color:gray; margin-top:-15px; font-size:1.2em;">
    Black Hole simulation in C++
</p>

---

## Physics

### Schwarzschild Black Hole

A black hole is an astronomical body so compact that its gravity prevents anything from escaping, even light. 

It is defined by its **mass** $M$ and its **Schwarzschild radius**

$$
r_s = \frac{2GM}{c^2}
$$

The black hole's singularity is the origin of the global referential of our model thats uses $(ct, r, \theta, \phi)$ coordinates.

The black hole is curving the space-time due to his mass.  This curvation is given by **Schwarzschild metric**

$$\mathrm{d}s^{2} = -\left(1-\frac{2GM}{c^{2}r}\right)c^{2}\mathrm{d}t^{2}+ \left(1-\frac{2GM}{c^{2}r}\right)^{-1}\mathrm{d}r^{2}+ r^{2}\,\mathrm{d}\theta^{2}+ r^{2}\sin^{2}\theta\,\mathrm{d}\varphi^{2}$$

### Ray tracing

The method used in this projet will be backward **ray tracing**. It consists, for each **pixel** of the **image**, in determining the direction in which it arrived then compute its origin by rewinding the trajectory

#### Camera

The **camera** is the point from which we are looking at the scene. It'll also be the starting point of each photon casted. The camera also has its own local referential used to compute the direction of an incoming photon

$$ x_{cam_{local}} = (0,0,0)$$

#### Image

The image is the rectangular 2D plan that is seen by the **camera**. It is determined thanks to te **FOV** (field of view) and the distance between the camera and the image (1 by default in the camera local ref.). So the center of the image should be at $(0,0,1)$

<p align="center">
    <img src="assets/Ray_trace_diagram.svg" alt="texte alternatif" style="width:40%; max-width:400px;">
</p>

This image should also correspond to an array of pixels with the resolution : **W** for width and **H** for height

From there we can compute the demi-height of the image : 

$$ h = \frac{1}{\tan{(FOV)}}$$

We have to keep the same ratio as our resolution

$$ w = h * \frac{W}{H}$$

#### Direction of the photon (locally)

The next step is to compute , for each pixel of the image, the direction of the incoming photon. The direction is the vector defined by the pixel's coordinates and the camera coordinates. In fact , because of the camera being the origin of its own local referential, the direction simply is the pixel's coordinates

##### Pixel coordinates in local referential

To compute some pixel **(i,j)** coordinates in the local referential, we just need to proportionally transform i, which belongs to $[0, W]$ to an $x_{pixel}$ which should belongs to $[-w, +w]$. Same things with j in $[0,H]$ to $y_{pixel}$ in $[-h,h]$


To achieve this, we start by substracting $\frac{W}{2}$ to $i$ so it will reduce the interval to $[-\frac{W}{2}; \frac{W}{2}]$

Next we multiply by $\frac{2w}{W}$ to bring the $[-w,w]$

$$ (x_{pixel}, y_{pixel}, z_{pixel}) = \left(  \frac{2w}{W}(i-\frac{W}{2}) ,  \frac{2h}{H}(j-\frac{H}{2}), 1 \right)$$

$z_{pixel}$ is always 1 because every pixels are on the same plan (the image)

In order to only keep the direction information we can normalize this vector to simplify and make the computations easier.
We can call this vector $\mathrm{dir}_{local}$

$$\mathrm{dir}_{local} = \frac{1}{\sqrt{( x_{pixel}^2+y_{pixel}^2+ z_{pixel}^2)}}   \left( x_{pixel},y_{pixel}, z_{pixel} \right)$$



##### Photon's quadrivector

The photon quadrivector is the $4$ dimensional vector (considering time) representing the photon direction.

When the photon hit the camera, his direction would be 
$$k_{local} = (1,\mathrm{dir}_{{local}_x}, \mathrm{dir}_{{local}_y}, \mathrm{dir}_{{local}_z} )$$ 

#### Direction of the photon (globally)

The next step is to convert our local space-time photon direction to the global referential (the blackhole's one).
So first, we need to fix a global position for the camera

$$x_{cam} = (t_{cam}, r_{cam}, \theta_{cam}, \phi_{cam})$$

##### Change of basis

In order to perform the change of basis we need the matrix corresponding. It is composed of 4 vectors given by Schwarzschild's metric

$$
e_{(0)}^{\mu} = \left( \frac{1}{\sqrt{1-\frac{2M}{r_{cam}}}},\ 0,\ 0,\ 0 \right)
$$

$$
e_{(1)}^{\mu} = \left( 0,\ \sqrt{1-\frac{2M}{r_{cam}}},\ 0,\ 0 \right)
$$

$$
e_{(2)}^{\mu} = \left( 0,\ 0,\ \frac{1}{r_{cam}},\ 0 \right)
$$

$$
e_{(3)}^{\mu} = \left( 0,\ 0,\ 0,\ \frac{1}{r_{cam} \sin\theta_{cam}} \right)
$$

Those are the base vector of the local referential expressed with the global referential's coordinates


##### Photon's quadrivector

So, we can compute our $k^{\mu}_{global}$

$$k^{\mu}_{global}  = e_{(a)}^{\mu}k_{local}^{(a)}$$


#### Photon's trajectory

Since we have the final direction of the photon which is $k^{\mu}_{global}$ ans the final position of it $(x_{cam})$, we need to rewind its trajectory in order to determine where it comes from.

##### Geodesics equation

the photon's trajectory is given by the geodesics equation

$$  \frac{ d^{2} x^{\mu} }{ d \lambda^{2} }+ \Gamma^{\mu}_{\alpha\beta} \frac{d x^{\alpha}}{d\lambda}\frac{d x^{\beta}}{d\lambda}= 0  $$

since we know $\frac{d x^{\mu}}{d\lambda} = k^{\mu}$, we can write

$$\frac{d k^{\mu}}{d\lambda}= - \Gamma^{\mu}_{\alpha\beta}k^{\alpha} k^{\beta}$$

So we'll keep those two equations

$$\frac{d x^{\mu}}{d\lambda} = k^{\mu}$$
$$\frac{d k^{\mu}}{d\lambda}= - \Gamma^{\mu}_{\alpha\beta}k^{\alpha} k^{\beta}$$


##### Christoffel symbols

Christoffel symbols are the $\Gamma$ symbols involved in one of our equations. They act like a force curving the trajectory. There are 64 of them and its expressions depends on the metric. With Schwarzschild metric, only 13 of them are non-nul. Here there are

$$
\Gamma^{t}_{\;tr} = \Gamma^{t}_{\;rt}
= \frac{M}{r(r-2M)}
$$

$$
\Gamma^{r}_{\;tt}
= \frac{M(r-2M)}{r^{3}}
$$

$$
\Gamma^{r}_{\;rr}
= -\,\frac{M}{r(r-2M)}
$$

$$
\Gamma^{r}_{\;\theta\theta}
= -(r-2M)
$$

$$
\Gamma^{r}_{\;\phi\phi}
= -(r-2M)\,\sin^{2}\theta
$$

$$
\Gamma^{\theta}_{\;r\theta}
= \Gamma^{\theta}_{\;\theta r}
= \frac{1}{r}
$$

$$
\Gamma^{\theta}_{\;\phi\phi}
= -\,\sin\theta\,\cos\theta
$$

$$
\Gamma^{\phi}_{\;r\phi}
= \Gamma^{\phi}_{\;\phi r}
= \frac{1}{r}
$$

$$
\Gamma^{\phi}_{\;\theta\phi}
= \Gamma^{\phi}_{\;\phi\theta}
= \cot\theta
$$


##### RK4

Since those equations doesnt have analytics solutions, we'll rewind them step by step using RK4 algorithm.

RK4 algorithm work by giving it a $X_n$ state and its derivative $F(X_n)$ then it computes $X_{n+1}$

In our case we'll have : 

$$X = (t,r,\theta,\phi,k^t, k^r, k^\theta, k^\phi)$$

It corresponds to the left parts of our 2 equations above

$$F(X) = (k^t, k^r, k^\theta, k^\phi, - \Gamma^{t}_{\alpha\beta}k^{\alpha} k^{\beta},- \Gamma^{r}_{\alpha\beta}k^{\alpha} k^{\beta},- \Gamma^{\theta}_{\alpha\beta}k^{\alpha} k^{\beta},- \Gamma^{\phi}_{\alpha\beta}k^{\alpha} k^{\beta})$$

Given a state $X_n$, the following state $X_{n+1}$ is computed as following : 

$$ X_{n+1} = X_n + \frac{h}{6}(k_1+2k_2+2k_3+k_4)$$

with

$$ k_1 = F(X_n)$$
$$k_2 = F(X_n + k_1\frac{h}{2})$$
$$k_3= F(X_n + k_2\frac{h}{2})$$
$$k_4 = F(X_n + hk_3)$$

###### Stop Conditions

We should stop the algorithm whenever $r$ composant of $X_n$ exceed a defined upper or lower limit,

$$ r > r_{max}$$

When $r$ exceed $r_{max}$, which should be defined to be an enough far distance from the black hole, we simply pick the pixel from the skybox at the current $(r,\theta,\phi)$ position

$$ r < r_s$$

When $r$ exceed $r_s$ (Schwarzschild radius), the photon is trapped into the black hole and we must return a black pixel then.