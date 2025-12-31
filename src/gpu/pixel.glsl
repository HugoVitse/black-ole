

struct Pixel {
    vec3 color;
    int i;
    int j;

    Photon photon;
    
};

mat3 rotateY(float angle) { 
    float c = cos(angle); float s = sin(angle);
    return mat3(c, 0, s,  0, 1, 0,  -s, 0, c);
}
mat3 rotateX(float angle) { 
    float c = cos(angle); float s = sin(angle);
    return mat3(1, 0, 0,  0, c, -s,  0, s, c);
}


#define M_PI 3.14159265358979323846


void initPixel(inout Pixel pixel, float h, vec3 camPos, image2D imgOutput, vec2 viewAngles, float fov, inout Tetrade tetrade){
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(imgOutput);

    pixel.i = pixel_coords.x;
    pixel.j = pixel_coords.y;
   

    float W = dims.x;
    float H = dims.y;

    float w = h * W/H;

    //on passe à l'interval -w; w
    float xtmp = ((2.0 * w) / float(W)) * (float(pixel.i) - (float(W) / 2.0));
    float ytmp = ((2.0 * h) / float(H)) * (float(pixel.j) - (float(H) / 2.0));
    float ztmp = 1;

    // float aspect_ratio = float(dims.x) / float(dims.y);
    
    vec3 k_local_space = normalize(vec3(xtmp*fov,ytmp*fov,ztmp));
    k_local_space = rotateY(viewAngles.x) * rotateX(viewAngles.y) * k_local_space;
    vec4 k_local = vec4( k_local_space , 1.0);

   
   

    // kr = -1.0 (Le photon part vers le trou noir)

    vec4 x = vec4(camPos, 0.0);

    vec4 k_global =  vec4(
        dot(k_local,tetrade.e0),
        dot(k_local,tetrade.e1),
        dot(k_local,tetrade.e2),
        0.0
    );

    pixel.photon.k = k_global;
    pixel.photon.x = x;


    // // ensure initial photon's 4-momentum is null: g_{μν} k^μ k^ν = 0 (important)
    // float f = 1.0 - blackhole.rs/x.x;
    // float spatial = (1.0/f)*k.x*k.x + x.x*x.x*k.y*k.y + x.x*x.x*sin(x.y)*sin(x.y)*k.z*k.z;
    // k.w = sqrt(spatial / f);
}


void castPhoton() {

}

void setColor(bool hitHorizon, float transmittance, vec4 finalColor, sampler2D skybox, inout Pixel pixel) {
    vec3 color;
    if (hitHorizon) {
        color = vec3(0.0); // noir
    } else {
        // convertisseur sphérique vers uv de la skybox
        float u_tex = pixel.photon.x.z / (2.0 * M_PI);
        float v_tex = pixel.photon.x.y / M_PI;
        
        color = texture(skybox, vec2(u_tex, v_tex)).rgb;
    }

    vec3 result = finalColor.rgb + color * transmittance;
    pixel.color = result;
}