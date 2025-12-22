<h1>black++ole</h1>
<p style="color:gray; margin-top:-15px; font-size:1.2em;">
    Black Hole simulation in C++
</p>

---
## Classes

Thoses classes and struct will be used in our code. Setters and getters are implicit

### Vectors

#### Vec3

Space-only vector

```cpp
struct Vec3 {
    double x,y,z;
    double &r, &theta, &phi //aliases for spherical
}
```

#### Vec4

Space-time  quadrivector

```cpp
struct Vec4 {
    double t,x,y,z
    double &r,&theta,&phi;  
```
### Black Hole

```cpp
class BlackHole {
    double mass;
    doubls rs;
    double christoffel(int mu, int alpha, int beta, const Vec4& x);

}
```

### Photon

```cpp
class Photon {
    Vec4 x;  //position
    Vec4 k;  //direction

    void RK4step( const BlackHole& blackHole, double h);
}
```

### Camera


```cpp
class Camera {
    Vec3 position; //globaly
    double fov;
    Image image;
}
```
#### Image

```cpp
class Image {
    int width, height;
    Pixel** pixels;
}
```

##### Pixel

```cpp
class Pixel {
    int i,j;
    char r,g,b;
    Photon* photon;

    void castPhoton();
    void computeColour(Skybox* skybox)
}
```
### Skybox
```cpp
struct Skybox {
    Image texture;  // equirectangular
};
```


$$\phi_{\rm sky} = \operatorname{atan2}(y, x)     \quad\text{(range }(-\pi,\pi]\text{)}
\theta_{\rm sky} = \arccos\big( z / \|\mathbf{v}\|\big) \quad\text{(0..}\pi\text{)}$$