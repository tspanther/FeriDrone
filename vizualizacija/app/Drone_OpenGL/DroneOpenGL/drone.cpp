#include "drone.h"
#include <iostream>

unsigned int drone::prevLocWindowSize = 5;

drone::drone(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, const char* objFileArrow, const char* texFileArrow_1, const char* texFileArrow_2, const char* texFileArrow_3, const char* texFileTrajectory) : Object(gl_in, objFile, texFile) {
    lookAt = glm::vec3(0.0, 0.0, -1.0);
    upVec = glm::vec3(0.0, 1.0, 0.0);

    Object::loadObj(objFileArrow, &data_ArrLA);
    Object::loadObj(objFileArrow, &data_ArrUV);
    Object::loadObj(objFileArrow, &data_ArrVEL);
    Object::loadTexture(texFileArrow_1, &tex_id_ArrLA);
    Object::loadTexture(texFileArrow_2, &tex_id_ArrUV);
    Object::loadTexture(texFileArrow_3, &tex_id_ArrVEL);
    Object::loadTexture(texFileTrajectory, &tex_id_traj);

    gl->glGenVertexArrays(1, &VAO_ArrLA);
    gl->glBindVertexArray(VAO_ArrLA);
    gl->glGenBuffers(1, &VBO_ArrLA);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrLA);
    gl->glBufferData(GL_ARRAY_BUFFER, data_ArrLA.size() * sizeof(float), data_ArrLA.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    gl->glEnableVertexAttribArray(2);

    gl->glGenVertexArrays(1, &VAO_ArrUV);
    gl->glBindVertexArray(VAO_ArrUV);
    gl->glGenBuffers(1, &VBO_ArrUV);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrUV);
    gl->glBufferData(GL_ARRAY_BUFFER, data_ArrUV.size() * sizeof(float), data_ArrUV.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    gl->glEnableVertexAttribArray(2);

    gl->glGenVertexArrays(1, &VAO_ArrVEL);
    gl->glBindVertexArray(VAO_ArrVEL);
    gl->glGenBuffers(1, &VBO_ArrVEL);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrVEL);
    gl->glBufferData(GL_ARRAY_BUFFER, data_ArrVEL.size() * sizeof(float), data_ArrVEL.data(), GL_STATIC_DRAW);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    gl->glEnableVertexAttribArray(2);

    // Trajectory.
    gl->glGenVertexArrays(1, &VAO_traj);
    gl->glBindVertexArray(VAO_traj);
    gl->glGenBuffers(1, &VBO_traj);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_traj);
    gl->glBufferData(GL_ARRAY_BUFFER, data_traj.size() * sizeof (float), data_traj.data(), GL_STATIC_DRAW);

    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    gl->glEnableVertexAttribArray(3);
}

drone::~drone(){
    gl->glDeleteVertexArrays(1, &VAO);
    gl->glDeleteVertexArrays(1,&VAO_ArrLA);
    gl->glDeleteVertexArrays(1,&VAO_ArrUV);
    gl->glDeleteVertexArrays(1,&VAO_ArrVEL);
    gl->glDeleteVertexArrays(1,&VAO_traj);
}

void drone::moveTo(glm::vec3 vec, unsigned int step){
    prevLocations.push(vec);

    if (prevLocations.size() > prevLocWindowSize){
        prevLocations.pop();

        // trajectory
        if (step % prevLocWindowSize == 0){

            // height
            glm::vec3 previousPosition = prevLocations.front();
            glm::vec3 currentPosition = prevLocations.back();
            std::vector<float> a = {
                previousPosition.x, previousPosition.y, previousPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, alpha,
                currentPosition.x, currentPosition.y, currentPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, alpha,
                previousPosition.x, 0.0, previousPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, alpha,
                currentPosition.x, currentPosition.y, currentPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, alpha,
                previousPosition.x, 0.0, previousPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, alpha,
                currentPosition.x, 0.0, currentPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, alpha
            };

            data_traj.insert(data_traj.end(), a.begin(), a.end());

            // orientation & tilt
            // todo
        }
    }

    pos = vec;

    cam.camPos = pos - 2.0f * lookAt + glm::vec3(0.0f, 1.0f, 0.0f);
}

void drone::tiltTo(float roll_, float pitch_, float yaw_){
    roll = roll_; pitch = pitch_; yaw = yaw_;

    cam.roll = glm::pi<float>() / 2 - roll_;
    cam.pitch = pitch_;
    cam.yaw = -glm::pi<float>() / 2 - yaw_;

    cam.updateUpVec(); cam.updateLookAt();
}

void drone::clearTraj(void){
    data_traj.clear();
}

void drone::draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program) {
    Object::draw(P, V, id_shader_program);

    // trajectory
    gl->glBindVertexArray(VAO_traj);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_traj);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_traj);
    gl->glBufferData(GL_ARRAY_BUFFER, data_traj.size() * sizeof(float), data_traj.data(), GL_STATIC_DRAW);
    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(P * V));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_traj.size() * sizeof(float));

    // lookAt Arrow
    glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, pos); // object move
    M = glm::rotate(M, float(yaw), glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate(M, float(pitch), glm::vec3(0, 1, 0));
    M = glm::rotate(M, float(roll), glm::vec3(0, 0, 1));
    M = glm::rotate(M, float(-glm::pi<double>() / 2.0), glm::vec3(1, 0, 0));
    M = glm::scale(M, arrScale * glm::vec3(scale, scale, scale));
    glm::mat4 PVM = P * V * M;

    gl->glBindVertexArray(VAO_ArrLA);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_ArrLA);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrLA);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_ArrLA.size() * sizeof(float));

    // upVec Arrow
    M = glm::mat4(1);
    M = glm::translate(M, pos); // object move
    M = glm::rotate(M, float(yaw), glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate(M, float(pitch), glm::vec3(0, 1, 0));
    M = glm::rotate(M, float(roll), glm::vec3(0, 0, 1));
    M = glm::scale(M, arrScale * glm::vec3(scale, scale, scale));
    PVM = P * V * M;

    gl->glBindVertexArray(VAO_ArrUV);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_ArrUV);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrUV);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_ArrUV.size() * sizeof(float));

    // velocity Arrow
    if (prevLocations.size() == 0){
        return;
    }
    glm::vec3 velocity = glm::normalize(prevLocations.back() - prevLocations.front());
    float v_pitch = asin(-velocity.y);
    float v_yaw = -atan2(velocity.z, velocity.x);
    auto vel_unscaled = prevLocations.back() - prevLocations.front();
    float length = sqrt(vel_unscaled.x * vel_unscaled.x + vel_unscaled.y * vel_unscaled.y + vel_unscaled.z * vel_unscaled.z);

    M = glm::mat4(1);
    M = glm::translate(M, pos); // object move
    M = glm::rotate(M, v_yaw + float(glm::pi<double>()), glm::vec3(0, 1, 0)); // object rotate
    M = glm::rotate(M, v_pitch + float(glm::pi<double>() / 2), glm::vec3(0, 0, 1));
    M = glm::scale(M, length * 5.0f * arrScale * glm::vec3(scale, scale, scale));
    PVM = P * V * M;

    gl->glBindVertexArray(VAO_ArrVEL);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_ArrVEL);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrVEL);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_ArrVEL.size() * sizeof(float));
}
