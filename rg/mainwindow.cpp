#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
	ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_translacijaX_valueChanged(int value)
{
    ui->widget->KameraTranslacijaX(value);
    std::cout << "kval: " << value << std::endl;
}

void MainWindow::on_translacijaY_valueChanged(int value)
{
    ui->widget->KameraTranslacijaY(value);
}

void MainWindow::on_translacijaZ_valueChanged(int value)
{
    ui->widget->KameraTranslacijaZ(value);
}

void MainWindow::on_rotacijaX_valueChanged(int value)
{
    ui->widget->KameraRotacijaX(value);
}

void MainWindow::on_rotacijaY_valueChanged(int value)
{
    ui->widget->KameraRotacijaY(value);
}

void MainWindow::on_rotacijaZ_valueChanged(int value)
{
    ui->widget->KameraRotacijaZ(value);
}

void MainWindow::on_actionPerspektivna_triggered()
{
    ui->widget->PerspektivnaProj();
}

void MainWindow::on_actionParalelna_triggered()
{
    ui->widget->ParalelnaProj();
}

void MainWindow::on_btnIzberiObj_clicked()
{
    // Odpri FileDialog.

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("OBJ datoteka (*.obj)"));
    dialog.setViewMode(QFileDialog::Detail);

    QStringList potiDatotek;
    if (dialog.exec())
        potiDatotek = dialog.selectedFiles();
    ui->widget->setUvozObjektov(potiDatotek[0].toStdString());

    std::string base_filename = potiDatotek[0].toStdString().substr(potiDatotek[0].toStdString().find_last_of("/") + 1);
    ui->cBoxIzbiraZaTranslacijo->addItem(QString::number(stevecObjektov) + "|" + QString::fromStdString(base_filename));
    stevecObjektov++;
}

void MainWindow::on_cBoxIzbiraZaTranslacijo_activated(const QString &arg1)
{
    //std::cout << arg1.toStdString() << std::endl;
    // Naloži zadnjo znano pozicijo za objekt z trenutnim indeksom.
    std::string delimiter = "|";
    std::string token = arg1.toStdString().substr(0, arg1.toStdString().find(delimiter));

    int indeks = std::stoi(token);
    if(indeks >= 0){
        // Obnovi drsnike za izbrani objekt.
        std::vector<float> drs = ui->widget->getDrsnik(indeks);
        ui->hSxTO->setValue(static_cast<int>(drs[0]));
        ui->hSyTO->setValue(static_cast<int>(drs[1]));
        ui->hSzTO->setValue(static_cast<int>(drs[2]));
        ui->hSxRO->setValue(static_cast<int>(drs[3]));
        ui->hSyRO->setValue(static_cast<int>(drs[4]));
        ui->hSzRO->setValue(static_cast<int>(drs[5]));
        ui->hSxSO->setValue(static_cast<int>(drs[6]));
        ui->hSySO->setValue(static_cast<int>(drs[7]));
        ui->hSzSO->setValue(static_cast<int>(drs[8]));
    }


    ui->widget->izberiObjektPremika(arg1.toStdString());
}

void MainWindow::on_hSxTO_valueChanged(int value)
{
    ui->widget->ObjTraX(value);
}

void MainWindow::on_hSyTO_valueChanged(int value)
{
    ui->widget->ObjTraY(value);
}

void MainWindow::on_hSzTO_valueChanged(int value)
{
    ui->widget->ObjTraZ(value);
}

void MainWindow::on_hSxRO_valueChanged(int value)
{
    ui->widget->ObjRotX(value);
}

void MainWindow::on_hSyRO_valueChanged(int value)
{
    ui->widget->ObjRotY(value);
}

void MainWindow::on_hSzRO_valueChanged(int value)
{
    ui->widget->ObjRotZ(value);
}

void MainWindow::on_hSxSO_valueChanged(int value)
{
    ui->widget->ObjSkaX(value);
}

void MainWindow::on_hSySO_valueChanged(int value)
{
    ui->widget->ObjSkaY(value);
}

void MainWindow::on_hSzSO_valueChanged(int value)
{
    ui->widget->ObjSkaZ(value);
}

void MainWindow::on_cBox_stateChanged(int arg1)
{
    if(arg1 == 2) // CheckBox izbran, nastavi false.
        ui->widget->SetNalaganje(false);
    else
        ui->widget->SetNalaganje(true);
}
