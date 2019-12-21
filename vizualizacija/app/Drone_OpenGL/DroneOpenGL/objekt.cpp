#include "objekt.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <QImage>

Object::Object(QOpenGLFunctions_3_3_Core *gl_in, QString objFile, QString texFile) {
    gl = gl_in;

    loadObj(objFile);
    loadTexture(texFile);

    gl->glGenVertexArrays(1, &VAO);
    gl->glBindVertexArray(VAO);
    gl->glGenBuffers(1, &VBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    gl->glEnableVertexAttribArray(2);
}

void Object::draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program) {
    glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, glm::vec3(objx0, objy0, objz0)); // object move
    M = glm::rotate_slow(M, glm::radians(float(objrotX)), glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate_slow(M, glm::radians(float(objrotY)), glm::vec3(0, 1, 0));
    M = glm::rotate_slow(M, glm::radians(float(objrotZ)), glm::vec3(0, 0, 1));
    M = glm::scale_slow(M, glm::vec3(scale, scale, scale));
    glm::mat4 PVM = P * V * M;


    gl->glBindVertexArray(VAO);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data.size() * sizeof(float));
}

Object::~Object(){
    gl->glDeleteVertexArrays(1,&VAO);
}

void Object::loadObj(QString objFile){
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> vertices;

    std::ifstream file(objFile.toStdString());
    if (file.is_open()) {
        std::string opt;
        while (file >> opt) {
            switch(opt[0]){
            case 'v':
                if (opt[1] == '\0') {
                    float xv, yv, zv;
                    file >> xv >> yv >> zv;
                    vertices.push_back(glm::vec3(xv, yv, zv));
                } else if (opt[1] == 't') {
                    float xt, yt;
                    file >> xt >> yt;
                    textureCoords.push_back(glm::vec2(xt, yt));
                } else if (opt[1] == 'n') {
                    float xn, yn, zn;
                    file >> xn >> yn >> zn;
                    normals.push_back(glm::vec3(xn, yn, zn));
                }
                break;
            case 'f':
                for (int i = 0; i < 3; i++) {
                    char temp[256];
                    file >> temp;

                    unsigned int v, t, n;
                    int matches = sscanf(temp, "%d/%d/%d", &v, &t, &n);
                    if (matches != 3){
                        printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                        return;
                    }
                    v--; t--; n--;

                    data.push_back(vertices[v].x); data.push_back(vertices[v].y); data.push_back(vertices[v].z);
                    data.push_back(normals[n].x); data.push_back(normals[n].y); data.push_back(normals[n].z);
                    data.push_back(textureCoords[t].x); data.push_back(textureCoords[t].y);
                }

                break;
            }
        }
        file.close();
    }
    else {
        std::cout << "Obj file not found." << std::endl;
    }
}

void Object::loadTexture(QString texFile){
    QImage img;
    img.load(texFile);
    img = img.convertToFormat(QImage::Format_RGBA8888);

    gl->glGenTextures(1, &tex_id);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
