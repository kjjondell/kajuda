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
    buffer = new Buffer(2, FRAMES_PER_BUFFER * channels);
    encoder = SndfileHandle(fname, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT,
                            channels, samplerate);
    err = Pa_Initialize();
    stream = 0;
    count = 0;
    read = false;
}

bool AudioInputFile::start() {
    for (int i = 0; i < buffer->getNbUnits(); i += 1){
        buffer->setEmpty(i);
    }
    buffer_index_read = 0;
    buffer_index_write = 0;

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
    if (buffer->isFull(buffer_index_read)) {
        float* bufferUnit = buffer->getUnit(buffer_index_read);
        encoder.write(bufferUnit, buffer->getUnitSize());
        buffer->setEmpty(buffer_index_read);
        for (int i = 1; i < buffer->getUnitSize(); i += channels) {
            if (max_r < bufferUnit[i])
                max_r = bufferUnit[i];
            if (min_r > bufferUnit[i])
                min_r = bufferUnit[i];
        }

        for (int i = 0; i < buffer->getUnitSize(); i += channels) {
            if (max_l < bufferUnit[i])
                max_l = bufferUnit[i];
            if (min_l > bufferUnit[i])
                min_l = bufferUnit[i];
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
    buffer_index_read = (buffer_index_read + 1) % buffer->getNbUnits();
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
    if(!buffer->isFull(buffer_index_write)){
        float* bufferUnit = buffer->getUnit(buffer_index_write);
        for (i = 0; i < buffer->getUnitSize(); i++) {
            bufferUnit[i] = *in++;
            count++;
        }
        buffer->setFull(buffer_index_write);
    }
    buffer_index_write = (buffer_index_write + 1) % buffer->getNbUnits();

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
