#ifndef OBJEKT_H
#define OBJEKT_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>


class Object {
public:
    Object(QOpenGLFunctions_3_3_Core *gl_in, QString objFile, QString texFile);

    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program);

    ~Object();

    double objx0;
    double objy0;
    double objz0;
    int objrotX;
    int objrotY;
    int objrotZ;
    double scale;

private:
    void loadTexture(QString texFile);
    void loadObj(QString objFile);

    QOpenGLFunctions_3_3_Core* gl;
    GLuint VBO;
    GLuint VAO;
    GLuint tex_id;
    std::vector<float> data;
};

#endif // OBJEKT_H
