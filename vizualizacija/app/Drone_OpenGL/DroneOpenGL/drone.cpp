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
std::cout << "step" << step << std::endl;
    glm::vec3 lastPosition;
    glm::vec3 currentPosition;
    //glm::vec3 lastPositionProjected;

    if(prevLocations.empty()){
        currentPosition = vec;
        lastPosition = vec;
        /*currentPositionProjected = vec;
        currentPositionProjected.y = 0.0f;
        lastPositionProjected = vec;
        lastPositionProjected.y = 0.0f;*/
    }else{
        currentPosition = vec;
        lastPosition = prevLocations.back();
        /*currentPositionProjected = vec;
        currentPositionProjected.y = 0.0f;
        lastPositionProjected = prevLocations.back();
        lastPositionProjected.y = 0.0f;*/
    }

    /*if(step == 2){
        lastPosition = prevLocations.back();

    }

    if(step == 10){
        currentPosition = vec;
    }*/

    prevLocations.push(vec);
    if (prevLocations.size() > prevLocWindowSize){
        prevLocations.pop();
    }

    pos=vec;

    cam.camPos = pos + offset + 3.0f * glm::vec3(0.0f, 0.0f, 1.0f);// * lookAt;
    //cam.lookAt = lookAt;//glm::normalize(pos);

    //if(step == 10){
        std::cout << "cur.x" << currentPosition.x << "cur.y" << currentPosition.y << "cur.z" << currentPosition.z << std::endl;
        std::cout << "prev.x" << lastPosition.x << "cur.y" << lastPosition.y << "cur.z" << lastPosition.z << std::endl;

        // Trajectory data.
            std::vector<float> a = {
                lastPosition.x, lastPosition.y, lastPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                currentPosition.x, currentPosition.y, currentPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                lastPosition.x, 0.0, lastPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                currentPosition.x, currentPosition.y, currentPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                lastPosition.x, 0.0, lastPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
                 currentPosition.x, 0.0, currentPosition.z, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0
            };

            for (unsigned int i = 0; i< 9*6; i++){
                data_traj.push_back(a[i]);
            }
    //}


}

void drone::tiltTo(double roll_, double pitch_, double yaw_){
    roll = roll_; pitch = pitch_; yaw = yaw_;

    // update lookAt and upVec
    upVec = glm::vec3(glm::cos(roll), glm::sin(roll), 0.0);
    glm::vec3 front;
    front.x = cos(yaw) * cos(pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    lookAt = glm::normalize(front);

    //cam.camPos = pos + 3.0f * glm::vec3(0.0f, 0.0f, 1.0f);// * lookAt;
    //cam.lookAt = lookAt;
    //cam.camUp = upVec;
}

void drone::draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program) {
    Object::draw(P, V, id_shader_program);

    /*glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, pos); // object move
    M = glm::rotate(M, float(yaw), glm::vec3(1, 0, 0)); // object rotate
    M = glm::rotate(M, float(pitch), glm::vec3(0, 1, 0));
    M = glm::rotate(M, float(roll), glm::vec3(0, 0, 1));
    M = glm::scale(M, glm::vec3(scale, scale, scale));
    glm::mat4 PVM = P * V * M;*/

    // Draw trajectory.

    gl->glBindVertexArray(VAO_traj);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_traj);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_traj);
    gl->glBufferData(GL_ARRAY_BUFFER, data_traj.size() * sizeof(float), data_traj.data(), GL_STATIC_DRAW);

    //gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_traj.size() * sizeof(float));


    // lookAt Arrow
    glm::mat4 M = glm::mat4(1);
    M = glm::translate(M, pos + offset); // object move
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
    M = glm::translate(M, pos + offset); // object move
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
    float v_yaw = atan2(velocity.x, velocity.z);
    auto vel_unscaled = prevLocations.back() - prevLocations.front();
    float length = sqrt(vel_unscaled.x * vel_unscaled.x + vel_unscaled.y * vel_unscaled.y + vel_unscaled.z * vel_unscaled.z);

    M = glm::mat4(1);
    M = glm::translate(M, pos + offset); // object move
    M = glm::rotate(M, v_yaw * float(glm::pi<double>()) + float(glm::pi<double>() / 2), glm::vec3(0, 1, 0)); // object rotate
    M = glm::rotate(M, v_pitch * float(glm::pi<double>()) + float(glm::pi<double>() / 2), glm::vec3(0, 0, 1));
    M = glm::scale(M, /*length * 20.0f **/ arrScale * glm::vec3(scale, scale, scale));
    PVM = P * V * M;

    gl->glBindVertexArray(VAO_ArrVEL);
    gl->glBindTexture(GL_TEXTURE_2D, tex_id_ArrVEL);
    gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_ArrVEL);

    gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_shader_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
    gl->glDrawArrays(GL_TRIANGLES, 0, data_ArrVEL.size() * sizeof(float));
}
