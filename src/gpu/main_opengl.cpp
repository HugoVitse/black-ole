#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "opengl.hpp"
#include "camera.hpp"
#include "commons.hpp"


int main() {

    OpenGL opengl;    
    opengl.initOpenGL();

 
    int frameCount = 0; 
    
    Camera camera(6.0f, 1.37f, 2.6f, 3.107012f, 0.088906f, 1.0f , WINDOW_W / 2.0, WINDOW_H / 2.0,true, 0.005f);
    
    GLint camPosLoc = glGetUniformLocation(opengl.computeProgram, "camPos");
    GLint viewAnglesLoc = glGetUniformLocation(opengl.computeProgram, "viewAngles");
    GLint timeLoc = glGetUniformLocation(opengl.computeProgram, "time");
    GLint fovLoc = glGetUniformLocation(opengl.computeProgram, "fov");
    GLint skyboxLoc = glGetUniformLocation(opengl.computeProgram, "skybox");
    GLint massLoc = glGetUniformLocation(opengl.computeProgram, "mass");

    
    bool mouse = false;
    bool keyboard = true;

    float mass = 0.0f;
    
    
    int count = 0;
    while (!glfwWindowShouldClose(opengl.window) && count <= 600) {
    

        if(keyboard) camera.handleKeyboard(opengl);
        if(mouse)    camera.mouseMoveCam(opengl);
        else         camera.autoMoveCam();

        mass += 0.005;
        if(mass>=1.0) mass=1.0;

        //phase 1 : compute pixel with cpu parameters

        glUseProgram(opengl.computeProgram);

        //params
        glUniform1f(fovLoc, camera.fov);
        glUniform1f(massLoc, mass);

        glUniform1f(timeLoc, (float)glfwGetTime());
        glUniform3f(camPosLoc, camera.camR, camera.camTheta, camera.camPhi);
        glUniform2f(viewAnglesLoc, camera.viewYaw, camera.viewPitch);
        
        
        //bind skybox to texture 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, opengl.skyboxTexID);
        glUniform1i(skyboxLoc, 1);
      

        //launch computation for each work_group (8*8 = 64 pixels (nvidia = 32warps , amd =64 wavefronts))
        glDispatchCompute((RENDER_W + 7) / 8, (RENDER_H + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        // phase 2 : display

        glViewport(0, 0, WINDOW_W, WINDOW_H); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(opengl.displayProgram);
        
        glBindVertexArray(opengl.quadVAO);

        //bind screenTex to texture 0
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, opengl.screenTex);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);

        std::string filename = "../output_gpu/render_" + std::to_string(frameCount++) + ".png";
        opengl.saveImage(filename.c_str());

        glfwSwapBuffers(opengl.window);
        glfwPollEvents();
        count+=1;
    }

    glfwTerminate();
    return 0;
}
