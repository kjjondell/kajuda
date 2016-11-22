#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QIcon>
#include <QLoggingCategory>
#include <QMessageLogger>
#include <thread>

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
//    TODO: Check filename
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Audio File"), "~");
    filename = fileName.toStdString();
    const char* f_name = filename.c_str();
  // printf(f_name);
    af = new AudioFile(f_name,0);
   // printf("\n");
   // printf(af->filename);
    QObject::connect(af, &AudioFile::timeChanged,
                     this->ui->sliderTrackPos, &QSlider::setValue);

    QObject::connect(af, &AudioFile::formatted_timeChanged,
                     this->ui->labelTrackTime, &QLabel::setText);


    this->ui->labelInfo->setText(f_name);
    this->ui->labelTrackTime->setText(af->timestring);
    this->ui->labelTrackTime->update();
   // this->setTime(af->getTimeOfSong());
    }

void MainWindow::on_buttonRecord_clicked()
{

}

void MainWindow::on_buttonPlay_clicked()
{

//    TODO: Check that af was correctly initialized

    static bool isPlaying = false;

    QString icon;
    if (isPlaying) {
        icon = ":/ic_play_arrow_black_48dp.png";
        af->stop();

    } else {

        icon = ":/ic_pause_black_48dp.png";
        std::thread t1(AudioFile::play, af);
        t1.detach();

    }

    isPlaying = !isPlaying;
    this->ui->buttonPlay->setIcon(QIcon(icon));

    qDebug() << isPlaying;
}

void MainWindow::on_sliderTrackPos_sliderReleased()
{
  af->setTime(this->ui->sliderTrackPos->value());
  //qDebug() << af->timestring;
  this->ui->labelTrackTime->setText(af->timestring);
  this->ui->labelTrackTime->update();
}
