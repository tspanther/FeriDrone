#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <iostream>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow{
	Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	private slots:
        void on_translacijaX_valueChanged(int value);

        void on_translacijaY_valueChanged(int value);

        void on_translacijaZ_valueChanged(int value);

        void on_rotacijaX_valueChanged(int value);

        void on_rotacijaY_valueChanged(int value);

        void on_rotacijaZ_valueChanged(int value);

        void on_actionPerspektivna_triggered();

        void on_actionParalelna_triggered();

        void on_btnIzberiObj_clicked();

        void on_hSxTO_valueChanged(int value);

        void on_cBoxIzbiraZaTranslacijo_activated(const QString &arg1);

        void on_hSyTO_valueChanged(int value);

        void on_hSzTO_valueChanged(int value);

        void on_hSxRO_valueChanged(int value);

        void on_hSyRO_valueChanged(int value);

        void on_hSzRO_valueChanged(int value);

        void on_hSxSO_valueChanged(int value);

        void on_hSySO_valueChanged(int value);

        void on_hSzSO_valueChanged(int value);

        void on_cBox_stateChanged(int arg1);

private:
	Ui::MainWindow *ui;
    unsigned int stevecObjektov = 0;
};

#endif // MAINWINDOW_H
