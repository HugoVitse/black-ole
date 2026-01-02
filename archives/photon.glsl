

struct Photon {
    vec4 x; // r, theta, phi, t
    vec4 k; // kr, ktheta, kphi, kt
};

Photon F(inout vec4 x, inout vec4 k, inout BlackHole blackhole) {


    Photon p;
    p.x = vec4(k.x, k.y, k.z, k.w);    // dx/dt = k
    p.k = vec4((-k.x*k.x*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(x.x*(-2*mass*x.x + moment*moment + x.x*x.x) + (mass - x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x))*(-2.0*mass*x.x + 1.0*moment*moment + 1.0*x.x*x.x) + 1.0*k.x*k.y*moment*moment*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*sin(2*x.y) + 1.0*k.y*k.y*x.x*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x) + (-2*mass*x.x + moment*moment + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*(1.0*k.w*k.w*mass*(moment*moment*cos(x.y)*cos(x.y) - x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x) - 2.0*k.w*k.z*mass*moment*(moment*moment*cos(x.y)*cos(x.y) - x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*sin(x.y)*sin(x.y) + k.z*k.z*(-2.0*mass*x.x + 1.0*moment*moment + 1.0*x.x*x.x)*(-2*mass*moment*moment*x.x*x.x*sin(x.y)*sin(x.y) + mass*moment*moment*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*sin(x.y)*sin(x.y) + x.x*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x))*sin(x.y)*sin(x.y)))/((moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)), (k.w*mass*moment*x.x*(8.0*k.w*moment - 16.0*k.z*(moment*moment + x.x*x.x))*(-2*mass*x.x + moment*moment + x.x*x.x)*sin(2*x.y)/8 - 0.5*k.x*k.x*moment*moment*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*sin(2*x.y) - 2.0*k.x*k.y*x.x*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x) + 0.5*k.y*k.y*moment*moment*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*sin(2*x.y) + k.z*k.z*(2.0*mass*moment*moment*x.x*(moment*moment + x.x*x.x)*sin(x.y)*sin(x.y) + 1.0*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(2*mass*moment*moment*x.x*sin(x.y)*sin(x.y) + (moment*moment + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)))*(-2*mass*x.x + moment*moment + x.x*x.x)*sin(x.y)*cos(x.y))/((moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)), 0.0625*(32.0*k.w*k.x*mass*moment*(moment*moment*cos(x.y)*cos(x.y) - x.x*x.x)*tan(x.y) + 64.0*k.w*k.y*mass*moment*x.x*(-2*mass*x.x + moment*moment + x.x*x.x) + 32.0*k.x*k.z*(1.0*mass*moment*moment*moment*moment*sin(x.y)*sin(x.y)*sin(x.y)*sin(x.y) + 1.0*mass*moment*moment*moment*moment*cos(x.y)*cos(x.y) - 1.0*mass*moment*moment*moment*moment + 1.0*mass*moment*moment*x.x*x.x*cos(x.y)*cos(x.y) + 1.0*mass*moment*moment*x.x*x.x + 2.0*mass*x.x*x.x*x.x*x.x - 1.0*moment*moment*moment*moment*x.x*sin(x.y)*sin(x.y)*sin(x.y)*sin(x.y) - 2.0*moment*moment*moment*moment*x.x*cos(x.y)*cos(x.y) + 1.0*moment*moment*moment*moment*x.x - 2.0*moment*moment*x.x*x.x*x.x*cos(x.y)*cos(x.y) - 1.0*x.x*x.x*x.x*x.x*x.x)*tan(x.y) - 16.0*k.y*k.z*(-2*mass*x.x + moment*moment + x.x*x.x)*(4.0*mass*moment*moment*x.x*sin(x.y)*sin(x.y) + 2.0*moment*moment*moment*moment*sin(x.y)*sin(x.y)*sin(x.y)*sin(x.y) - 4.0*moment*moment*moment*moment*sin(x.y)*sin(x.y) + 2.0*moment*moment*moment*moment - 4.0*moment*moment*x.x*x.x*sin(x.y)*sin(x.y) + 4.0*moment*moment*x.x*x.x + 2.0*x.x*x.x*x.x*x.x))/((moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*tan(x.y)), 0.125*mass*(16.0*k.w*k.x*(-2.0*mass*x.x + 1.0*moment*moment + 1.0*x.x*x.x)*(-moment*moment*moment*moment*sin(x.y)*sin(x.y) + moment*moment*moment*moment - moment*moment*x.x*x.x*sin(x.y)*sin(x.y) - x.x*x.x*x.x*x.x) + 4.0*k.w*k.y*moment*moment*x.x*(-8.0*mass*x.x + 4.0*moment*moment + 4.0*x.x*x.x)*(-2.0*mass*x.x + 1.0*moment*moment + 1.0*x.x*x.x)*sin(2*x.y) + 16.0*k.x*k.z*moment*(-2*mass*x.x + moment*moment + x.x*x.x)*(-1.0*moment*moment*moment*moment*cos(x.y)*cos(x.y) + 1.0*moment*moment*x.x*x.x*cos(x.y)*cos(x.y) + 1.0*moment*moment*x.x*x.x + 3.0*x.x*x.x*x.x*x.x)*sin(x.y)*sin(x.y) - 32.0*k.y*k.z*moment*moment*moment*x.x*(-2*mass*x.x + moment*moment + x.x*x.x)*(-2.0*mass*x.x + 1.0*moment*moment + 1.0*x.x*x.x)*sin(x.y)*sin(x.y)*sin(x.y)*cos(x.y))/((moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(moment*moment*cos(x.y)*cos(x.y) + x.x*x.x)*(-2*mass*x.x + moment*moment + x.x*x.x)*(-2.0*mass*x.x + 1.0*moment*moment + 1.0*x.x*x.x))); // dk/dt = accélération
    return p;
}


void RK4step(inout BlackHole blackhole, float pas, inout Photon photon) {

    Photon p = Photon(photon.x, photon.k);
    Photon k1 = F(p.x, p.k, blackhole);
    
    Photon p2 = Photon(photon.x + k1.x * (pas*0.5), photon.k + k1.k * (pas*0.5));
    Photon k2 = F(p2.x, p2.k, blackhole);
    
    Photon p3 = Photon(photon.x + k2.x * (pas*0.5), photon.k + k2.k * (pas*0.5));
    Photon k3 = F(p3.x, p3.k, blackhole);
    
    Photon p4 = Photon(photon.x + k3.x * pas, photon.k + k3.k * pas);
    Photon k4 = F(p4.x, p4.k, blackhole);


    photon.x += (k1.x + k2.x*2.0 + k3.x*2.0 + k4.x) * (pas/6.0);
    photon.k += (k1.k + k2.k*2.0 + k3.k*2.0 + k4.k) * (pas/6.0);
}

