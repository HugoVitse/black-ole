#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "opengl.hpp"
#include "camera.hpp"
#include <cmath>
#include "commons.hpp"


int main() {

    OpenGL opengl;    
    opengl.initOpenGL();

 
    int frameCount = 0; 
    
    // Camera camera(8.7f, 1.37f, 2.6f, 3.14f, 0.088906f, 0.6f , WINDOW_W / 2.0, WINDOW_H / 2.0,true, 0.005f);
    Camera camera(20.5f, 1.67f, 2.6f, 1.53f, 0.0f, 0.5f , WINDOW_W / 2.0, WINDOW_H / 2.0,true, 0.005f);

    GLint camPosLoc = glGetUniformLocation(opengl.computeProgram, "camPos");
    GLint viewAnglesLoc = glGetUniformLocation(opengl.computeProgram, "viewAngles");
    GLint timeLoc = glGetUniformLocation(opengl.computeProgram, "time");
    GLint fovLoc = glGetUniformLocation(opengl.computeProgram, "fov");
    GLint skyboxLoc = glGetUniformLocation(opengl.computeProgram, "skybox");
    GLint sunLoc = glGetUniformLocation(opengl.computeProgram, "sunTex");

    GLint massLoc = glGetUniformLocation(opengl.computeProgram, "mass");
    GLint momentLoc = glGetUniformLocation(opengl.computeProgram, "moment");
    GLint sunRadiusLoc = glGetUniformLocation(opengl.computeProgram, "sunRadius");

    bool mouse = false;
    bool keyboard = true;

    float mass = 1.0f;
    float moment = 0.99f;
    
    int count = 0;

    int tileSize = 128;
    float initialsunRadius = 10.0f;
    float sunRadius = 9.90f;



    
    while (!glfwWindowShouldClose(opengl.window)) {
    

        if(keyboard) camera.handleKeyboard(opengl);
        if(mouse)    camera.mouseMoveCam(opengl);
        else         camera.autoMoveCam();

        // mass += 0.005;
        // if(mass>=0.8) mass=0.8;

        // moment += 0.01;
        // if(moment>=0.99) moment=0.99;

        float vitesse = -1 * sqrt(2*mass*( (1.0f/sunRadius) - (1.0f/initialsunRadius) ));
        printf("vitesse : %f\n", vitesse);

        // sunRadius += vitesse * 0.1;
        // if(sunRadius < 2*mass*0.8) {
        //     break;
        // } 

        // camera.camPhi += 0.01;
        printf("sunRadius : %f\n", sunRadius);

        printf("moment : %f\n", moment);

        //phase 1 : compute pixel with cpu parameters

        glUseProgram(opengl.computeProgram);

        glUniform1f(sunRadiusLoc, sunRadius);

        glUniform1f(fovLoc, camera.fov);
        glUniform1f(massLoc, mass);
        glUniform1f(momentLoc, moment);

        glUniform1f(timeLoc, (float)glfwGetTime());
        glUniform3f(camPosLoc, camera.camR, camera.camTheta, camera.camPhi);
        glUniform2f(viewAnglesLoc, camera.viewYaw, camera.viewPitch);
        
        //bind skybox to texture 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, opengl.skyboxTexID);
        glUniform1i(skyboxLoc, 1);

        //params
     
        //launch computation for each work_group (8*8 = 64 pixels (nvidia = 32warps , amd =64 wavefronts))
        glDispatchCompute((RENDER_W + 7) / 8, (RENDER_H + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glFinish();

        

        // phase 2 : display

        // glViewport(0, 0, WINDOW_W, WINDOW_H); 
        // glClear(GL_COLOR_BUFFER_BIT);
        
        // glUseProgram(opengl.displayProgram);
        
        // glBindVertexArray(opengl.quadVAO);

        // //bind screenTex to texture 0
        // glActiveTexture(GL_TEXTURE0); 
        // glBindTexture(GL_TEXTURE_2D, opengl.screenTex);
        
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        std::string filename = "../output_gpu/render_" + std::to_string(frameCount++) + ".png";
        opengl.saveImage(filename.c_str());

        glfwSwapBuffers(opengl.window);
        glfwPollEvents();
        count+=1;
    }

    glfwTerminate();
    return 0;
}
