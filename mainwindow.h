#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "input_handling.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonOpenFile_clicked();

    void on_buttonRecord_clicked();

    void on_buttonPlay_clicked();

    void on_sliderTrackPos_sliderReleased();

private:
    Ui::MainWindow *ui;
    AudioFile* af;
    std::string filename;
    int start_time;

};

#endif // MAINWINDOW_H
