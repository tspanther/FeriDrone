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
    activeCam = &thirdP;
    setMouseTracking(true);
    /* Cursor se v tem primeru skrije samo znotraj predvidenega območja izrisa */
    // QWidget::setCursor(Qt::BlankCursor);
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

    /*
    std::vector<const char*> objFiles = { "../DroneOpenGL/models/drone.obj", "../DroneOpenGL/models/Low-Poly_Models.obj" };
    std::vector<const char*> texFiles = { "../DroneOpenGL/models/plain_red.jpg", "../DroneOpenGL/models/plain_grey.jpg" };

    for (unsigned int i = 0; i < objFiles.size(); i++){
        objekti.push_back(new Object(gl, objFiles[i], texFiles[i]));
    }
    */

    std::vector<const char*> texFiles = { "../DroneOpenGL/models/merc.jpg", "../DroneOpenGL/models/equirect.jpg" };
    std::vector<glm::vec3> offsets = {   glm::vec3(0.0f, 0.0f, 0.0f),
                                         glm::vec3(0.0f, 3.0f, 0.0f),
                                         glm::vec3(3.0f, 0.0f, 0.0f),
                                         glm::vec3(3.0f, 3.0f, 0.0f),
                                         glm::vec3(6.0f, 0.0f, 0.0f),
                                         glm::vec3(6.0f, 3.0f, 0.0f),
                                         glm::vec3(9.0f, 0.0f, 0.0f),
                                         glm::vec3(9.0f, 3.0f, 0.0f),
                                         glm::vec3(12.0f, 0.0f, 0.0f),
                                         glm::vec3(12.0f, 3.0f, 0.0f),
                                         glm::vec3(15.0f, 0.0f, 0.0f),
                                         glm::vec3(15.0f, 3.0f, 0.0f)    };
    unsigned int idx = 0;

    for (int lt = lepjenjeTeksture::izDatoteke; lt < lepjenjeTeksture::stoplt; lt++){
        for (int sl = smerLepljenja::x; sl < smerLepljenja::stopsl; sl++){
            for (unsigned int i = 0; i < 2; i++) {
                objekti.push_back(new Object(gl, "../DroneOpenGL/models/ball.obj", texFiles[i], static_cast<lepjenjeTeksture>(lt), static_cast<smerLepljenja>(sl)));
                objekti[idx]->offset = offsets[idx];
                idx++;
            }
        }
    }


    /*
    objekti.push_back(new Object(gl, "../DroneOpenGL/models/Low-Poly_Models.obj", "../DroneOpenGL/models/plain_grey.jpg"));

    dron = new drone(gl, "../DroneOpenGL/models/drone.obj", "../DroneOpenGL/models/plain_red.jpg", "../DroneOpenGL/models/arr.obj", "../DroneOpenGL/models/blue.jpg", "../DroneOpenGL/models/green.jpg", "../DroneOpenGL/models/purple.jpg");
    firstP = &dron->cam;

    // debug --- empiricni offseti za objekte
    objekti[0]->offset = glm::vec3(0, -0.4, 0);
    dron->offset = glm::vec3(-0.1, 1.06, 0.52);
    dron->pitcho = 4.84;
    */

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
    //dron->draw(P, V, id_shader_program);

    const unsigned int err = gl->glGetError();
	if (err != 0) {
		std::cerr << "OpenGL napaka: " << err << std::endl;
    }
}

void WidgetOpenGLDraw::stepAnimation(){
    int step = 1;
    double pos_x, pos_y, pos_z, roll, pitch, yaw;

    pos_x = animation[animationIdx * 6 + 0];
    pos_y = animation[animationIdx * 6 + 1];
    pos_z = animation[animationIdx * 6 + 2];
    roll = animation[animationIdx * 6 + 3];
    pitch = animation[animationIdx * 6 + 4];
    yaw = animation[animationIdx * 6 + 5];

    dron->moveTo(glm::vec3(pos_x, pos_y, pos_z));
    dron->tiltTo(roll, pitch, yaw);

    /*
    firstP->pitch = 0.0 ;//+ pitch;
    firstP->yaw = -glm::pi<double>() / 2 ;// + yaw;
    firstP->roll = glm::pi<double>() / 2 ;//+ roll;
    firstP->camPos = glm::vec3(pos_x, pos_y, pos_z + 3);

    firstP->updateLookAt();
    firstP->updateUpVec();

    objekti[0]->pos = glm::vec3(pos_x, pos_y, pos_z);
    objekti[0]->roll = roll;
    objekti[0]->pitch = pitch;
    objekti[0]->yaw = yaw;
    */

    paintGL();
    animationIdx+=step;
    animationIdx%=animation.size()/6;
}

void WidgetOpenGLDraw::loadAnimation(){
    std::ifstream f("../DroneOpenGL/flight.csv");
    int idx;
    double roll, pitch, yaw;
    float pos_x, pos_y, pos_z;
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

    glm::vec3 projLookAt = glm::vec3(thirdP.lookAt.x, 0.0f, thirdP.lookAt.z);
    glm::vec3 projLookAtFlip = glm::vec3(thirdP.lookAt.z, 0.0f, thirdP.lookAt.x);
    float speed = 0.02f; // param

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
        /*
            if (activeCam == &thirdP) {
                activeCam = firstP;
            } else {
                activeCam = &thirdP;
            }
            */
            break;
        case Qt::Key::Key_2:
            //loadAnimation();
            break;
        case Qt::Key::Key_3:
            //stepAnimation();
            break;
        case Qt::Key::Key_4:
            objekti[currObj]->rollo+=speed*sign;
            break;
        case Qt::Key::Key_5:
            objekti[currObj]->pitcho+=speed*sign;
            break;
        case Qt::Key::Key_6:
            objekti[currObj]->yawo+=speed*sign;
            break;
        case Qt::Key::Key_7:
            objekti[currObj]->offset.x+=speed*sign;
            break;
        case Qt::Key::Key_8:
            objekti[currObj]->offset.y+=speed*sign;
            break;
        case Qt::Key::Key_9:
            objekti[currObj]->offset.z+=speed*sign;
            break;
        case Qt::Key::Key_BracketRight:
            currObj++;
            currObj%=objekti.size();
            break;
        case Qt::Key::Key_BracketLeft:
            sign*=(-1);
            break;
        case Qt::Key::Key_Escape:
            thirdP.lockToThirdPersonCamera = false;
            QWidget::setCursor(Qt::ArrowCursor);
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
    // do we need this?
    //leftMouseButton = false;
    //rightMouseButton = false;
    thirdP.lockToThirdPersonCamera = true;
    QWidget::setCursor(Qt::BlankCursor);
    update();
}

void WidgetOpenGLDraw::mouseMoveEvent(QMouseEvent *event){
    /*
     * todo: Stefan::
     * ideja je, da bi miska bila v oknu nevidna (to je narjeno, v konstruktorju je klic funkcije)
     * kaj bi blo treba nardit, je to, da nikoli ne prides z misko na rob
     *  - jaz sem nekaj gledal, mogoce bi slo z kako funkcijo skos postavit misko na sredino potem, ko preberes da se je premaknila
     *  - lahko tudi da ko prides z misko na rob, dalje vrtis kamero v tisto smer (kot da bi se miska premikala dalje - ven iz okna - kar se ne more)
     *
     * sej ves, ce v igrici misko premikas ne vem po mizi 1 meter, bos se zavrtel 3x okoli sebe. pri nas pa ko prides na rob okna, se nikamor ne mores vec premaknit
     */

    /* Rotacija kamere v 3D okolju: zdaj deluje OK */
    /* Način rešitve: Cursor je "zaklenjen" na sredino (čeprav dejansko ni, saj se še vedno čuti "preskakovanje").*/
    /* Na ta način bi moral cursor vedno ostati v sredini (razen če mam nevem 2,3 ali več screenov, takrat če hitro potegneš miško lahko da še vedno pobegne) */
    /* Možna izboljšava: Limitiranje npr. Yaw-a (zdaj se lahko yawaš over and over again, če bo čas mogoče spisati kake meje, znotraj katerih kamera operira) */


    makeCurrent();

    if(thirdP.lockToThirdPersonCamera){
        double sensitivity = 0.004;

        thirdP.yaw += sensitivity * (event->x() - current.x());
        thirdP.pitch += sensitivity * (-(event->y() - current.y()));
        thirdP.updateLookAt();

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
