#include "audioinputfile.h"

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

#define FRAMES_PER_BUFFER (512)

AudioInputFile::AudioInputFile(const char *fname) {
    moveToThread(&thread);
    thread.start();

    channels = 1;
    samplerate = 44100;
//    frames = samplerate * 5 * channels;
//    buffer = new float[frames]
    buffer = new float[FRAMES_PER_BUFFER * channels];
    encoder = SndfileHandle(fname, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT,
                            channels, samplerate);
    err = Pa_Initialize();
    stream = 0;
    count = 0;
    read = false;
}

bool AudioInputFile::start() {
    err = Pa_OpenDefaultStream(&stream, channels, 0, paFloat32, samplerate, FRAMES_PER_BUFFER,
                               &AudioInputFile::paCallback, this);

    Pa_StartStream(stream);
    return true;
}

bool AudioInputFile::foreground() {
    if ((int)((count / channels) / (samplerate)) != time) {
        time = (int)((count / channels) / (samplerate));
        qDebug() << time;
    }
    if (read) {
        read = false;
        encoder.write(buffer, FRAMES_PER_BUFFER * channels);
        for (int i = 1; i < FRAMES_PER_BUFFER * channels; i += channels) {
            if (max_r < buffer[i])
                max_r = buffer[i];
            if (min_r > buffer[i])
                min_r = buffer[i];
        }

        for (int i = 0; i < FRAMES_PER_BUFFER * channels; i += channels) {
            if (max_l < buffer[i])
                max_l = buffer[i];
            if (min_l > buffer[i])
                min_l = buffer[i];
        }

        float x = log10((max_r - min_r) / 2);
        emit l_amplitude(1.0 + x);
        float y = log10((max_l - min_l) / 2);
        emit r_amplitude(1.0 + y);
        // printf("%f %f\n", x, y);
        max_r = 0;
        min_r = 0;
        max_l = 0;
        min_l = 0;
//        buffer = new float[frames];
    }
    return true;
}

void AudioInputFile::stop() {
    Pa_StopStream(stream);
    Pa_Terminate();
//    encoder
}

void AudioInputFile::record() {
    if(start())
        while (foreground())
            ;
}

int AudioInputFile::paCallbackFunction(const void *input, void *output,
                                       unsigned long frameCount,
                                       const PaStreamCallbackTimeInfo *timeInfo,
                                       PaStreamCallbackFlags statusFlags,
                                       void *userData) {
    float *in = (float *)input;
    //  printf("%i \n", count);

    unsigned long i;
    (void)output;

    for (i = 0; i < frameCount * channels; i++) {
        buffer[i] = *in++;
        count++;
    }

    read = true;

    return paContinue;
}

void AudioInputFile::paStreamFinished(void *userData) {
    printf("End of song!");
}

int AudioInputFile::paCallback(const void *input, void *output,
                                      unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo *timeInfo,
                                      PaStreamCallbackFlags statusFlags,
                                      void *userData) {
    return ((AudioInputFile *)userData)
            ->paCallbackFunction(input, output, frameCount, timeInfo, statusFlags,
                                 userData);
}
