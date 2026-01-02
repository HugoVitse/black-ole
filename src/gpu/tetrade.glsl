uniform float mass;
uniform float moment;


struct Tetrade {
    vec4 e0;
    vec4 e1;
    vec4 e2;
    vec4 e3;
};



void initTetrade(inout Tetrade tetrade, vec4 camPos){

    float f = 1.0 - (2.0*mass / camPos.x); //
    double sinth = sin(camPos.y);
    if (abs(sinth) < 1e-12) sinth = (sinth >= 0 ? 1e-12 : -1e-12); // pour pas diviser par 0


    tetrade.e0 = vec4(sqrt(f), 0.0, 0.0, 0.0);
    tetrade.e1 = vec4(0.0, (1.0 / camPos.x), 0.0, 0.0);
    tetrade.e2 = vec4(0.0, 0.0, -1.0 / (camPos.x * sinth), 0.0);
    tetrade.e3 = vec4(0.0, 0.0, 0.0, 1.0 / sqrt(f));

    // float Delta_raw = camPos.x * camPos.x + moment*moment - 2*mass*camPos.x;
    // float Delta = max(1e-5, Delta_raw);
    // float rho_squared = (camPos.x*camPos.x)  + ( (moment*moment) * (cos(camPos.y)*cos(camPos.y)) );
    // float Sigma_squared = (( (camPos.x)*(camPos.x) + moment*moment )*( (camPos.x)*(camPos.x) + moment*moment ) ) - (   (moment*moment) * Delta * ( sin(camPos.y)*sin(camPos.y) )  );

    // float alpha_squared = ( rho_squared / Sigma_squared) * Delta;
    // float pi_squared = (Sigma_squared/rho_squared) * (sin(camPos.y) * sin(camPos.y));
    // float omega = (2*moment*mass*camPos.x) / (Sigma_squared);

    // tetrade.e0 = vec4(sqrt(Delta)/sqrt(rho_squared), 0.0, 0.0, 0.0);
    // tetrade.e1 = vec4(0.0, (1.0 / sqrt(rho_squared)), 0.0, 0.0);
    // tetrade.e2 = vec4(0.0, 0.0, -1.0 / (sqrt(pi_squared)), 0.0);
    // tetrade.e3 = vec4(0.0, 0.0, omega/sqrt(alpha_squared), 1.0 /sqrt(alpha_squared)  );

}

