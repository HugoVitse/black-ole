#include "camera.hpp"

void Camera::mouseMoveCam(OpenGL &opengl) {

    double xpos, ypos;
    glfwGetCursorPos(opengl.window, &xpos, &ypos);

    if (this->firstMouse) {
        this->lastX = xpos;
        this->lastY = ypos;
        this->firstMouse = false;
    }

    float xoffset = (float)(xpos - this->lastX);
    float yoffset = (float)(ypos - this->lastY); 
    
    this->lastX = xpos;
    this->lastY = ypos;


    this->viewYaw += xoffset * this->sensitivity;
    this->viewPitch += yoffset * this->sensitivity;
    if (this->viewPitch > 1.5f) this->viewPitch = 1.5f;
    if (this->viewPitch < -1.5f) this->viewPitch = -1.5f;


    
        
    printf("camera.camR : %f\n", this->camR);
    printf("camTheta : %f\n", this->camTheta);
    printf("camera.camR : %f\n", this->camPhi);
    printf("viewPitch : %f\n", this->viewPitch);
    printf("viewYaw : %f\n",this->viewYaw);
    printf("fov : %f\n", this->fov);
}


void Camera::autoMoveCam(){
        
    float time = glfwGetTime();
    float speed = 0.000015f;
    printf("camphi avant : %f\n", this->camPhi);

    this->camPhi = this->camPhi + time * speed;
    printf("camphi après : %f\n", this->camPhi);
}

void Camera::handleKeyboard(OpenGL &opengl){


    if(glfwGetKey(opengl.window, GLFW_KEY_UP) == GLFW_PRESS){
        this->fov -= 0.1f;
        if (this->fov < 0.1f) this->fov = 0.1f; 
    }
    if(glfwGetKey(opengl.window, GLFW_KEY_DOWN) == GLFW_PRESS){
        this->fov += 0.1f;
        if (this->fov > 2.0f) this->fov = 2.0f; 
    }
    if(glfwGetKey(opengl.window, GLFW_KEY_W) == GLFW_PRESS){         

        this->camR-=0.1;
        
    }

    if(glfwGetKey(opengl.window, GLFW_KEY_S) == GLFW_PRESS){

        this->camR+=0.1;
    }
    if(glfwGetKey(opengl.window, GLFW_KEY_D) == GLFW_PRESS){

        this->camTheta+=0.1;
    }

    if(glfwGetKey(opengl.window, GLFW_KEY_A) == GLFW_PRESS){

        this->camTheta-=0.1;
    }

    if(glfwGetKey(opengl.window, GLFW_KEY_LEFT) == GLFW_PRESS){

        this->camPhi+=0.1;
    }

    if(glfwGetKey(opengl.window, GLFW_KEY_RIGHT) == GLFW_PRESS){

        this->camPhi-=0.1;
    }

    
}