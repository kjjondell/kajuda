//
//  main2.cpp
//  newpa
//
//  Created by kj jondell on 07/11/16.
//  Copyright © 2016 kj jondell. All rights reserved.
//

#include <stdio.h>
#include "portaudio.h"
#include <iostream>
#include <math.h>
#include <sndfile.hh>

#include <QObject>
#include <QString>
#include <QFileDialog>
#include <QIcon>
#include <QLoggingCategory>
#include <QMessageLogger>
#include <QThread>

#define FRAMES_PER_BUFFER (1000)



class AudioInputFile : public QObject {
    Q_OBJECT
public:
    
    AudioInputFile (const char* fname){
        channels = 1 ;
        samplerate = 44100 ;
         frames = samplerate*5*channels;
        buffer = new float[frames];
        encoder = SndfileHandle (fname, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, channels, samplerate);
        err = Pa_Initialize();
        stream = 0;
        count = 0;
    }

    bool start(){
        err = Pa_OpenDefaultStream
        (&stream,
         channels,
         0,
         paFloat32,
         samplerate,
         512,
         &AudioInputFile::paCallback,
         this);

    Pa_StartStream(stream);
    return true;
    }

    bool foreground(){
            if ( (int)((count/channels) / (samplerate)) != time){
                time = (int)((count/channels)  / (samplerate));
                qDebug()<<time;
            }
            if(read){
                read = false;
                encoder.write(buffer, 512*channels);
                for ( int i = 1 ; i < FRAMES_PER_BUFFER*channels; i+=channels)
                {
                    if (max_r < buffer[i]) max_r = buffer[i];
                    if (min_r > buffer[i]) min_r = buffer[i];
                }

                for ( int i = 0 ; i < FRAMES_PER_BUFFER*channels; i+=channels)
                {
                    if (max_l < buffer[i]) max_l = buffer[i];
                    if (min_l > buffer[i]) min_l = buffer[i];
                }

                float x = log10((max_r-min_r)/2);
                emit l_amplitude(1.0+x);
                float y = log10((max_l-min_l)/2);
                emit r_amplitude(1.0+y);
                //printf("%f %f\n", x, y);
                max_r = 0; min_r = 0;
                max_l = 0; min_l = 0;
                buffer = new float[frames];
            }
        return true;
    }

    void stop(){
        Pa_StopStream(stream);
        Pa_Terminate();
    }
    
    static void record(AudioInputFile* af){
        if(af->start())
            while(af->foreground());
        af->stop();
    }

signals:
    void l_amplitude(float amp);
    void r_amplitude(float amp);

private:
    
    int paCallbackFunction(const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData){
        float *in = (float*)input;
        //  printf("%i \n", count);

        
        unsigned long i;
        (void)output;
        
        for (i = 0; i < frameCount*channels; i++){
            buffer[i] = *in++;
            count++;
        }
        
        read = true;
        
        return paContinue;
    }
    
    static void paStreamFinished(void* userData){
        printf("End of song!");
    }
        
        static int paCallback ( const void *input,
                               void *output,
                               unsigned long frameCount,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData ) {
            return ((AudioInputFile*)userData)->paCallbackFunction(input, output, frameCount, timeInfo, statusFlags, userData);
        }
        
        SndfileHandle encoder;
        PaStream* stream;
        int samplerate, err, channels, time;
        const char* filename;
        unsigned long frames, count;
        float* buffer;
        float* double_buffer;
        float* adress;
        float max_r, min_r, max_l, min_l;
        bool read;
        
    
};


//int main(void){

//    AudioFile af = AudioFile("/home/julien/Musique/Mixxx/Demon_You_are_my_high.wav");
//   // printf("START!");
//    //AudioInputFile aif = AudioInputFile("/Users/kj/Desktop/newpa/newpa/test_in.wav");

//  //  printf("STOP!");

//    if(af.play())
//      while(af.foreground());

//    printf("Done!");

//    return 0;
//}
