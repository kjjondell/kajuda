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
    ui(new Ui::MainWindow),
    isPlaying(false),
    af(nullptr)
{
    ui->setupUi(this);
    slider_pressed = false;
    filename= std::string("../kajuda/res/curve.wav");
    openNewFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openNewFile(int sampleRate){


    const char* f_name = this->filename.c_str();
    if(af!=nullptr){
        QString icon;
        if (isPlaying) {
            icon = ":/ic_play_arrow_black_48dp.png";
            af->stop();
            isPlaying = !isPlaying;
            this->ui->buttonPlay->setIcon(QIcon(icon));
        }
        delete af;
    }
    af = new AudioFile(f_name,0,sampleRate);
    this->ui->spinBoxSampleRate->setValue(af->getSampleRate());

    connect(this, &MainWindow::startPlayback, af, &AudioFile::play);
    connect(this, &MainWindow::stopPlayback, af, &AudioFile::stop);
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

void MainWindow::on_buttonOpenFile_clicked()
{
//    TODO: Check filename
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Audio File"), "~");

    filename = fileName.toStdString();
    const char* f_name = this->filename.c_str();
    if(strcmp(f_name,"")!=0)
        this->openNewFile();
}

void MainWindow::on_applySampleRate_clicked()
{
        this->openNewFile(this->ui->spinBoxSampleRate->value());
}


void MainWindow::moveLMeter(float rms, float peak){
   this->ui->volmeter1->levelChanged(rms, peak, 512);
}

void MainWindow::moveRMeter(float rms, float peak){
    this->ui->volmeter2->levelChanged(rms, peak, 512);
}

void MainWindow::moveSlider(int time){
    if(!slider_pressed)
        this->ui->sliderTrackPos->setValue(time);
}

void MainWindow::on_buttonRecord_clicked()
{
    static bool isRecording = false;
    if(!isRecording) {
        QString filename = "../kajuda/res/rec" + QDateTime::currentDateTime().toString("-yyyyMMdd-hhmmss") + ".wav";
        aif = new AudioInputFile(filename.toStdString().c_str());

        QObject::connect(aif, &AudioInputFile::l_amplitude,
                         this, &MainWindow::moveLMeter);

        QObject::connect(aif, &AudioInputFile::r_amplitude,
                         this, &MainWindow::moveRMeter);

        QObject::connect(this, &MainWindow::startRecord, aif, &AudioInputFile::record);

        emit startRecord();
    } else {
        aif->stop();
    }

    isRecording = !isRecording;
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
        emit startPlayback();

    }

    isPlaying = !isPlaying;
    this->ui->buttonPlay->setIcon(QIcon(icon));

//    qDebug() << isPlaying;
}

void MainWindow::on_sliderTrackPos_sliderReleased()
{
    slider_pressed = false;
    if(isPlaying){
        af->stop();
    }
  af->setTime(this->ui->sliderTrackPos->value());
  //qDebug() << af->timestring;
  this->ui->labelTrackTime->setText(af->timestring);
  this->ui->labelTrackTime->update();
  if(isPlaying){
      emit startPlayback();
  }
}

void MainWindow::on_sliderTrackPos_sliderPressed(){
    slider_pressed = true;
}

