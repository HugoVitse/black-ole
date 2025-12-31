struct Tetrade {
    vec4 e0;
    vec4 e1;
    vec4 e2;
    vec4 e3;
};



void initTetrade(inout Tetrade tetrade, vec4 camPos){

    float f =  1;//1.0 - (2.0 / camPos.x); //
    double sinth = sin(camPos.y);
    if (abs(sinth) < 1e-12) sinth = (sinth >= 0 ? 1e-12 : -1e-12); // pour pas diviser par 0

    tetrade.e0 = vec4(0.0, 0.0, sqrt(f), 0.0);
    tetrade.e1 = vec4(0.0, -(1.0 / camPos.x), 0.0, 0.0);
    tetrade.e2 = vec4(1.0 / (camPos.x * sinth), 0.0, 0.0, 0.0);
    tetrade.e3 = vec4(0.0, 0.0, 0.0, 1.0 / sqrt(f));
}
