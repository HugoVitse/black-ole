
const float EPS = 0.001;
uniform float mass;



mat4 getMetric(vec4 x) {

    // float rs = 2.0; // Rayon de Schwarzschild (M=1)
    // float r = length(p.xyz); // Si on est en cartésien

    mat4 g = mat4(0.0);


    float term = 1 -  (2*mass)/(x.x);
    g[0][0] =  1/term;
    g[1][1] =  (x.x*x.x);
    g[2][2] =  (x.x*x.x) * (sin(x.y)*sin(x.y));
    g[3][3] =  -term;

    
    return g; 
}

//litteralement un calcul de dérivé avec h petit
mat4 getDerivMetric(vec4 p, int d_idx) {
    vec4 step = vec4(0.0);
    step[d_idx] = EPS; // le h s'applique que sur le paramètre a dériver !!!!!!!!!
    
    mat4 g_plus  = getMetric(p + step);
    mat4 g_minus = getMetric(p - step);
    
    return (g_plus - g_minus) / (2.0 * EPS);
}


// calcul des symboles de christoffel a partir de la métrique !!!! splendid !!
float getChristoffel(int mu, int alpha, int beta, vec4 p) {

    mat4 g = getMetric(p);
    mat4 g_inv = inverse(g); 
    
    float sum = 0.0;

    
    for(int lam = 0; lam < 4; lam++) {
        float g_upper = g_inv[lam][mu]; 
        
        if(abs(g_upper) < 1e-6) continue; 
        
        mat4 dg_dalpha = getDerivMetric(p, alpha);
        mat4 dg_dbeta  = getDerivMetric(p, beta);
        mat4 dg_dlam   = getDerivMetric(p, lam);
        
        float term = dg_dalpha[lam][beta] 
                   + dg_dbeta[lam][alpha] 
                   - dg_dlam[beta][alpha];
                   
        sum += 0.5 * g_upper * term;
    }
    
    return sum;
}