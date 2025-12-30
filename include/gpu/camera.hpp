#include "commons.hpp"
#include "opengl.hpp"

struct Camera {

    Camera(float camR ,float camTheta ,float camPhi , float viewYaw , float viewPitch , float fov , double lastX , double lastY , bool firstMouse , float sensitivity ) : camR(camR) ,camTheta(camTheta) ,camPhi(camPhi) , viewYaw(viewYaw) , viewPitch(viewPitch) , fov(fov) ,  lastX(lastX) ,  lastY(lastY) ,  firstMouse(firstMouse) , sensitivity(sensitivity)  {

    }  


    float camR;
    float camTheta;
    float camPhi;
    float viewYaw;   
    float viewPitch;
    float fov ;     
    double lastX;
    double lastY;
    bool firstMouse;
    float sensitivity;

    void mouseMoveCam(OpenGL &opengl);
    void autoMoveCam();
    void handleKeyboard(OpenGL &opengl);

    
  
};
