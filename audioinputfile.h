#ifndef AUDIOINPUTFILE_H
#define AUDIOINPUTFILE_H

#include <iostream>
#include <math.h>
#include <stdio.h>

#include "portaudio.h"
#include <sndfile.hh>

#include <QFileDialog>
#include <QIcon>
#include <QLoggingCategory>
#include <QMessageLogger>
#include <QObject>
#include <QString>
#include <QThread>

#define FRAMES_PER_BUFFER (1000)

class AudioInputFile : public QObject {
  Q_OBJECT

public:
  char *timestring;

  // AudioFile is called when a new file is loaded, but also when changing
  // sample rate or time in the time bar.
  AudioInputFile(const char *fname);

  bool start();
  bool foreground();
  void record();
  int getTimeOfSong();
  void getFormattedTime(int time);
  void setTime(int fraction);

  int paCallbackFunction(const void *input, void *output,
                         unsigned long frameCount,
                         const PaStreamCallbackTimeInfo *timeInfo,
                         PaStreamCallbackFlags statusFlags, void *userData);

  static int paCallback(const void *input, void *output,
                        unsigned long frameCount,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags, void *userData);

  static void paStreamFinished(void *userData);

signals:
  void timeChanged(int time);
  void formatted_timeChanged(char *time);

  void l_amplitude(float amp);
  void r_amplitude(float amp);

public slots:
  void stop();

private:
  QThread thread;
  SndfileHandle encoder;
  PaStream *stream;
  int samplerate, err, channels, time, deb_count = 0;
  float time_of_song = 0.0;
  const char *filename;
  unsigned long frames, count;
  float *buffer;
  float *double_buffer;
  float *adress;
  float max_r, min_r, max_l, min_l;
  bool read;
};


#endif // AUDIOINPUTFILE_H
