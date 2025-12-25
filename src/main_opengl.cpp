#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>

const int RENDER_W = 4096;
const int RENDER_H = 2160;

const int WINDOW_W = 1280;
const int WINDOW_H = 720;

GLuint screenTex;
GLuint computeProgram;
GLuint displayProgram;
GLuint quadVAO;
GLuint skyboxTexID;

void saveImage(const char* filename) {
    // 1. On alloue de la mémoire CPU pour recevoir l'image (W * H * 3 canaux RGB)
    std::vector<unsigned char> pixels(RENDER_W * RENDER_H * 3);

    glBindTexture(GL_TEXTURE_2D, screenTex);
    // 2. On configure OpenGL pour aligner les bytes correctement
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // 4. Flip et écriture (inchangé)
    stbi_flip_vertically_on_write(true);
    if (stbi_write_png(filename, RENDER_W, RENDER_H, 3, pixels.data(), RENDER_W * 3)) {
        std::cout << "IMAGE 4K SAUVEGARDEE : " << filename << std::endl;
    } else {
        std::cerr << "ERREUR sauvegarde" << std::endl;
    }
}

void MyKeyboardFunc(unsigned char Key, int x, int y)
{
 
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // CORRECTION: Utiliser GL_TEXTURE_2D au lieu de 1
    glBindTexture(GL_TEXTURE_2D, textureID); 

    int width, height, nrChannels;
    // ASTUCE: Retourner l'image pour qu'elle soit dans le bon sens (v de 0 à 1)
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
        std::cout << "Texture chargee: " << path << " (" << width << "x" << height << ")" << std::endl;
    } else {
        std::cout << "Erreur chargement texture : " << path << std::endl;
    }
    return textureID;
}

// Lecture de fichier
std::string loadFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERREUR: Impossible de lire " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::cout << "--- DEBUG: CONTENU DU SHADER LU ---" << std::endl;
    std::cout << content.substr(0, 100) << "..." << std::endl; // Affiche les 100 premiers caractères
    std::cout << "-----------------------------------" << std::endl;
    return buffer.str();
}

// Compilation d'un shader (Compute, Vertex ou Fragment)
GLuint createShader(const char* sourceCode, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "ERREUR SHADER (" << (type == GL_COMPUTE_SHADER ? "COMPUTE" : "GRAPHIC") << "):\n" << log << std::endl;
        exit(1);
    }
    return shader;
}

void initOpenGL() {
    // 1. Texture de sortie (Ecran du Raytracer)
    glGenTextures(1, &screenTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, RENDER_W, RENDER_H, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // 2. Compute Shader (Le Raytracer)
    std::string csSrc = loadFile("raytracer.glsl");
    GLuint cs = createShader(csSrc.c_str(), GL_COMPUTE_SHADER);
    computeProgram = glCreateProgram();
    glAttachShader(computeProgram, cs);
    glLinkProgram(computeProgram);

    // 3. Display Shader (Pour afficher la texture sur un quad plein écran)
    // Vertex Shader simple
    const char* vsCode = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    // Fragment Shader simple
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

    // 4. Géométrie du Quad (2 triangles couvrant l'écran)
    float vertices[] = {
        // positions    // uv
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,

        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    unsigned int VBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    skyboxTexID = loadTexture("../assets/milkyway.png");
}

int main() {
    if (!glfwInit()) return -1;
    
    // Configuration OpenGL 4.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_W, WINDOW_H, "BlackHole Linux GPU", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSwapInterval(0); // FPS illimités

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr << "Erreur GLEW" << std::endl; return -1; }

    initOpenGL();
    std::cout << "Moteur GPU initialise sur Linux." << std::endl;

    // --- SOURIS 2 : Variables d'état ---
    float camR = 6.0f;
    float camTheta = 1.37f; // PI/2 (Vue de face)
    float camPhi = 2.6f;    // Angle horizontal

    float viewYaw = 3.107012f;   // Gauche/Droite
    float viewPitch = 0.088906f; // Haut/Bas

    float fov = 1.0f;
        
    // Pour calculer le mouvement de la souris
    double lastX = WINDOW_W / 2.0;
    double lastY = WINDOW_H / 2.0;
    bool firstMouse = true; // Pour éviter un saut brusque au démarrage
    float sensitivity = 0.005f; // Vitesse de la souris

    int screenshotCount = 0; // Pour nommer les fichiers img_0.png, img_1.png...
  
    GLint camPosLoc = glGetUniformLocation(computeProgram, "camPos");
    GLint viewAnglesLoc = glGetUniformLocation(computeProgram, "viewAngles");
    GLint timeLoc = glGetUniformLocation(computeProgram, "time");
    GLint fovLoc = glGetUniformLocation(computeProgram, "fov");

    bool mouse = false;
    int count = 0;
    bool keyboard = true;
    while (!glfwWindowShouldClose(window) && count <= 600) {
        // Dans ta boucle de rendu
      

        if(keyboard) {
            if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
                fov -= 0.1f;
                if (fov < 0.1f) fov = 0.1f; // Max Zoom (Télescope)
            }
            // E pour Dézoomer (augmenter le FOV)
            if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
                fov += 0.1f;
                if (fov > 2.0f) fov = 2.0f; // Max Grand Angle (Fish-eye)
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
    
            // Calcul du décalage
            float xoffset = (float)(xpos - lastX);
            float yoffset = (float)(ypos - lastY); 
            
            lastX = xpos;
            lastY = ypos;
    
       
            viewYaw += xoffset * sensitivity;  // - pour inverser la rotation (naturel)
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
            float speed = 0.000015f; // Vitesse de rotation
            printf("camphi avant : %f\n", camPhi);

            camPhi = camPhi + time * speed;
            printf("camphi après : %f\n", camPhi);

                
            // float dist = 6.0f;
            // float camR = dist;

        }

        printf("time : %f\n", (float)glfwGetTime());
             
  
        glUseProgram(computeProgram);

        glUniform1f(fovLoc, fov);
        glUniform1f(timeLoc, (float)glfwGetTime());

        glUniform3f(camPosLoc, camR, camTheta, camPhi);
        glUniform2f(viewAnglesLoc, viewYaw, viewPitch);

        // Lier la skybox sur l'unité 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyboxTexID);

        
        // On passe l'index 1 au shader
        glUniform1i(glGetUniformLocation(computeProgram, "skybox"), 1);

        // On lance (W/8, H/8) groupes de travail de 8x8 pixels
        glDispatchCompute((RENDER_W + 7) / 8, (RENDER_H + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // --- ETAPE 2 : AFFICHAGE ---
        glViewport(0, 0, WINDOW_W, WINDOW_H); 
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(displayProgram);
        
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0); // On remet le curseur sur l'unité 0
        glBindTexture(GL_TEXTURE_2D, screenTex);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        std::string filename = "../output_gpu/render_" + std::to_string(screenshotCount++) + ".png";
        saveImage(filename.c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
        count+=1;
    }

    glfwTerminate();
    return 0;
}