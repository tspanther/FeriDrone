#ifndef DRONE_H
#define DRONE_H

#include <objekt.h>
#include <camera.h>

class drone : public Object
{
public:
    drone(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, const char* objFileArrow, const char* texFileArrow_1, const char* texFileArrow_2, const char* texFileArrow_3);
    ~drone();
    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program);
    void moveTo(glm::vec3 vec);
    void tiltTo(double roll, double pitch, double yaw);
    // 1stP camera
    Camera cam;
private:
    glm::vec3 prevLocation;

    // convenience
    glm::vec3 lookAt;
    glm::vec3 upVec;

    // arrows
    float arrScale = 0.1f;

    GLuint VBO_ArrLA;
    GLuint VAO_ArrLA;
    GLuint tex_id_ArrLA;
    std::vector<float> data_ArrLA;

    GLuint VBO_ArrUV;
    GLuint VAO_ArrUV;
    GLuint tex_id_ArrUV;
    std::vector<float> data_ArrUV;

    GLuint VBO_ArrVEL;
    GLuint VAO_ArrVEL;
    GLuint tex_id_ArrVEL;
    std::vector<float> data_ArrVEL;
};

#endif // DRONE_H
