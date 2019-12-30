#ifndef WIDGETOPENGLDRAW_H
#define WIDGETOPENGLDRAW_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <objekt.h>
#include <camera.h>
#include <vector>
#include <drone.h>

class QOpenGLFunctions_3_3_Core;
class WidgetOpenGLDraw : public QOpenGLWidget{
public:
	WidgetOpenGLDraw(QWidget* parent);

    ~WidgetOpenGLDraw() override;
private:
    void compileShaders();
	void printProgramInfoLog(GLuint obj);
    void printShaderInfoLog(GLuint obj);
    void loadAnimation();
    void stepAnimation();
    glm::mat4 createProjectionMatrix();

    QOpenGLFunctions_3_3_Core* gl;
    unsigned int id_shader_program;

    // objects
    std::vector<Object*> objekti;
    drone* dron;

    // cam
    Camera thirdP;
    Camera* firstP;
    Camera* activeCam;

    // premikanje objektov ---- debug
    int currObj = 0;
    int sign = 1;

    std::vector<double> animation;
    int animationIdx= 0;

    // other
    unsigned int projMode = 0;
    //bool invert = false;
    //bool leftMouseButton = false, rightMouseButton = false;
    bool test = false;
    QPoint current = QPoint(0, 0);
protected:
	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void readFile(std::string path, std::string &save);
signals:

public slots:

};

#endif // WIDGETOPENGLDRAW_H
