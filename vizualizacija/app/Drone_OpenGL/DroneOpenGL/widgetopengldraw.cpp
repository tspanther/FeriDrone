#include "widgetopengldraw.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <fstream>
#include <QOpenGLFunctions_3_3_Core>
#include <chrono>
#include <thread>
#include <QApplication>
#include <QKeyEvent>
#include <string>

WidgetOpenGLDraw::WidgetOpenGLDraw(QWidget *parent) : QOpenGLWidget(parent) {
    QWidget::setFocusPolicy(Qt::StrongFocus);
    activeCam = &thirdPCam;
    setMouseTracking(true);
}

WidgetOpenGLDraw::~WidgetOpenGLDraw() {
    for (unsigned i = 0; i < objekti.size(); i++){
        delete objekti[i];
    }
    delete dron;
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

    current = QPoint(QWidget::width()/2, QWidget::height()/2);

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

    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_rock1.obj",
                                 "../DroneOpenGL/models/scene_rock1.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_rock2.obj",
                                 "../DroneOpenGL/models/scene_rock2.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_rock2.obj",
                                 "../DroneOpenGL/models/scene_rock2.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_rock3.obj",
                                 "../DroneOpenGL/models/scene_rock3.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scena_ground.obj",
                                 "../DroneOpenGL/models/scena_ground.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_leaves.obj",
                                 "../DroneOpenGL/models/scene_leaves.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_brown.obj",
                                 "../DroneOpenGL/models/scene_brown.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_darkgreen.obj",
                                 "../DroneOpenGL/models/scene_darkgreen.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_lightgreen.obj",
                                 "../DroneOpenGL/models/scene_lightgreen.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_leaves2.obj",
                                 "../DroneOpenGL/models/scene_leaves2.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_rockyMountains.obj",
                                 "../DroneOpenGL/models/scene_rockyMountains.jpg",
                                 &light));
    objekti.push_back(new Object(gl,
                                 "../DroneOpenGL/models/scene_road.obj",
                                 "../DroneOpenGL/models/color1.jpg",
                                 &light));

    dron = new drone(gl,
                     "../DroneOpenGL/models/drone.obj",
                     "../DroneOpenGL/models/plain_red.jpg",
                     "../DroneOpenGL/models/arrow.obj",
                     "../DroneOpenGL/models/blue.jpg",
                     "../DroneOpenGL/models/green.jpg",
                     "../DroneOpenGL/models/purple.png",
                     "../DroneOpenGL/models/yellowblue.png",
                     &light);

    lockedOnCam = &dron->lockedOnCam;
    firstPCam = &dron->firstPCam;

    const unsigned int err = gl->glGetError();
	if (err != 0) {
        std::cerr << "OpenGL init error: " << err << std::endl;
	}
}

void WidgetOpenGLDraw::resizeGL(int w, int h) {
    gl->glViewport(0, 0, w, h);
}

glm::mat4 WidgetOpenGLDraw::createProjectionMatrix() {
    return glm::perspective(glm::radians(60.0f), float(width()) / height(), 0.01f, 1000.0f);
}

void WidgetOpenGLDraw::paintGL() {
    makeCurrent();

    gl->glClearColor(0.7f, 0.7f, 0.7f, 1);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->glUseProgram(id_shader_program);

    glm::mat4 P = createProjectionMatrix();
    auto camPosZoomed = activeCam->pos + float(activeCam->zoom) * glm::normalize(activeCam->lookAt);
    glm::mat4 V = glm::lookAt(camPosZoomed, activeCam->lookAt + activeCam->pos, activeCam->upVec);

    for (unsigned int i = 0; i < objekti.size(); i++){
        objekti[i]->draw(P, V, id_shader_program, activeCam->pos);
    }
    dron->draw(P, V, id_shader_program, activeCam->pos);

    const unsigned int err = gl->glGetError();
	if (err != 0) {
		std::cerr << "OpenGL napaka: " << err << std::endl;
    }
}

void WidgetOpenGLDraw::stepAnimation(){
    unsigned int step = 1;
    double pos_x, pos_y, pos_z, roll, pitch, yaw;

    pos_x = animation[animationIdx * 6 + 0];
    pos_y = animation[animationIdx * 6 + 1];
    pos_z = animation[animationIdx * 6 + 2];
    roll = animation[animationIdx * 6 + 3];
    pitch = animation[animationIdx * 6 + 4];
    yaw = animation[animationIdx * 6 + 5];

    dron->moveTo(glm::vec3(pos_x, pos_y, pos_z), animationIdx);
    dron->tiltTo(float(roll), float(pitch), float(yaw));

    paintGL();

    animationIdx += step;
    animationIdx %= animation.size() / 6;
}

void WidgetOpenGLDraw::loadAnimation(){
    std::ifstream f("../DroneOpenGL/flight.csv");

    int idx;
    double roll, pitch, yaw, pos_x, pos_y, pos_z;
    animation.clear();
    while (f >> idx){
        f >> pos_x >> pos_y >> pos_z >> roll >> pitch >> yaw;
        animation.push_back(pos_x);
        animation.push_back(pos_y);
        animation.push_back(pos_z);
        animation.push_back(roll);
        animation.push_back(pitch);
        animation.push_back(yaw);
    }
    animationIdx = 0;
}

void WidgetOpenGLDraw::keyPressEvent(QKeyEvent *event){
    makeCurrent();

    glm::vec3 projLookAt = glm::vec3(thirdPCam.lookAt.x, 0.0f, thirdPCam.lookAt.z);
    glm::vec3 projLookAtFlip = glm::vec3(thirdPCam.lookAt.z, 0.0f, thirdPCam.lookAt.x);
    float speed = 0.02f; // param

    switch(event->key()) {
        case Qt::Key::Key_W:
            thirdPCam.pos += speed * glm::normalize(projLookAt);
            break;
        case Qt::Key::Key_A:
            thirdPCam.pos += speed * glm::normalize(projLookAtFlip);
            break;
        case Qt::Key::Key_S:
            thirdPCam.pos -= speed * glm::normalize(projLookAt);
            break;
        case Qt::Key::Key_D:
            thirdPCam.pos -= speed * glm::normalize(projLookAtFlip);
            break;
        case Qt::Key::Key_1:
            thirdPCam.setDefaults();
            dron->clearTraj();
            break;
        case Qt::Key::Key_Tab:
            if (activeCam == &thirdPCam) {
                activeCam = firstPCam;
            } else if (activeCam == firstPCam) {
                activeCam = lockedOnCam;
            } else {
                activeCam = &thirdPCam;
            }
            break;
        case Qt::Key::Key_2:
            loadAnimation();
            break;
        case Qt::Key::Key_3:
            stepAnimation();
            break;
        case Qt::Key::Key_4:
            objekti[currObj]->roll+=speed*sign;
            break;
        case Qt::Key::Key_5:
            objekti[currObj]->pitch+=speed*sign;
            break;
        case Qt::Key::Key_6:
            objekti[currObj]->yaw+=speed*sign;
            break;
        case Qt::Key::Key_7:
            objekti[currObj]->pos.x+=speed*sign;
            break;
        case Qt::Key::Key_8:
            objekti[currObj]->pos.y+=speed*sign;
            break;
        case Qt::Key::Key_9:
            objekti[currObj]->pos.z+=speed*sign;
            break;
        case Qt::Key::Key_BracketRight:
            currObj++;
            currObj%=2;
            break;
        case Qt::Key::Key_BracketLeft:
            sign*=(-1);
            break;
        case Qt::Key::Key_Escape:
            thirdPCam.lockToThirdPersonCamera = false;
            QWidget::setCursor(Qt::ArrowCursor);
            break;
    }
    update();
}


void WidgetOpenGLDraw::wheelEvent(QWheelEvent *event){
    makeCurrent();

    double speed = 1.0;

    if (activeCam == firstPCam){
        return;
    }

    if (event->delta() > 0)
        activeCam->zoom -= speed;
    else
        activeCam->zoom += speed;

    update();
}

void WidgetOpenGLDraw::mousePressEvent(QMouseEvent *event){
    makeCurrent();
    update();
}

void WidgetOpenGLDraw::mouseReleaseEvent(QMouseEvent *event){
    makeCurrent();

    thirdPCam.lockToThirdPersonCamera = true;
    QWidget::setCursor(Qt::BlankCursor);

    update();
}

void WidgetOpenGLDraw::mouseMoveEvent(QMouseEvent *event){
    makeCurrent();

    if(thirdPCam.lockToThirdPersonCamera){
        double sensitivity = 0.004;

        thirdPCam.yaw += float(sensitivity * (event->x() - current.x()));
        thirdPCam.pitch += float(sensitivity * (-(event->y() - current.y())));
        thirdPCam.updateLookAt();

        QPoint glob = mapToGlobal(QPoint(width()/2,height()/2));
        QCursor::setPos(glob);
        current = QPoint(width()/2,height()/2);
    }

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
