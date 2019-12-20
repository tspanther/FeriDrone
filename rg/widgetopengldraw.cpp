#include "widgetopengldraw.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <QOpenGLFunctions_3_3_Core>
#include <QApplication>
#include <fstream>
#include <sstream>
#include "objloader.hpp"
#include <string>

WidgetOpenGLDraw::WidgetOpenGLDraw(QWidget *parent) : QOpenGLWidget(parent) {
}

// Destruktor.
WidgetOpenGLDraw::~WidgetOpenGLDraw() {
    // Za sabo je potrebno počistiti (ni GC-ja).
    for(unsigned int i = 0; i < seznamObjektov.size(); i++){
        gl->glDeleteVertexArrays(1, &seznamObjektov[i].bufferVAO);
    }
    gl->glDeleteProgram(id_sencilni_program);
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

void WidgetOpenGLDraw::PrevediSencilnike() {
    id_sencilni_program = gl->glCreateProgram();

	{  // vhod v senčilnik oglišč je in_Pos, izhod pa gl_Position (rezervirana beseda)
        GLuint vs = gl->glCreateShader(GL_VERTEX_SHADER);

        std::string vss;

        readFile("../RG_Template/vsencilnik.vert", vss);

		std::cout << vss;
		const char *vssc = vss.c_str();
        gl->glShaderSource(vs, 1, &vssc, nullptr);
        gl->glCompileShader(vs);
		printShaderInfoLog(vs);
        gl->glAttachShader(id_sencilni_program, vs);
	}

	{  // out_Color je barva, ki bo prišla do zaslona
        GLuint fs = gl->glCreateShader(GL_FRAGMENT_SHADER);
        std::string fss;  // priporočamo hrambo spodnjega niza v datoteki (fsencilnik.frag), potem dobite barvanje sintakse in autocomplete

        readFile("../RG_Template/fsencilnik.frag", fss);

		std::cout << fss;
		const char *fssc = fss.c_str();
        gl->glShaderSource(fs, 1, &fssc, nullptr);
        gl->glCompileShader(fs);
		printShaderInfoLog(fs);
        gl->glAttachShader(id_sencilni_program, fs);
	}

    gl->glLinkProgram(id_sencilni_program);
	printProgramInfoLog(id_sencilni_program);
}

void WidgetOpenGLDraw::initializeGL() {
	// naložimo funkcije za OpenGL
    std::cout << "OpenGL context version: "<< context()->format().majorVersion() <<"." <<context()->format().minorVersion()<<std::endl;

    gl=context()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!gl) {
        std::cerr << "Could not obtain required OpenGL context version";
        QApplication::exit(1);
    }


    std::cout << gl->glGetString(GL_VENDOR) << std::endl;
    std::cout << gl->glGetString(GL_VERSION) << std::endl;
    std::cout << gl->glGetString(GL_RENDERER) << std::endl;

	PrevediSencilnike();

    // Ne prekrivaj.
    gl->glEnable(GL_DEPTH_TEST);

    if(izbraneDatoteke.size() > 0){

        // Počisti vektorje nalaganja.
        out_vertices.clear();
        out_uvs.clear();
        out_normals.clear();

        loadOBJ(izbraneDatoteke[izbraneDatoteke.size() - 1].c_str(), out_vertices, out_uvs, out_normals);

        // Seznam točk uporabljen pri predstavitvi objekta.
        std::vector<Tocka> seznamTock;

        for (int i = 0; static_cast<unsigned int>(i)< out_vertices.size(); ++i) {
            tocka.pozicija = out_vertices[static_cast<unsigned int>(i)];

            if(normsBarva)
                tocka.barva = glm::abs(out_normals[static_cast<unsigned int>(i)]);
            else
                tocka.barva = glm::vec3(glm::abs(out_uvs[static_cast<unsigned int>(i)]), 0);

            seznamTock.push_back(tocka);
        }

        Tocka* seznamTockPolje = &seznamTock[0];
        obj.steviloOglisc = out_vertices.size();

        gl->glGenVertexArrays(1, &obj.objektVAO);
        gl->glBindVertexArray(obj.objektVAO);

        gl->glGenBuffers(1, &obj.bufferVAO);
        gl->glBindBuffer(GL_ARRAY_BUFFER, obj.bufferVAO);
        gl->glBufferData(GL_ARRAY_BUFFER, static_cast<int>(sizeof(Tocka) * obj.steviloOglisc), static_cast<const void*>(seznamTockPolje), GL_STATIC_DRAW);

        gl->glEnableVertexAttribArray(0);
        gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Tocka), reinterpret_cast<int*>(offsetof(Tocka, pozicija)));
        gl->glEnableVertexAttribArray(1);
        gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Tocka), reinterpret_cast<int*>(offsetof(Tocka, barva)));

        glm::mat4 M = glm::mat4(1);
        M = glm::translate(M, glm::vec3(0, 0.9, -4));
        obj.lokacija = M;
        obj.zadnjaLokacija = glm::mat4(1);

        obj.pozicijeSliderja.push_back(objTraX);
        obj.pozicijeSliderja.push_back(objTraY);
        obj.pozicijeSliderja.push_back(objTraZ);
        obj.pozicijeSliderja.push_back(objRotX);
        obj.pozicijeSliderja.push_back(objRotY);
        obj.pozicijeSliderja.push_back(objRotZ);
        obj.pozicijeSliderja.push_back(objSkaX);
        obj.pozicijeSliderja.push_back(objSkaY);
        obj.pozicijeSliderja.push_back(objSkaZ);

        seznamObjektov.push_back(obj);
        std::cout << "Objekt nalozen! Stevilo oglisc: " << seznamObjektov[0].steviloOglisc << std::endl;
    }

    // Napake.
    const unsigned int err = gl->glGetError();
	if (err != 0) {
		std::cerr << "OpenGL init napaka: " << err << std::endl;
	}
}

void WidgetOpenGLDraw::resizeGL(int w, int h) {
    gl->glViewport(0, 0, w, h);
}

void WidgetOpenGLDraw::paintGL() { // Izris
    gl->glClearColor(0.2f, 0.2f, 0.2f, 1);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl->glUseProgram(id_sencilni_program);

    glm::mat4 P;
    if(jePerspektivna)
        P = glm::perspective(glm::radians(60.0f), float(width()) / height(), 0.01f, 1000.0f);
    else
        P = glm::ortho( -7.0f, 7.0f, -7.0f / (float(width()) / height()), 7.0f / (float(width()) / height()), 0.01f, 1000.0f );

    glm::mat4 V = glm::mat4(1);  // matrika pogleda (view) (premikanje kamere...)

    // Kamera:
    V = glm::rotate(V, glm::radians(static_cast<float>(kamRotX)), glm::vec3(1, 0, 0));
    V = glm::rotate(V, glm::radians(static_cast<float>(kamRotY)), glm::vec3(0, 1, 0));
    V = glm::rotate(V, glm::radians(static_cast<float>(kamRotZ)), glm::vec3(0, 0, 1));
    V = glm::translate(V, glm::vec3(kamTraX, kamTraY, kamTraZ));

    // Premikanje objekta po sceni:
    glm::mat4 Mobjekt = glm::mat4(1);
    Mobjekt = glm::translate(Mobjekt, glm::vec3(objTraX, objTraY, objTraZ));
    Mobjekt = glm::rotate(Mobjekt, glm::radians(static_cast<float>(objRotX)), glm::vec3(1,0,0));
    Mobjekt = glm::rotate(Mobjekt, glm::radians(static_cast<float>(objRotY)), glm::vec3(0,1,0));
    Mobjekt = glm::rotate(Mobjekt, glm::radians(static_cast<float>(objRotZ)), glm::vec3(0,0,1));
    Mobjekt = glm::scale(Mobjekt, glm::vec3(objSkaX/50, objSkaY/50, objSkaZ/50));
    Mobjekt = glm::translate(Mobjekt, glm::vec3(0, 0, 4));

    // Izris objektov.
    for(unsigned int i = 0; i <seznamObjektov.size(); i++){
        gl->glBindVertexArray(seznamObjektov[i].objektVAO);

        glm::mat4 PVM;
        if(std::stoi(indeksPremikanegaObjekta) >= 0 && i == static_cast<unsigned int>(std::stoi(indeksPremikanegaObjekta))){
            seznamObjektov[i].zadnjaLokacija = Mobjekt;
            PVM = P * V * seznamObjektov[i].zadnjaLokacija *seznamObjektov[i].lokacija;
            ObdelajPonastavitevDrsnikov(seznamObjektov[i].pozicijeSliderja);
        }else{
            PVM = P * V * seznamObjektov[i].zadnjaLokacija * seznamObjektov[i].lokacija;
        }

        gl->glUniformMatrix4fv(gl->glGetUniformLocation(id_sencilni_program, "PVM"), 1, GL_FALSE, glm::value_ptr(PVM));
        gl->glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(seznamObjektov[i].steviloOglisc* sizeof(Tocka)));
    }

    // Izpis napake.
    const unsigned int err = gl->glGetError();
	if (err != 0) {
		std::cerr << "OpenGL napaka: " << err << std::endl;
    }
}

void WidgetOpenGLDraw::readFile(std::string path, std::string &save){
    std::ifstream fStream;
    fStream.open(path, std::ifstream::in);

    std::string line;
    while(!fStream.eof()) {
        std::getline(fStream, line);
        save.append(line + "\n");
    }

    fStream.close();
}

void WidgetOpenGLDraw::KameraTranslacijaX(int value){
    makeCurrent();

    kamTraX = value;
    update();
}
void WidgetOpenGLDraw::KameraTranslacijaY(int value){
    makeCurrent();

    kamTraY = value;
    update();
}

void WidgetOpenGLDraw::KameraTranslacijaZ(int value){
    makeCurrent();

    kamTraZ = value;
    update();
}

void WidgetOpenGLDraw::KameraRotacijaX(int value){
    makeCurrent();

    kamRotX = value;
    update();
}

void WidgetOpenGLDraw::KameraRotacijaY(int value){
    makeCurrent();

    kamRotY = value;
    update();
}

void WidgetOpenGLDraw::KameraRotacijaZ(int value){
    makeCurrent();

    kamRotZ = value;
    update();
}

void WidgetOpenGLDraw::PerspektivnaProj(){
    makeCurrent();
    jePerspektivna = true;
    update();
}

void WidgetOpenGLDraw::ParalelnaProj(){
    makeCurrent();
    jePerspektivna = false;
    update();
}

void WidgetOpenGLDraw::setUvozObjektov(std::string potiDatotek){
    makeCurrent();

    // Enojni / zamenjaj z \\.
    std::string novaPot = "";

    for(unsigned int i= 0; i < potiDatotek.length(); i++){
        if(potiDatotek[i] == '/')
            novaPot = novaPot + "\\\\";
        else
            novaPot = novaPot + potiDatotek[i];
    }

    izbraneDatoteke.push_back(novaPot);

    indeksPremikanegaObjekta = "-1";
    initializeGL();
    update();
}

void WidgetOpenGLDraw::izberiObjektPremika(std::string vhod){
    makeCurrent();

    std::string delimiter = "|";
    std::string token = vhod.substr(0, vhod.find(delimiter));

    if(vhod == "")
        indeksPremikanegaObjekta = "-1";
    else
        indeksPremikanegaObjekta = token;
    update();
}

std::vector<float> WidgetOpenGLDraw::getDrsnik(int indeks){
    return seznamObjektov[static_cast<unsigned int>(indeks)].pozicijeSliderja;
}

void WidgetOpenGLDraw::ObjTraX(int value){
    makeCurrent();
    objTraX = value;
    update();
}

void WidgetOpenGLDraw::ObjTraY(int value){
    makeCurrent();
    objTraY = value;
    update();
}

void WidgetOpenGLDraw::ObjTraZ(int value){
    makeCurrent();
    objTraZ = value;
    update();
}

void WidgetOpenGLDraw::ObjRotX(int value){
    makeCurrent();
    objRotX = value;
    update();
}

void WidgetOpenGLDraw::ObjRotY(int value){
    makeCurrent();
    objRotY = value;
    update();
}

void WidgetOpenGLDraw::ObjRotZ(int value){
    makeCurrent();
    objRotZ = value;
    update();
}

void WidgetOpenGLDraw::ObjSkaX(int value){
    makeCurrent();
    objSkaX = value;
    update();
}

void WidgetOpenGLDraw::ObjSkaY(int value){
    makeCurrent();
    objSkaY = value;
    update();
}

void WidgetOpenGLDraw::ObjSkaZ(int value){
    makeCurrent();
    objSkaZ = value;
    update();
}

void WidgetOpenGLDraw::ObdelajPonastavitevDrsnikov(std::vector<float> &drsnik){
    drsnik[0] = objTraX;
    drsnik[1] = objTraY;
    drsnik[2] = objTraZ;
    drsnik[3] = objRotX;
    drsnik[4] = objRotY;
    drsnik[5] = objRotZ;
    drsnik[6] = objSkaX;
    drsnik[7] = objSkaY;
    drsnik[8] = objSkaZ;
}

void WidgetOpenGLDraw::SetNalaganje(bool value){
    makeCurrent();
    normsBarva = value;
    update();
}
