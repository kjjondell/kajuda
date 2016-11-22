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
    slider_pressed = false;
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
                     this, &MainWindow::moveSlider);

    QObject::connect(af, &AudioFile::formatted_timeChanged,
                     this->ui->labelTrackTime, &QLabel::setText);

    QObject::connect(af, &AudioFile::l_amplitude,
                     this, &MainWindow::moveLMeter);

    QObject::connect(af, &AudioFile::r_amplitude,
                     this, &MainWindow::moveRMeter);

    this->ui->labelInfo->setText(f_name);
    this->ui->labelTrackTime->setText(af->timestring);
    this->ui->labelTrackTime->update();
   // this->setTime(af->getTimeOfSong());
    }

void MainWindow::moveLMeter(float amp){
    qDebug()<<amp;
    this->ui->volmeter1->levelChanged(amp,amp,1000);
}

void MainWindow::moveRMeter(float amp){
    this->ui->volmeter2->levelChanged(amp,amp,1000);
}

void MainWindow::moveSlider(int time){
    if(!slider_pressed)
        this->ui->sliderTrackPos->setValue(time);
}

void MainWindow::on_buttonRecord_clicked()
{

}

void MainWindow::on_buttonPlay_clicked()
{

//    TODO: Check that af was correctly initialized

    //static bool isPlaying = false;

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

//    qDebug() << isPlaying;
}

void MainWindow::on_sliderTrackPos_sliderReleased()
{
    slider_pressed = false;
    if(isPlaying)
        af->stop();
  af->setTime(this->ui->sliderTrackPos->value());
  //qDebug() << af->timestring;
  this->ui->labelTrackTime->setText(af->timestring);
  this->ui->labelTrackTime->update();
  if(isPlaying){
      std::thread t1(AudioFile::play, af);
      t1.detach();
  }
}

void MainWindow::on_sliderTrackPos_sliderPressed(){
    slider_pressed = true;
}
