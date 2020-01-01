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

Object::Object(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, lepjenjeTeksture lt, smerLepljenja sl) {
    gl = gl_in;

    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    roll = 0;
    pitch = 0;
    yaw = 0;

    offset = glm::vec3(0.0f, 0.0f, 0.0f);
    rollo = 0.0;
    pitcho = 0.0;
    yawo = 0.0;

    scale = 1.0;

    loadObj(objFile, &data, lt, sl);
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
    M = glm::translate(M, pos + offset); // object move
    M = glm::rotate(M, float(yaw + yawo), glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate(M, float(pitch + pitcho), glm::vec3(0, 1, 0));
    M = glm::rotate(M, float(roll + rollo), glm::vec3(0, 0, 1));
    M = glm::scale(M, glm::vec3(scale, scale, scale));
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

float vlen(glm::vec3 vec){
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

void Object::loadObj(const char* objFile, std::vector<float>* datav, lepjenjeTeksture lt, smerLepljenja sl){
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
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

        for (unsigned int j = 0; j < curMesh.Vertices.size(); j++)
        {
            if (curMesh.Vertices[j].Position.X < min.x){
                min.x = curMesh.Vertices[j].Position.X;
            }
            if (curMesh.Vertices[j].Position.X > max.x){
                max.x = curMesh.Vertices[j].Position.X;
            }
            if (curMesh.Vertices[j].Position.Y < min.y){
                min.y = curMesh.Vertices[j].Position.Y;
            }
            if (curMesh.Vertices[j].Position.Y > max.y){
                max.y = curMesh.Vertices[j].Position.Y;
            }
            if (curMesh.Vertices[j].Position.Z < min.z){
                min.z = curMesh.Vertices[j].Position.Z;
            }
            if (curMesh.Vertices[j].Position.Z > max.z){
                max.z = curMesh.Vertices[j].Position.Z;
            }
        }

        glm::vec3 d = glm::vec3(max.x - min.x, max.y - min.y, max.z - min.z);

        for (unsigned int j = 0; j < curMesh.Vertices.size(); j++)
        {
            datav->push_back(curMesh.Vertices[j].Position.X);          datav->push_back(curMesh.Vertices[j].Position.Y);          datav->push_back(curMesh.Vertices[j].Position.Z);
            datav->push_back(curMesh.Vertices[j].Normal.X);            datav->push_back(curMesh.Vertices[j].Normal.Y);            datav->push_back(curMesh.Vertices[j].Normal.Z);

            float s, t;
            glm::vec3 p = glm::vec3(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z);

            if (lt == lepjenjeTeksture::izDatoteke){
                s = curMesh.Vertices[j].TextureCoordinate.X;
                t = curMesh.Vertices[j].TextureCoordinate.Y;
            } else if(lt == lepjenjeTeksture::ravninsko){
                if (sl == smerLepljenja::x){
                    s = (p.z - min.z) / d.z;
                    t = (p.y - min.y) / d.y;
                } else if (sl == smerLepljenja::y){
                    s = (p.x - min.x) / d.x;
                    t = (p.z - min.z) / d.z;
                } else if (sl == smerLepljenja::z){
                    s = (p.x - min.x) / d.x;
                    t = (p.y - min.y) / d.y;
                }
            } else if (lt == lepjenjeTeksture::cilindricno){
                glm::vec3 c = min + 0.5f * d;
                glm::vec3 v = p - c;
                float theta;

                if (sl == smerLepljenja::x){
                    theta = atan2(v.z, v.y)  * 180.0f / glm::pi<float>() + 180.0f;
                    t = v.x / d.x + 0.5f;
                } else if (sl == smerLepljenja::y){
                    theta = atan2(v.x, v.z)  * 180.0f / glm::pi<float>() + 180.0f;
                    t = v.y / d.y + 0.5f;
                } else if (sl == smerLepljenja::z){
                    theta = atan2(v.x, v.y)  * 180.0f / glm::pi<float>() + 180.0f;
                    t = v.z / d.z + 0.5f;
                }

                s = theta / 360.0f;
            } else if (lt == lepjenjeTeksture::sfericno){
                glm::vec3 c = min + 0.5f * d;
                glm::vec3 v = p - c;
                float theta, rho;

                if (sl == smerLepljenja::x){
                    theta = atan2(v.z, v.y) * 180.0f / glm::pi<float>() + 180.0f;
                    rho = asin(v.x / vlen(v)) * 180.0f / glm::pi<float>();
                } else if (sl == smerLepljenja::y){
                    theta = atan2(v.x, v.z) * 180.0f / glm::pi<float>() + 180.0f;
                    rho = asin(v.y / vlen(v)) * 180.0f / glm::pi<float>();
                } else if (sl == smerLepljenja::z){
                    theta = atan2(v.x, v.y) * 180.0f / glm::pi<float>() + 180.0f;
                    rho = asin(v.z / vlen(v)) * 180.0f / glm::pi<float>();
                }

                s = theta / 360.0f;
                t = rho / 180.0f + 0.5f;
            }

            datav->push_back(s); datav->push_back(t);
        }
    }
}
