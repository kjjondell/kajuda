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

#define FRAMES_PER_BUFFER (512)

class AudioFile{
    
public:
    
    AudioFile(const char* fname) {
        filename = fname;
        decoder = SndfileHandle(fname);
        err = decoder.error();
        
        if(err == 0){
            channels = decoder.channels();
            samplerate = decoder.samplerate();
            frames = decoder.frames();
            double_buffer = new float [FRAMES_PER_BUFFER*channels];
            buffer = new float [FRAMES_PER_BUFFER*channels];
        } else {printf("The file does not exist."); return;}
        decoder.read(double_buffer, FRAMES_PER_BUFFER*channels);
        for(int i = 0; i<FRAMES_PER_BUFFER*channels; i++){
            buffer[i] = double_buffer[i];
        }
        count = frames/2;
        
        err = Pa_Initialize();
        stream = 0;
        
        err = Pa_OpenDefaultStream
        (&stream,
         0,
         channels,
         paFloat32,
         samplerate,
         FRAMES_PER_BUFFER,
         &AudioFile::paCallback,
         this);
        
        err = Pa_SetStreamFinishedCallback( stream, &AudioFile::paStreamFinished);
        
        if(err != 0) printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
        
        max_r = 0; min_r = 0;
        max_l = 0; min_l = 0;
        
        time = (int)((frames)/samplerate);
        printf("Time of song: %i:%s%i \n",  time/60, time%60 <= 9 ? "0" : "",  time%60);
        
    }
    
    bool play (){
        if(err != 0){
            printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
            return false;
        }
        
        read = false;
        PaError err = Pa_StartStream(stream);
        // err = Pa_StopStream(stream);
        return true;
    }
    
    bool foreground(){
        if (!read){
            if(count>=frames*channels)
                return  false;
            
            /decoder = SndfileHandle(filename);
            
            for(int i = 0; i<count; i += FRAMES_PER_BUFFER*channels)
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
           printf("%i \n",  time);
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

int main(void){
    
    AudioFile af = AudioFile("/Users/kj/Desktop/newpa/newpa/test3.aif");
    
    if(af.play())
        while(af.foreground());
    
    printf("Done!");
    
    return 0;
}