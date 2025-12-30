struct Photon {
    vec4 x; // r, theta, phi, t
    vec4 k; // kr, ktheta, kphi, kt
};



Photon F(vec4 x, vec4 k, BlackHole blackhole) {

   
    float dkr =  -(christoffel(0,0,0,x,blackhole) * k.x * k.x + christoffel(0,1,1,x,blackhole) * k.y * k.y +  christoffel(0,2,2,x,blackhole) * k.z * k.z +  christoffel(0,3,3,x,blackhole) * k.w * k.w);
    float dkth = - ( 2.0 * christoffel(1,0,1,x,blackhole) * k.x * k.y + christoffel(1,2,2,x, blackhole) * k.z * k.z );
    float dkph = - ( 2.0 * christoffel(2,0,2,x, blackhole) * k.x * k.z +  2.0 * christoffel(2,1,2,x,blackhole) * k.y * k.z );
    float dkt  = - ( 2.0 * christoffel(3,0,3,x,blackhole) * k.x * k.w);

    Photon p;
    p.x = vec4(k.x, k.y, k.z, k.w);    // dx/dt = k
    p.k = vec4(dkr, dkth, dkph, dkt); // dk/dt = accélération
    return p;
}


void RK4step(BlackHole blackhole, float h, inout Photon photon) {

    Photon p = Photon(photon.x, photon.k);
    Photon k1 = F(p.x, p.k, blackhole);
    
    Photon p2 = Photon(photon.x + k1.x * (h*0.5), photon.k + k1.k * (h*0.5));
    Photon k2 = F(p2.x, p2.k, blackhole);
    
    Photon p3 = Photon(photon.x + k2.x * (h*0.5), photon.k + k2.k * (h*0.5));
    Photon k3 = F(p3.x, p3.k, blackhole);
    
    Photon p4 = Photon(photon.x + k3.x * h, photon.k + k3.k * h);
    Photon k4 = F(p4.x, p4.k, blackhole);


    photon.x += (k1.x + k2.x*2.0 + k3.x*2.0 + k4.x) * (h/6.0);
    photon.k += (k1.k + k2.k*2.0 + k3.k*2.0 + k4.k) * (h/6.0);


}