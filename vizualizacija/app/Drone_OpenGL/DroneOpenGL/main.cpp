#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[]){
    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(glFormat);

    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
