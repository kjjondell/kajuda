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

#define FRAMES_PER_BUFFER (1000)




class AudioFile{
    
public:

    //AudioFile is called when a new file is loaded, but also when changing sample rate or time in the time bar.
    AudioFile(const char* fname, int start_time, int sample_rate =-1 ) {
        //TODO: Check file extension
        filename = fname;
        stream = 0;
        decoder = SndfileHandle(filename);

        if(err == 0){
            channels = decoder.channels();
            if (sample_rate==-1)
                samplerate = decoder.samplerate();
            else samplerate = sample_rate;
            frames = decoder.frames();
            double_buffer = new float [FRAMES_PER_BUFFER*channels];
            buffer = new float [FRAMES_PER_BUFFER*channels];
        }

        else {
            printf("The file does not exist.");
            return;
        }

        count = 0;
        for(int i = 0; i < decoder.samplerate() * start_time; i++)
            decoder.read(buffer, channels);

        decoder.read(double_buffer, FRAMES_PER_BUFFER*channels);
        for (int i = 0; i<FRAMES_PER_BUFFER*channels; i++){
            buffer[i] = double_buffer[i];
        }

        count = 0;
        max_r = 0;
        min_r = 0;
        max_l = 0;
        min_l = 0;
        
        time = (int)((frames)/samplerate);
        printf("Time of song: %i:%s%i \n",  time/60, time%60 <= 9 ? "0" : "",  time%60);
        
    }
    
    bool start (){

        err = Pa_Initialize();


        err = Pa_OpenDefaultStream
        (&stream,
         0,
         channels,
         paFloat32,
         samplerate,
         FRAMES_PER_BUFFER,
         &AudioFile::paCallback,
         this);

        if(err != 0) printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
        err = Pa_SetStreamFinishedCallback( stream, &AudioFile::paStreamFinished);

        if(err != 0){
            printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
         }

        read = false;
        PaError err = Pa_StartStream(stream);
        // err = Pa_StopStream(stream);
        return true;
    }

    static bool play(AudioFile* af){
        if(af->start())
            while(af->foreground());

        return true;
    }

    bool stop (){
        if(err != 0){
            printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
            return false;
        }

        PaError err = Pa_StopStream(stream);
        err = Pa_Terminate();
        return true;
    }


    bool foreground(){
        if (!read){
            if(count>=frames*channels)
                return  false;
            
            decoder.read(double_buffer, FRAMES_PER_BUFFER*channels);
            for(int i = 0; i<FRAMES_PER_BUFFER*channels; i++){
                buffer[i] = double_buffer[i];
            }
            read = true;
            
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
            
            float x = 20*log10((max_r-min_r)/2);
            float y = 20*log10((max_l-min_l)/2);
            //printf("%f %f\n", x, y);
            max_r = 0; min_r = 0;
            max_l = 0; min_l = 0;
        }
        if ( (int)((count/channels) / samplerate) != time){
            time = (int)((count/channels)  / samplerate);
//            printf("%i \n",  time);
        }
        return Pa_IsStreamActive(stream);
    }
    


private:
    
    int paCallbackFunction(const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData){
        float *out = (float*)output;
        
        unsigned long i;
        (void)input;
        
        for (i = 0; i < frameCount*channels; i++){
            *out++ = buffer[i];
            count++;
        }
        
        read = false;
        
        return count >= frames *channels ? paComplete : paContinue;
    }
    
    static void paStreamFinished(void* userData){printf("End of song!");}
    
    static int paCallback ( const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData ) {
        return ((AudioFile*)userData)->paCallbackFunction(input, output, frameCount, timeInfo, statusFlags, userData);
    }
    
    SndfileHandle decoder;
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

class AudioInputFile{
    
public:
    
    AudioInputFile (const char* fname){
        channels = 2 ;
        samplerate = 44100 ;
         frames = samplerate*5*channels;
        buffer = new float[frames];
        encoder = SndfileHandle (fname, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, channels, samplerate);
        err = Pa_Initialize();
        stream = 0;
        count = 0;
        
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
        while(count <= frames*channels){
            if ( (int)((count/channels) / (samplerate)) != time){
                time = (int)((count/channels)  / (samplerate));
                printf("%i \n",  time);
            }
            if(read){
                read = false;
                encoder.write(buffer, 512*channels);
            }
        }
        
    }
    
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
        
        return count >= frames *channels ? paComplete : paContinue;
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
