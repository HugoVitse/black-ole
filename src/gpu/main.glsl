
void main() {


    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    float _h = 1/tan(fov);


    initTetrade(tetrade, vec4(camPos,0.0) );
    initPixel(pixel, _h ,camPos, imgOutput, viewAngles, fov, tetrade);

    blackhole.rs = 2*mass;
 
    for(int i = 0; i < maxIter; i++) {

        float prevTheta = pixel.photon.x.y;  //sauvegarde des coordonnées précédentes pour detecter une potnetielle traversée de disque
        float prevR = pixel.photon.x.x;

        RK4step(blackhole, h, pixel.photon);

        // stop conditions
        if (pixel.photon.x.x <= blackhole.rs * 1.01) {
            hitHorizon = true;
            break;
        }
        
        if (pixel.photon.x.x > blackhole.rmax) {
            break;
        }


        if(!handleCollision(prevTheta, prevR , pixel, disk, finalColor, transmittance, time, blackhole)) break;     


    }

    // set color
   
    setColor(hitHorizon, transmittance, finalColor ,skybox, pixel);
    imageStore(imgOutput, pixel_coords, vec4(pixel.color, 1.0));

   
}