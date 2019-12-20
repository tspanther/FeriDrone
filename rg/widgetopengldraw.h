#ifndef WIDGETOPENGLDRAW_H
#define WIDGETOPENGLDRAW_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include <glm/glm.hpp>

class QOpenGLFunctions_3_3_Core;
class WidgetOpenGLDraw : public QOpenGLWidget{
public:
	WidgetOpenGLDraw(QWidget* parent);

    ~WidgetOpenGLDraw() override;
    void KameraTranslacijaX(int value);
    void KameraTranslacijaY(int value);
    void KameraTranslacijaZ(int value);
    void KameraRotacijaX(int value);
    void KameraRotacijaY(int value);
    void KameraRotacijaZ(int value);
    void PerspektivnaProj();
    void ParalelnaProj();

    // Upravljanje z izbranim objektom.
    void ObjTraX(int value);
    void ObjTraY(int value);
    void ObjTraZ(int value);

    void ObjRotX(int value);
    void ObjRotY(int value);
    void ObjRotZ(int value);

    void ObjSkaX(int value);
    void ObjSkaY(int value);
    void ObjSkaZ(int value);


    void setUvozObjektov(std::string potiDatotek);
    void izberiObjektPremika(std::string vhod);
    std::vector<float> getDrsnik(int indeks);
    void SetNalaganje(bool value);

private:
	void PrevediSencilnike();
	void printProgramInfoLog(GLuint obj);
	void printShaderInfoLog(GLuint obj);
    void readFile(std::string path, std::string &save);
    void ObdelajPonastavitevDrsnikov(std::vector<float> &drsnik);

    QOpenGLFunctions_3_3_Core* gl;
	unsigned int id_sencilni_program;
    std::vector<std::string> izbraneDatoteke; // Tukaj hranim poti do naloženih objektov.

    struct Tocka{
        glm::vec3 pozicija;
        glm::vec3 barva;
    } tocka;

    struct objekt{
        GLuint objektVAO;
        unsigned int bufferVAO;
        unsigned int steviloOglisc;
        glm::mat4 lokacija;
        glm::mat4 zadnjaLokacija;
        std::vector<float> pozicijeSliderja;
    } obj;

    // Seznam objektov.
    std::vector<objekt> seznamObjektov;

    // Podatki uvoza.
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;

    // Kamera
    int kamTraX=0;
    int kamTraY=0;
    int kamTraZ=-3;
    int kamRotX = 0;
    int kamRotY = 0;
    int kamRotZ = 0;

    // Objekt
    int objTraX = 0, objTraY = 0, objTraZ = 0;
    int objRotX = 0, objRotY = 0, objRotZ = 0;
    float objSkaX = 50, objSkaY = 50, objSkaZ = 50;

    // Ostalo
    bool jePerspektivna = true;
    bool normsBarva = true;
    std::string indeksPremikanegaObjekta = "-1";
protected:
	void paintGL() override;
	void initializeGL() override;
	void resizeGL(int w, int h) override;
signals:

public slots:

};

#endif // WIDGETOPENGLDRAW_H
