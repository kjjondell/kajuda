#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QIcon>
#include <QLoggingCategory>
#include <QMessageLogger>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Audio File"), "~");

}

void MainWindow::on_buttonRecord_clicked()
{

}

void MainWindow::on_buttonPlay_clicked()
{
    static bool isPlaying = false;

    QString icon;
    if (isPlaying) {
        icon = ":/ic_pause_black_48dp.png";
    } else {
        icon = ":/ic_play_arrow_black_48dp.png";
    }

    isPlaying = !isPlaying;
    this->ui->buttonPlay->setIcon(QIcon(icon));

    qDebug() << isPlaying;
}
