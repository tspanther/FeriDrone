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

    // material
    glm::vec3 Ra = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 Rd = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 Rs = glm::vec3(0.5f, 0.5f, 0.5f);
    GLint ns = 64;

    // to ne sodi sem - to bi moral passat preko metode draw kot argument;; pa bi moral dosti spreminjat druge kode v tem primeru
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(1.0f, 5.0f, 7.0f);
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);

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
