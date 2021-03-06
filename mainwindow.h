#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audiofile.h"
#include "audioinputfile.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void startPlayback();
    void stopPlayback();
    void startRecord();
private slots:
    void on_buttonOpenFile_clicked();

    void on_buttonRecord_clicked();

    void on_buttonPlay_clicked();

    void on_applySampleRate_clicked();

    void on_sliderTrackPos_sliderReleased();

    void on_sliderTrackPos_sliderPressed();

private:

    void moveSlider(int time);
    void moveLMeter(float rms, float peak);
    void moveRMeter(float rms, float peak);
    void openNewFile(int sampleRate=-1);

    Ui::MainWindow *ui;
    AudioFile* af;
    AudioInputFile* aif;
    std::string filename;
    QString recordName;
    int start_time;
    bool slider_pressed, isPlaying;

};

#endif // MAINWINDOW_H
