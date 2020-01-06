#ifndef OBJEKT_H
#define OBJEKT_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include <light.h>

class Object {
public:
    Object(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, Light* light);
    ~Object();

    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program, glm::vec3 camPos);

    glm::vec3 pos;
    float pitch;
    float yaw;
    float roll;

    double scale;

    // material
    glm::vec3 Ra = 3.5f * glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 Rd = 3.0f * glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 Rs = 2.0f * glm::vec3(0.5f, 0.5f, 0.5f);
    GLint ns = 200;

    Light* light;
protected:
    void loadTexture(const char* texFile, GLuint* texture_id);
    void loadObj(const char* objFile, std::vector<float>* datav);

    QOpenGLFunctions_3_3_Core* gl;
    GLuint VBO;
    GLuint VAO;
    GLuint tex_id;
    std::vector<float> data;
};

#endif // OBJEKT_H
