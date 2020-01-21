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

Object::Object(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, Light* light_in) {
    gl = gl_in;

    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    roll = 0;
    pitch = 0;
    yaw = 0;

    light = light_in;

    scale = 1.0;

    loadObj(objFile, &data);
    loadTexture(texFile, &tex_id);

/*
 *  Triangle test
 *
    data.clear();
    std::vector<float> a = {
            -10.0, -10.0, -5.0, 1.0, 1.0, 1.0, 0.1, 0.1,
            0.0, 10.0, -5.0, 1.0, 1.0, 1.0, 0.1, 0.1,
          10.0, -10.0, -5.0, 1.0, 1.0, 1.0, 0.1, 0.1
    };

    for (unsigned int i = 0; i< 8*3; i++){
        data.push_back(a[i]);
    }
*/

    gl->glGenVertexArrays(1, &VAO);
    gl->glBindVertexArray(VAO);
    gl->glGenBuffers(1, &VBO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->glBufferData(GL_ARRAY_BUFFER, static_cast<int>(data.size() * sizeof(float)), data.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>((3 * sizeof(float))));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>((6 * sizeof(float))));
    gl->glEnableVertexAttribArray(2);
}

void Object::draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program, glm::vec3 camPos) {
    glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, pos); // object move
    M = glm::rotate(M, yaw, glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate(M, pitch, glm::vec3(0, 1, 0));
    M = glm::rotate(M, roll, glm::vec3(0, 0, 1));
    M = glm::scale(M, glm::vec3(scale, scale, scale));
    glm::mat4 PVM = P * V * M;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(M));

    gl->glBindVertexArray(VAO);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "M"), 1, GL_FALSE, glm::value_ptr(M));
    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
    gl->glUniform3fv(gl->glGetUniformLocation(id_shader_program, "lightColor"), 1, glm::value_ptr(light->color));
    gl->glUniform3fv(gl->glGetUniformLocation(id_shader_program, "lightPos"), 1, glm::value_ptr(light->lpos));
    gl->glUniform3fv(gl->glGetUniformLocation(id_shader_program, "campPos"), 1, glm::value_ptr(camPos));
    gl->glUniform3fv(gl->glGetUniformLocation(id_shader_program, "Ra"), 1, glm::value_ptr(Ra));
    gl->glUniform3fv(gl->glGetUniformLocation(id_shader_program, "Rd"), 1, glm::value_ptr(Rd));
    gl->glUniform3fv(gl->glGetUniformLocation(id_shader_program, "Rs"), 1, glm::value_ptr(Rs));
    gl->glUniform1i(gl->glGetUniformLocation(id_shader_program, "ns"), ns);
    gl->glUniform1i(gl->glGetUniformLocation(id_shader_program, "activateShading"), 1);

    gl->glDrawArrays(GL_TRIANGLES, 0,static_cast<int>(data.size() * sizeof(float)) );
}

Object::~Object(){
    gl->glDeleteVertexArrays(1,&VAO);
}

void Object::loadTexture(const char* texFile, GLuint* texture_id){
    QImage img;
    img.load(texFile);
    img = img.convertToFormat(QImage::Format_RGBA8888);

    gl->glGenTextures(1, texture_id);
    gl->glBindTexture(GL_TEXTURE_2D, *texture_id);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

    gl->glGenerateMipmap(GL_TEXTURE_2D);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Object::loadObj(const char* objFile, std::vector<float>* datav){
    // Load OBJ file with OBJ_Loader library.
    objl::Loader loader;
    bool out = loader.LoadFile(objFile);

    // Go through each loaded mesh and out its contents
    for (unsigned int i = 0; i < loader.LoadedMeshes.size(); i++)
    {
        // Copy one of the loaded meshes to be our current mesh
        objl::Mesh curMesh = loader.LoadedMeshes[i];

        // Go through each vertex and print its number,
        //  position, normal, and texture coordinate
        for (unsigned int j = 0; j < curMesh.Vertices.size(); j++)
        {
            datav->push_back(curMesh.Vertices[j].Position.X);          datav->push_back(curMesh.Vertices[j].Position.Y);          datav->push_back(curMesh.Vertices[j].Position.Z);
            datav->push_back(curMesh.Vertices[j].Normal.X);            datav->push_back(curMesh.Vertices[j].Normal.Y);            datav->push_back(curMesh.Vertices[j].Normal.Z);
            datav->push_back(curMesh.Vertices[j].TextureCoordinate.X); datav->push_back(curMesh.Vertices[j].TextureCoordinate.Y);
        }
    }

}
