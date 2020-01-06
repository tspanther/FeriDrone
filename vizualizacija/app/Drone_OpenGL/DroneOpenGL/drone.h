#ifndef DRONE_H
#define DRONE_H

#include <objekt.h>
#include <camera.h>
#include <queue>

class drone : public Object
{
public:
    drone(QOpenGLFunctions_3_3_Core *gl_in, const char* objFile, const char* texFile, const char* objFileArrow, const char* texFileArrow_1, const char* texFileArrow_2, const char* texFileArrow_3, const char* texFileTrajectory, Light* light_in);
    ~drone();
    void draw(glm::mat4 P, glm::mat4 V, unsigned int id_shader_program, glm::vec3 camPos);
    void moveTo(glm::vec3 vec, unsigned int step);
    void tiltTo(float roll, float pitch, float yaw);
    void clearTraj(void);

    // 1stP camera
    Camera cam;
private:
    std::queue<glm::vec3> prevLocations;
    std::queue<glm::mat4> LRUD;
    static unsigned int prevLocWindowSize;
    static unsigned int LRUDWindowSize;

    // convenience
    glm::vec3 lookAt;
    glm::vec3 upVec;

    // arrows
    float arrScale = 0.1f;

    GLuint VBO_ArrLA;
    GLuint VAO_ArrLA;
    GLuint tex_id_ArrLA;
    std::vector<float> data_ArrLA;

    GLuint VBO_ArrUV;
    GLuint VAO_ArrUV;
    GLuint tex_id_ArrUV;
    std::vector<float> data_ArrUV;

    GLuint VBO_ArrVEL;
    GLuint VAO_ArrVEL;
    GLuint tex_id_ArrVEL;
    std::vector<float> data_ArrVEL;

    // trajectory
    float alpha = 0.4f;
    GLuint VBO_traj;
    GLuint VAO_traj;
    GLuint tex_id_traj;
    std::vector<float> data_traj;
};

#endif // DRONE_H
