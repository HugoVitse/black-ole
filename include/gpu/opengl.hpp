#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "commons.hpp"


struct OpenGL {


    void saveImage(const char* filename);
    void initOpenGL();

    GLuint screenTex;
    GLuint computeProgram;
    GLuint displayProgram;
    GLuint quadVAO;
    GLuint skyboxTexID;
    GLuint sunTexID;

    GLFWwindow* window;

    private:
    
    GLuint createShader(const char* sourceCode, GLenum type);
    GLuint createComputeShader(const std::vector<std::string> &sources, GLenum type);

    std::string loadFile(const char* filename);
    GLuint loadTexture(const char* path);
};