#include "widgetopengldraw.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <fstream>
#include <QOpenGLFunctions_3_3_Core>
#include <QApplication>
#include <QKeyEvent>
#include <string>

WidgetOpenGLDraw::WidgetOpenGLDraw(QWidget *parent) : QOpenGLWidget(parent) {
    QWidget::setFocusPolicy(Qt::StrongFocus);
    activeCam = &thirdP;
}

WidgetOpenGLDraw::~WidgetOpenGLDraw() {
    for (unsigned i = 0; i < objekti.size(); i++){
        delete objekti[i];
    }
    gl->glDeleteProgram(id_shader_program);
}

void WidgetOpenGLDraw::printProgramInfoLog(GLuint obj) {
    int infologLength = 0;
    gl->glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
        std::unique_ptr<char[]> infoLog(new char[infologLength]);
        int charsWritten = 0;
        gl->glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog.get());
        std::cerr << infoLog.get() << "\n";
    }
}

void WidgetOpenGLDraw::printShaderInfoLog(GLuint obj) {
    int infologLength = 0;
    gl->glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
		std::unique_ptr<char[]> infoLog(new char[infologLength]);
		int charsWritten = 0;
        gl->glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog.get());
		std::cerr << infoLog.get() << "\n";
	}
}

void WidgetOpenGLDraw::compileShaders() {
    id_shader_program = gl->glCreateProgram();

    {
        GLuint vs = gl->glCreateShader(GL_VERTEX_SHADER);
        std::string vss;
        readFile("../DroneOpenGL/vshader.vert", vss);
		std::cout << vss;
		const char *vssc = vss.c_str();
        gl->glShaderSource(vs, 1, &vssc, nullptr);
        gl->glCompileShader(vs);
		printShaderInfoLog(vs);
        gl->glAttachShader(id_shader_program, vs);
	}

    {
        GLuint fs = gl->glCreateShader(GL_FRAGMENT_SHADER);
        std::string fss;
        readFile("../DroneOpenGL/fshader.frag", fss);
		std::cout << fss;
		const char *fssc = fss.c_str();
        gl->glShaderSource(fs, 1, &fssc, nullptr);
        gl->glCompileShader(fs);
		printShaderInfoLog(fs);
        gl->glAttachShader(id_shader_program, fs);
	}

    gl->glLinkProgram(id_shader_program);
    printProgramInfoLog(id_shader_program);
}

void WidgetOpenGLDraw::initializeGL() {
    std::cout << "OpenGL context version: "<< context()->format().majorVersion() <<"." <<context()->format().minorVersion()<<std::endl;

    gl = context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!gl) {
        std::cerr << "Could not obtain required OpenGL context version";
        QApplication::exit(1);
    }

    std::cout << gl->glGetString(GL_VENDOR) << std::endl;
    std::cout << gl->glGetString(GL_VERSION) << std::endl;
    std::cout << gl->glGetString(GL_RENDERER) << std::endl;

    compileShaders();
    gl->glEnable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);

    std::vector<const char*> objFiles = { "../DroneOpenGL/models/drone.obj" };
    std::vector<const char*> texFiles = { "../DroneOpenGL/models/color2.jpg" };

    for (unsigned int i = 0; i < objFiles.size(); i++){
        objekti.push_back(new Object(gl, objFiles[i], texFiles[i]));
    }

    thirdP.setDefaults();

    const unsigned int err = gl->glGetError();
	if (err != 0) {
        std::cerr << "OpenGL init error: " << err << std::endl;
	}
}

void WidgetOpenGLDraw::resizeGL(int w, int h) {
    gl->glViewport(0, 0, w, h);
}

glm::mat4 WidgetOpenGLDraw::createProjectionMatrix() {
    switch (projMode){
        case 0:
            return glm::perspective(glm::radians(60.0f), float(width()) / height(), 0.01f, 1000.0f);
        case 1:
            return glm::ortho(-5.0, 5.0, -5.0, 5.0, 0.01, 1000.0);
    }
}

void WidgetOpenGLDraw::paintGL() {
    makeCurrent();

    gl->glClearColor(0.7f, 0.7f, 0.7f, 1);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glUseProgram(id_shader_program);

    glm::mat4 P = createProjectionMatrix();
    auto camPosZoomed = activeCam->camPos + (float)activeCam->zoom * glm::normalize(activeCam->lookAt);
    glm::mat4 V = glm::lookAt(camPosZoomed, activeCam->lookAt + activeCam->camPos, activeCam->camUp);

    for (unsigned int i = 0; i < objekti.size(); i++){
        objekti[i]->draw(P, V, id_shader_program);
    }

    const unsigned int err = gl->glGetError();
	if (err != 0) {
		std::cerr << "OpenGL napaka: " << err << std::endl;
    }
}

void WidgetOpenGLDraw::keyPressEvent(QKeyEvent *event){
    makeCurrent();

    glm::vec3 projLookAt = glm::vec3(thirdP.lookAt.x, 0.0f, thirdP.lookAt.z);
    glm::vec3 projLookAtFlip = glm::vec3(thirdP.lookAt.z, 0.0f, thirdP.lookAt.x);
    float speed = 1.0f; // param

    switch(event->key()) {
    /*
     * Translacije/rotacije objekta in ostala koda iz solskih vaj
     *
        case Qt::Key::Key_Q:
            objekti[focusObject]->objrotX+=10;
            break;
        case Qt::Key::Key_W:
            objekti[focusObject]->objrotX-=10;
            break;
        case Qt::Key::Key_E:
            objekti[focusObject]->objrotY+=10;
            break;
        case Qt::Key::Key_R:
            objekti[focusObject]->objrotY-=10;
            break;
        case Qt::Key::Key_T:
            objekti[focusObject]->objrotZ+=10;
            break;
        case Qt::Key::Key_Y:
            objekti[focusObject]->objrotZ-=10;
            break;
        case Qt::Key::Key_U:
            objekti[focusObject]->objx0+=0.1;
            break;
        case Qt::Key::Key_I:
            objekti[focusObject]->objx0-=0.1;
            break;
        case Qt::Key::Key_O:
            objekti[focusObject]->objy0+=0.1;
            break;
        case Qt::Key::Key_P:
            objekti[focusObject]->objy0-=0.1;
            break;
        case Qt::Key::Key_BracketLeft:
            objekti[focusObject]->objz0+=0.1;
            break;
        case Qt::Key::Key_BracketRight:
            objekti[focusObject]->objz0-=0.1;
            break;
        case Qt::Key::Key_2:
            projMode = 0;
            break;
        case Qt::Key::Key_3:
            projMode = 1;
            break;
        case Qt::Key::Key_Equal:
            focusObject++;
            focusObject %= objekti.size();
            break;
        case Qt::Key::Key_Z:
            objekti[focusObject]->scale*=1.1;
            break;
        case Qt::Key::Key_X:
            objekti[focusObject]->scale*=0.9;
            break;
        case Qt::Key::Key_A:
            pitch += glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_S:
            pitch -= glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_D:
            yaw += glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_F:
            yaw -= glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_G:
            roll += glm::pi<double>() / 30.0;
            updateUpVec();
            break;
        case Qt::Key::Key_H:
            roll -= glm::pi<double>() / 30.0;
            updateUpVec();
            break;
        case Qt::Key::Key_A:
            pitch += glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_S:
            pitch -= glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_D:
            yaw += glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_F:
            yaw -= glm::pi<double>() / 30.0;
            updateLookAt();
            break;
        case Qt::Key::Key_G:
            roll += glm::pi<double>() / 30.0;
            updateUpVec();
            break;
        case Qt::Key::Key_H:
            roll -= glm::pi<double>() / 30.0;
            updateUpVec();
            break;
    */
        case Qt::Key::Key_W:
            thirdP.camPos += speed * glm::normalize(projLookAt);
            break;
        case Qt::Key::Key_A:
            thirdP.camPos += speed * glm::normalize(projLookAtFlip);
            break;
        case Qt::Key::Key_S:
            thirdP.camPos -= speed * glm::normalize(projLookAt);
            break;
        case Qt::Key::Key_D:
            thirdP.camPos -= speed * glm::normalize(projLookAtFlip);
            break;
        case Qt::Key::Key_1:
            thirdP.setDefaults();
            break;
        case Qt::Key::Key_Tab:
            if (activeCam == &thirdP) {
                activeCam = &firstP;
            } else {
                activeCam = &thirdP;
            }
            break;

    }
    update();
}


void WidgetOpenGLDraw::wheelEvent(QWheelEvent *event){
    makeCurrent();
    double speed = 1.0;

    if (event->delta() > 0)
        activeCam->zoom -= speed;
    else
        activeCam->zoom += speed;

    /*
     *  Stefko: za zdaj sva se zmenila, da ne rabimo tega
     *
    // Camera, Z translation.
    if(!invert){
        if(event->delta() > 0)
            activeCam->camPos -= glm::vec3(0.0, 0.0, 0.1);
        else
            activeCam->camPos += glm::vec3(0.0, 0.0, 0.1);
    }else{
        if(event->delta() > 0)
            roll += glm::pi<double>() / 30.0;
        else
            roll -= glm::pi<double>() / 30.0;

        updateUpVec();
    }
    */

    update();
}

void WidgetOpenGLDraw::mousePressEvent(QMouseEvent *event){
    makeCurrent();

    /*
     *  Stefko: za zdaj sva se zmenila, da ne rabimo tega
     *
    if (event->button() == Qt::MiddleButton)
        invert = !invert;

    if (event->button()==Qt::LeftButton)
        leftMouseButton = true;

    if(event->button()==Qt::RightButton)
        rightMouseButton = true;
    */

    update();
}

void WidgetOpenGLDraw::mouseReleaseEvent(QMouseEvent *event){
    makeCurrent();
    leftMouseButton = false;
    rightMouseButton = false;
    update();
}

void WidgetOpenGLDraw::mouseMoveEvent(QMouseEvent *event){
    makeCurrent();

    QPoint point = event->pos();

    double sensitivity = 0.05;

    thirdP.yaw += sensitivity * (point.rx() - current.rx());
    thirdP.pitch += sensitivity * (point.ry() - current.ry());
    thirdP.updateLookAt();

    /*
     *  Stefko: miska rotira kamero kot dogovorjeno
     *
    if(leftMouseButton && !invert){
        if(deltaX>deltaY){
            if(current.rx()<point.rx()){
                camPos += glm::vec3(0.1, 0.0, 0.0);
            }else if(current.rx()>point.rx()){
                camPos -= glm::vec3(0.1, 0.0, 0.0);
            }
        }
    }else if(rightMouseButton && !invert){
        if(deltaX<=deltaY){
            if(current.ry()>point.ry()){
                camPos += glm::vec3(0.0, 0.1, 0.0);
            }else if(current.ry()<point.ry()){
                camPos -= glm::vec3(0.0, 0.1, 0.0);
            }
        }
    }else if(leftMouseButton && invert){
        if(deltaX>deltaY){
            if(current.rx()<point.rx()){
                yaw += glm::pi<double>() / 30.0;
            }else if(current.rx()>point.rx()){
                yaw -= glm::pi<double>() / 30.0;
            }
        }
        updateLookAt();
    }else if(rightMouseButton && invert){
        if(deltaX<=deltaY){
            if(current.ry()>point.ry()){
                pitch += glm::pi<double>() / 30.0;
            }else if(current.ry()<point.ry()){
                pitch -= glm::pi<double>() / 30.0;
            }
        }
        updateLookAt();
    }
    */

    current=point;

    update();
}

void WidgetOpenGLDraw::readFile(std::string path, std::string &save){
    std::ifstream fStream(path);

    std::string line;
    while(!fStream.eof()) {
        std::getline(fStream, line);
        save.append(line + "\n");
    }

    fStream.close();
}
