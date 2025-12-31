#include "opengl.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

std::string OpenGL::loadFile(const char* filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERREUR: Impossible de lire " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint OpenGL::createShader(const char* sourceCode, GLenum type){
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "------- ERREUR SHADER -------" << std::endl;
        std::cerr << log << std::endl;
        std::cerr << "-----------------------------" << std::endl;
        exit(1);
    }
    return shader;
}


GLuint OpenGL::createComputeShader(const std::vector<std::string> &sources, GLenum type) {
    GLuint shader = glCreateShader(type);

    std::vector<const char*> char_ptrs;
    for (const auto& source : sources) {
        char_ptrs.push_back(source.c_str());
    }

 
    glShaderSource(shader, char_ptrs.size(), char_ptrs.data(), NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "------- ERREUR SHADER -------" << std::endl;
        std::cerr << log << std::endl;
        std::cerr << "-----------------------------" << std::endl;
        exit(1);
    }
    return shader;
}


GLuint OpenGL::loadTexture(const char* path) {
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


void OpenGL::saveImage(const char* filename) {
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

void OpenGL::initOpenGL() {

    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    this->window = glfwCreateWindow(WINDOW_W, WINDOW_H, "BlackHole Linux GPU", NULL, NULL);
    if (!this->window) { glfwTerminate(); return ; }
    glfwMakeContextCurrent(this->window);
    
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0); // Fps
    
    glewExperimental = GL_TRUE;
    glewInit();

    glGenTextures(1, &this->screenTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RENDER_W, RENDER_H, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindImageTexture(0, this->screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


    //shader creation

    std::string version = "#version 430 core\n";

    
    std::string blackHoleCode = loadFile("blackhole.glsl");
    std::string accretionDiskCode = loadFile("accretionDisk.glsl");

    std::string tetradeCode = loadFile("tetrade.glsl");
    std::string pixelCode = loadFile("pixel.glsl");

    std::string photonCode = loadFile("photon.glsl"); 
    std::string mainCode = loadFile("raytracer.glsl"); 
    std::string metricCode = loadFile("metric.glsl");


    std::string variablesCode = loadFile("variables.glsl");
    std::string realMaincode = loadFile("main.glsl");


    const std::vector<std::string> sources = {
        version,
        blackHoleCode,
        tetradeCode,
        metricCode,
        photonCode,
        pixelCode,
        accretionDiskCode,
        variablesCode,
        realMaincode
    };

    GLuint cs = this->createComputeShader(sources,GL_COMPUTE_SHADER);

    
    this->computeProgram = glCreateProgram();
    glAttachShader(this->computeProgram, cs);
    glLinkProgram(this->computeProgram);

    
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
    this->displayProgram = glCreateProgram();
    glAttachShader(this->displayProgram, vs);
    glAttachShader(this->displayProgram, fs);
    glLinkProgram(this->displayProgram);

    glGenVertexArrays(1, &this->quadVAO);

    this->skyboxTexID = loadTexture("../assets/milkyway.png");


 
}