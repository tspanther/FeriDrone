#ifndef WIDGETOPENGLDRAW_H
#define WIDGETOPENGLDRAW_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <objekt.h>

class QOpenGLFunctions_3_3_Core;
class WidgetOpenGLDraw : public QOpenGLWidget{
public:
	WidgetOpenGLDraw(QWidget* parent);

    ~WidgetOpenGLDraw() override;
private:
    void compileShaders();
	void printProgramInfoLog(GLuint obj);
    void printShaderInfoLog(GLuint obj);
    void updateUpVec();
    void updateLookAt();
    void setDefaults();
    glm::mat4 createProjectionMatrix();

    QOpenGLFunctions_3_3_Core* gl;
    unsigned int id_shader_program;

    // objects
    std::vector<Object*> objekti;
    unsigned int focusObject;

    // cam
    glm::vec3 camPos;
    glm::vec3 lookAt;
    glm::vec3 camUp;
    double pitch;
    double yaw;
    double roll;

    unsigned int projMode = 0;
protected:
	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent *event) override;
    void readFile(std::string path, std::string &save);
signals:

public slots:

};

#endif // WIDGETOPENGLDRAW_H
