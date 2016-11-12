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

class AudioFile{
    
public:
    
    AudioFile(const char* filename) {
        
        decoder = SndfileHandle(filename);
        err = decoder.error();
        
        if(err == 0){
            channels = decoder.channels();
            samplerate = decoder.samplerate();
            frames = decoder.frames();
            buffer = new float [frames*channels];
            double_buffer = new float[frames*channels];
        } else {printf("The file does not exist."); return;}
        decoder.read(buffer, frames*channels);
        count = 0;
        
        err = Pa_Initialize();
        
        PaStreamParameters params;
        params.device = Pa_GetDefaultOutputDevice();
        
        params.channelCount = channels;       /* stereo output */
        params.sampleFormat = paFloat32; /* 32 bit floating point output */
        
        err = Pa_OpenStream
        (&stream,
         NULL,
         &params,
         samplerate,
         256, NULL,
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
            
            read = true;
            
            for ( int i = count+1 ; i < count+256*channels; i+=2)
            {
                if (max_r < buffer[i]) max_r = buffer[i];
                if (min_r > buffer[i]) min_r = buffer[i];
            }
            
            for ( int i = count ; i < count+256*channels; i+=2)
            {
                if (max_l < buffer[i]) max_l = buffer[i];
                if (min_l > buffer[i]) min_l = buffer[i];
            }
            
            float x = 20*log10((max_r-min_r)/2);
            float y = 20*log10((max_l-min_l)/2);
            printf("%f %f\n", x, y);
            max_r = 0; min_r = 0;
            max_l = 0; min_l = 0;
        }
        if ( (int)((count/channels) / samplerate) != time){
            time = (int)((count/channels)  / samplerate);
           //printf("%i \n",  time);
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
        
        for (i = 0; i < frameCount*channels; i++)
            *out++ = buffer[count++];
        
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
    unsigned long frames, count;
    float* buffer;
    float* adress;
    float max_r, min_r, max_l, min_l;
    bool read;
    
};

int main(void){
    
    AudioFile af = AudioFile("/Users/kj/Desktop/newpa/newpa/test2.wav");
    
    if(af.play())
        while(af.foreground());
    
    printf("Done!");
    
    return 0;
}