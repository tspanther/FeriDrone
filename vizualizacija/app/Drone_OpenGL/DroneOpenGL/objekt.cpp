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
#include "Source/OBJ_Loader.h"

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

    std::string inputfile = objFile.toStdString();

    // Load OBJ file with OBJ_Loader library.
    objl::Loader loader;
    bool out = loader.LoadFile(inputfile);

    // Go through each loaded mesh and out its contents
    for (int i = 0; i < loader.LoadedMeshes.size(); i++)
    {
        // Copy one of the loaded meshes to be our current mesh
        objl::Mesh curMesh = loader.LoadedMeshes[i];

        // Go through each vertex and print its number,
        //  position, normal, and texture coordinate
        for (int j = 0; j < curMesh.Vertices.size(); j++)
        {
            data.push_back(curMesh.Vertices[j].Position.X);data.push_back(curMesh.Vertices[j].Position.Y);data.push_back(curMesh.Vertices[j].Position.Z);
            data.push_back(curMesh.Vertices[j].Normal.X);data.push_back(curMesh.Vertices[j].Normal.Y);data.push_back(curMesh.Vertices[j].Normal.Z);
            data.push_back(curMesh.Vertices[j].TextureCoordinate.X);data.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
        }
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
