#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>


class Camera
{
public:
    Camera();
    ~Camera();

    glm::vec3 camPos;
    glm::vec3 lookAt;
    glm::vec3 camUp;
    float pitch;
    float yaw;
    float roll;

    double zoom; // factor of zoom:: we translate camera just before generating PVM by (zoom * normalize(lookAt))
    bool lockToThirdPersonCamera;

    void updateUpVec();
    void updateLookAt();
    void setDefaults();
};

#endif // CAMERA_H
