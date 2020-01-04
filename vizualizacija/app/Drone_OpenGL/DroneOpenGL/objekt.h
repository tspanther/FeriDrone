#ifndef OBJEKT_H
#define OBJEKT_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include <light.h>

enum lepjenjeTeksture{
    izDatoteke, ravninsko, cilindricno, sfericno, stoplt
};

enum smerLepljenja{
    x, y, z, stopsl
};


class Object {
public:
    Object(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, lepjenjeTeksture lt, smerLepljenja sl, Light* light_);

    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program, glm::vec3 camPos);

    ~Object();

    glm::vec3 pos;
    double pitch;
    double yaw;
    double roll;

    glm::vec3 offset;
    double rollo, pitcho, yawo;

    double scale;

    // material
    glm::vec3 Ra = 1.5f * glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 Rd = 5.0f * glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 Rs = 2.0f * glm::vec3(0.5f, 0.5f, 0.5f);
    GLint ns = 200;

    Light* light;

protected:
    void loadTexture(const char* texFile, GLuint* texture_id);
    void loadObj(const char* objFile, std::vector<float>* datav, lepjenjeTeksture lt, smerLepljenja sl);

    QOpenGLFunctions_3_3_Core* gl;
    GLuint VBO;
    GLuint VAO;
    GLuint tex_id;
    std::vector<float> data;
};

#endif // OBJEKT_H
