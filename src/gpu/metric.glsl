
const float EPS = 0.001;



mat4 getMetric(vec4 x) {

    mat4 g = mat4(0.0);

    

    float Delta_raw = x.x * x.x + moment*moment - 2*mass*x.x;
    float Delta = max(1e-5, Delta_raw);
    float rho_squared = (x.x*x.x)  + ( (moment*moment) * (cos(x.y)*cos(x.y)) );
    float Sigma_squared = (( (x.x)*(x.x) + moment*moment )*( (x.x)*(x.x) + moment*moment ) ) - (   (moment*moment) * Delta * ( sin(x.y)*sin(x.y) )  );

    float alpha_squared = ( rho_squared / Sigma_squared) * Delta;
    float pi_squared = (Sigma_squared/rho_squared) * (sin(x.y) * sin(x.y));
    float omega = (2*moment*mass*x.x) / (Sigma_squared);
    

    g[0][0] =  (rho_squared/Delta);
    g[1][1] =  rho_squared;
    g[2][2] =  pi_squared;
    g[3][3] =  (-1 * alpha_squared) + ((omega*omega) * pi_squared);

    g[2][3] = -1 * omega * pi_squared;
    g[3][2] = -1 * omega * pi_squared;

    

    // float term = 1 -  (2*mass)/(x.x);
    // g[0][0] =  1/term;
    // g[1][1] =  (x.x*x.x);
    // g[2][2] =  (x.x*x.x) * (sin(x.y)*sin(x.y));
    // g[3][3] =  -term;


    
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