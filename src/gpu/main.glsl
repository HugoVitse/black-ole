

void main() {


    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    
    float _h = 1/tan(fov);

    // Calcul correct de l'horizon externe de Kerr (indépendant de theta)
    blackhole.rs = mass + sqrt(mass*mass - moment*moment);

    initTetrade(tetrade, vec4(camPos,0.0) );
    initPixel(pixel, _h ,camPos, imgOutput, viewAngles, fov, tetrade, blackhole);

    // blackhole.rs = 2*mass;
    // float delta_horizon = mass * mass - moment * moment;
    bool hitSun = false;
    // Sun sun = Sun(10.0);


    int i = 0;
    for(i = 0; i < maxIter; i++) {

        float prevTheta = pixel.photon.x.y;  //sauvegarde des coordonnées précédentes pour detecter une potnetielle traversée de disque
        float prevR = pixel.photon.x.x;
        float r = pixel.photon.x.x;

        float h_adaptive = 0.02; // Valeur par défaut
        
        if (r > 10.0) h_adaptive = 0.1;
        else if (r > 5.0) h_adaptive = 0.1;
        else if (r > 2.0) h_adaptive = 0.01;
        else h_adaptive = 0.01;

 
        // else h_adaptive = 0.1;
        // sun.radius = sunRadius;
                   
        RK4step(blackhole, h_adaptive, pixel.photon);

        // if (pixel.photon.x.x <= sun.radius * 1.01 ) {
        //     hitSun = true;
        //     break;
        // }

        if (pixel.photon.x.x <= blackhole.rs * 1.015 ) {
            hitHorizon = true;
            break;
        }
        
        if (pixel.photon.x.x > blackhole.rmax) {
            break;
        }


        //if(!handleCollision(prevTheta, prevR , pixel, disk, finalColor, transmittance, time, blackhole)) break;     


    }



    // set color
   
    setColor(hitHorizon, hitSun, transmittance, finalColor ,skybox, sunTex, pixel);
    imageStore(imgOutput, pixel_coords, vec4(pixel.color, 1.0));

   
}