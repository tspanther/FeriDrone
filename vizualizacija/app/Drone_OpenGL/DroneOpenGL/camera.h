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
    double pitch;
    double yaw;
    double roll;
    // factor of zoom;;
    // we translate camera just before generating PVM by (zoom * normalize(lookAt))
    double zoom;

    void updateUpVec();
    void updateLookAt();
    void setDefaults();
};

#endif // CAMERA_H
