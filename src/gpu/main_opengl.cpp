#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


const int RENDER_W = 800;
const int RENDER_H = 480;

const int WINDOW_W = 800;
const int WINDOW_H = 480;

GLuint screenTex;
GLuint computeProgram;
GLuint displayProgram;
GLuint quadVAO;
GLuint skyboxTexID;

void saveImage(const char* filename) {
    std::vector<unsigned char> pixels(RENDER_W * RENDER_H * 3);

    glBindTexture(GL_TEXTURE_2D, screenTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    stbi_flip_vertically_on_write(true);
    if (stbi_write_png(filename, RENDER_W, RENDER_H, 3, pixels.data(), RENDER_W * 3)) {
        std::cout << "IMAGE SAUVEGARDEE : " << filename << std::endl;
    } else {
        std::cerr << "ERREUR sauvegarde" << std::endl;
    }
}


GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID); 

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 

    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 3);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Repeat pour phi
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Erreur  " << path << std::endl;
    }
    return textureID;
}

std::string loadFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERREUR: Impossible de lire " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint createShader(const char* sourceCode, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        exit(1);
    }
    return shader;
}

void initOpenGL() {

    //creating image
    glGenTextures(1, &screenTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RENDER_W, RENDER_H, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


    //shader creation
    std::string csSrc = loadFile("raytracer.glsl");
    GLuint cs = createShader(csSrc.c_str(), GL_COMPUTE_SHADER);
    computeProgram = glCreateProgram();
    glAttachShader(computeProgram, cs);
    glLinkProgram(computeProgram);

    
    //defining rectangle in which we print pixels

    //vertex
    const char* vsCode = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            TexCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
            gl_Position = vec4(TexCoord * 2.0 - 1.0, 0.0, 1.0);
        }
    )";
  
    // our final texture printed on fragment shader
    const char* fsCode = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D screenTexture;
        void main() {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";


    GLuint vs = createShader(vsCode, GL_VERTEX_SHADER);
    GLuint fs = createShader(fsCode, GL_FRAGMENT_SHADER);
    displayProgram = glCreateProgram();
    glAttachShader(displayProgram, vs);
    glAttachShader(displayProgram, fs);
    glLinkProgram(displayProgram);

    glGenVertexArrays(1, &quadVAO);

    skyboxTexID = loadTexture("../assets/milkyway.png");
}

int main() {
    if (!glfwInit()) return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_W, WINDOW_H, "BlackHole Linux GPU", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0); // Fps

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "Erreur " << std::endl; return -1; }

    initOpenGL();

    float camR = 6.0f;
    float camTheta = 1.37f; 
    float camPhi = 2.6f;    
    float viewYaw = 3.107012f;   
    float viewPitch = 0.088906f;
    float fov = 1.0f;     
    double lastX = WINDOW_W / 2.0;
    double lastY = WINDOW_H / 2.0;
    bool firstMouse = true; 
    float sensitivity = 0.005f; 

    int frameCount = 0; 

    GLint camPosLoc = glGetUniformLocation(computeProgram, "camPos");
    GLint viewAnglesLoc = glGetUniformLocation(computeProgram, "viewAngles");
    GLint timeLoc = glGetUniformLocation(computeProgram, "time");
    GLint fovLoc = glGetUniformLocation(computeProgram, "fov");

    bool mouse = false;
    int count = 0;
    bool keyboard = true;
    while (!glfwWindowShouldClose(window) && count <= 600) {
    

        if(keyboard) {
            if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
                fov -= 0.1f;
                if (fov < 0.1f) fov = 0.1f; 
            }
            if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
                fov += 0.1f;
                if (fov > 2.0f) fov = 2.0f; 
            }
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){         

                camR-=0.1;
                
            }

            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){

                camR+=0.1;
            }
            if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){

                camTheta+=0.1;
            }

            if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){

                camTheta-=0.1;
            }

            if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){

                camPhi+=0.1;
            }

            if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){

                camPhi-=0.1;
            }
        }
        if(mouse) {

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
    
            if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }
    
            float xoffset = (float)(xpos - lastX);
            float yoffset = (float)(ypos - lastY); 
            
            lastX = xpos;
            lastY = ypos;
    
       
            viewYaw += xoffset * sensitivity;
            viewPitch += yoffset * sensitivity;
            if (viewPitch > 1.5f) viewPitch = 1.5f;
            if (viewPitch < -1.5f) viewPitch = -1.5f;

       
            
                
            printf("camR : %f\n", camR);
            printf("camTheta : %f\n", camTheta);
            printf("camR : %f\n", camPhi);
            printf("viewPitch : %f\n", viewPitch);
            printf("viewYaw : %f\n", viewYaw);
            printf("fov : %f\n", fov);

            
        }
        else {
    
            float time = glfwGetTime();
            float speed = 0.000015f;
            printf("camphi avant : %f\n", camPhi);

            camPhi = camPhi + time * speed;
            printf("camphi après : %f\n", camPhi);


        }


        glUseProgram(computeProgram);

        glUniform1f(fovLoc, fov);
        glUniform1f(timeLoc, (float)glfwGetTime());
        glUniform3f(camPosLoc, camR, camTheta, camPhi);
        glUniform2f(viewAnglesLoc, viewYaw, viewPitch);
        glUniform1i(glGetUniformLocation(computeProgram, "skybox"), 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyboxTexID);

        
        
        glDispatchCompute((RENDER_W + 7) / 8, (RENDER_H + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glViewport(0, 0, WINDOW_W, WINDOW_H); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(displayProgram);
        
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, screenTex);
        
        glBindVertexArray(quadVAO); // VAO vide
        glDrawArrays(GL_TRIANGLES, 0, 3);

        std::string filename = "../output_gpu/render_" + std::to_string(frameCount++) + ".png";
        saveImage(filename.c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
        count+=1;
    }

    glfwTerminate();
    return 0;
}
