#include "drone.h"

drone::drone(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, const char* objFileArrow, const char* texFileArrow) : Object(gl_in, objFile, texFile) {
    Object::loadObj(objFileArrow, &data_ArrLA);
    Object::loadObj(objFileArrow, &data_ArrUV);
    Object::loadObj(objFileArrow, &data_ArrVEL);
    Object::loadTexture(texFileArrow, &tex_id_ArrLA);
    Object::loadTexture(texFileArrow, &tex_id_ArrUV);
    Object::loadTexture(texFileArrow, &tex_id_ArrVEL);

    gl->glGenVertexArrays(1, &VAO_ArrLA);
    gl->glBindVertexArray(VAO_ArrLA);
    gl->glGenBuffers(1, &VBO_ArrLA);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrLA);
    gl->glBufferData(GL_ARRAY_BUFFER, data_ArrLA.size() * sizeof(float), data_ArrLA.data(), GL_STATIC_DRAW);

    gl->glGenVertexArrays(1, &VAO_ArrUV);
    gl->glBindVertexArray(VAO_ArrUV);
    gl->glGenBuffers(1, &VBO_ArrUV);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrUV);
    gl->glBufferData(GL_ARRAY_BUFFER, data_ArrUV.size() * sizeof(float), data_ArrUV.data(), GL_STATIC_DRAW);

    gl->glGenVertexArrays(1, &VAO_ArrVEL);
    gl->glBindVertexArray(VAO_ArrVEL);
    gl->glGenBuffers(1, &VBO_ArrVEL);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrVEL);
    gl->glBufferData(GL_ARRAY_BUFFER, data_ArrVEL.size() * sizeof(float), data_ArrVEL.data(), GL_STATIC_DRAW);
}

void drone::draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program) {
    Object::draw(P, V, id_shader_program);

    // lookAt Arrow
    glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, glm::vec3(objx0, objy0, objz0)); // object move
    M = glm::rotate_slow(M, (float)yaw, glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate_slow(M, (float)pitch, glm::vec3(0, 1, 0));
    M = glm::rotate_slow(M, (float)roll, glm::vec3(0, 0, 1));
    M = glm::scale_slow(M, glm::vec3(scale, scale, scale));
    glm::mat4 PVM = P * V * M;

    gl->glBindVertexArray(VAO_ArrLA);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_ArrLA);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrLA);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_ArrLA.size() * sizeof(float));

    // upVec Arrow
    M = glm::mat4(1);
    M = glm::translate(M, glm::vec3(objx0, objy0, objz0)); // object move
    M = glm::rotate_slow(M, 90.0f, glm::vec3(0, 1, 0)); // object rotate
    M = glm::rotate_slow(M, (float)yaw, glm::vec3(0, 0, 1));
    M = glm::rotate_slow(M, (float)pitch, glm::vec3(0, 1, 0));
    M = glm::rotate_slow(M, (float)roll, glm::vec3(1, 0, 0));
    M = glm::scale_slow(M, glm::vec3(scale, scale, scale));
    PVM = P * V * M;

    gl->glBindVertexArray(VAO_ArrUV);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_ArrUV);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrUV);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_ArrUV.size() * sizeof(float));

    /*
     * todo: poracunat vektor hitrosti, ga izrisat
    */
}
