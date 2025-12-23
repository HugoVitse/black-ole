#include "skybox.hpp"


void Skybox::getColor(double theta, double phi, int &r, int &g, int &b) const{

    //coordonées uv sur l'image equireactangulaire
    double u = fmod(phi, 2.0 * M_PI) / (2.0 * M_PI);
    if (u < 0) u += 1.0; 
    double v = theta / M_PI;

    //passage en pixel
    int x = (int)(u * (width - 1));
    int y = (int)(v * (height - 1));

    int index = (y * width + x) * 3;
    r = data[index];
    g = data[index + 1];
    b = data[index + 2];

    // if(data[index] == 0 && data[index + 1] == 0 && data[index + 2] == 0 && phi == 0){
    //     printf("index : %d\n", index);
    //     printf("x : %d\n", x);
    //     printf("y : %d\n", y);
    //     printf("width : %d\n", width);
    //     printf("height : %d\n", height);
    //     printf("phi : %f\n", phi);
    //     printf("theta : %f\n", theta);
    //     printf("u : %f\n", u);
    //     printf("v : %f\n", v);
    // }
}

void Skybox::load(const char* filename) {

    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return;

    //header ppm
    char magic[3];
    fscanf(file, "%s\n%d %d\n255\n", magic, &width, &height);


    data.resize(width * height * 3);
    fread(data.data(), 1, data.size(), file);
    fclose(file);


}