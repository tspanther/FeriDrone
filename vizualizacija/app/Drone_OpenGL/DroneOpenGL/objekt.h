#ifndef OBJEKT_H
#define OBJEKT_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>


class Object {
public:
    Object(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile);

    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program);

    ~Object();

    float objx0;
    float objy0;
    float objz0;
    double pitch;
    double yaw;
    double roll;
    double scale;

private:
    void loadTexture(const char* texFile);
    void loadObj(const char* objFile);

    QOpenGLFunctions_3_3_Core* gl;
    GLuint VBO;
    GLuint VAO;
    GLuint tex_id;
    std::vector<float> data;
};

#endif // OBJEKT_H
