#ifndef OBJEKT_H
#define OBJEKT_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>

enum lepjenjeTeksture{
    izDatoteke, ravninsko, cilindricno, sfericno, stoplt
};

enum smerLepljenja{
    x, y, z, stopsl
};


class Object {
public:
    Object(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, lepjenjeTeksture lt, smerLepljenja sl);

    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program);

    ~Object();

    glm::vec3 pos;
    double pitch;
    double yaw;
    double roll;

    glm::vec3 offset;
    double rollo, pitcho, yawo;

    double scale;

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
