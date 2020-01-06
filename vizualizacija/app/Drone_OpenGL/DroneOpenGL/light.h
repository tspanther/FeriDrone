#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Light
{
public:
    Light();
    glm::vec3 lpos, color;
};

#endif // LIGHT_H
