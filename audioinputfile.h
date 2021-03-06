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
#include "buffer.h"

#define FRAMES_PER_BUFFER (128)

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

  void l_amplitude(float rms, float peak);
  void r_amplitude(float rms, float peak);

public slots:
  void stop();

private:
  QThread thread;
  SndfileHandle encoder;
  PaStream *stream;
  int samplerate, err, channels, time, deb_count = 0;
  int buffer_index_write, buffer_index_read =0;
  float time_of_song = 0.0;
  const char *filename;
  unsigned long frames, count;
  Buffer *buffer;
  float *adress;
  float max_r, min_r, max_l, min_l;
  bool read;
};


#endif // AUDIOINPUTFILE_H
