#include "camera.h"
#include <glm/ext.hpp>

Camera::Camera(){
    zoom = 0.0;
    camPos = glm::vec3(0.0, 0.0, 3.0);
    lookAt = glm::vec3(0.0, 0.0, -1.0);
    camUp = glm::vec3(0.0, 1.0, 0.0);
    pitch = 0.0;
    yaw = -glm::pi<double>() / 2;
    roll = glm::pi<double>() / 2;
}

Camera::~Camera(){}

void Camera::updateUpVec(){
    camUp = glm::vec3(glm::cos(roll), glm::sin(roll), 0.0);
}

void Camera::updateLookAt(){
    glm::vec3 front;
    front.x = cos(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    lookAt = glm::normalize(front);
}

// sets defaults for 3rd person camera (only call on 3rd person)
void Camera::setDefaults(){
    zoom = 0.0;
    camPos = glm::vec3(0.0, 0.0, 3.0);
    lookAt = glm::vec3(0.0, 0.0, -1.0);
    camUp = glm::vec3(0.0, 1.0, 0.0);
    pitch = 0.0;
    yaw = -glm::pi<double>() / 2;
    roll = glm::pi<double>() / 2;
}
