#include "audiofile.h"

AudioFile::AudioFile(const char *fname, int start_time, int sample_rate) {
    moveToThread(&thread);
    thread.start();

    // TODO: Check file extension
    filename = fname;
    stream = 0;
    err = 0;
    decoder = SndfileHandle(filename);

    // qDebug() << fname;

    if (err == 0) {
        if (sample_rate == -1)
            samplerate = decoder.samplerate();
        else
            samplerate = sample_rate;

        frames = decoder.frames();
        channels = decoder.channels();
        double_buffer = new float[FRAMES_PER_BUFFER * channels];
        buffer = new float[FRAMES_PER_BUFFER * channels];
    }
    else {
        printf("The file does not exist.");
    }

    for (int i = 0; i < decoder.samplerate() * start_time; i++)
        decoder.read(buffer, channels);

    /*decoder.read(double_buffer, FRAMES_PER_BUFFER*channels);
      for (int i = 0; i<FRAMES_PER_BUFFER*channels; i++){
          buffer[i] = double_buffer[i];
      }*/

    count = 0;
    max_r = 0;
    min_r = 0;
    max_l = 0;
    min_l = 0;

    time_of_song = ((frames) / samplerate);
    setTime(0);
    //   printf("Time of song: %i:%s%i \n",  time/60, time%60 <= 9 ? "0" : "",
    //   time%60);
}

AudioFile::~AudioFile() {
    thread.quit();
    thread.wait();
}

int AudioFile::getTimeOfSong() { return time_of_song; }

bool AudioFile::start() {

    err = Pa_Initialize();
    if (err != 0) {
        printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
    }

    err = Pa_OpenDefaultStream(&stream, 0, channels, paFloat32, samplerate,
                               FRAMES_PER_BUFFER, &paCallback, this);
    if (err != 0) {
        printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
    }

    err = Pa_SetStreamFinishedCallback(stream, &paStreamFinished);
    if (err != 0) {
        printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
    }

    // decoder.seek(count,SEEK_CUR );
    decoder = SndfileHandle(filename);
    for (int i = 0; i < count; i += FRAMES_PER_BUFFER * channels)
        decoder.read(double_buffer, FRAMES_PER_BUFFER * channels);
    for (int i = 0; i < FRAMES_PER_BUFFER * channels; i++) {
        buffer[i] = double_buffer[i];
    }

    read = false;
    PaError err = Pa_StartStream(stream);

    return true;
}

void AudioFile::getFormattedTime(int time) {
    timestring = new char[10];
    sprintf(timestring,
            "-%i:%s%i", time / 60,
            time % 60 <= 9 ? "0" : "",
            time % 60);
}

void AudioFile::setTime(int fraction) {
    // printf("%i ",fraction);
    count = (frames * channels * fraction) / 1000;
    time = ((count / channels) / samplerate);
    getFormattedTime(time_of_song - time);
    // qDebug() << timestring;
    // printf("%i %i %i \n",count, time, (int)((count/channels)  / samplerate));
}

bool AudioFile::foreground() {
    emit timeChanged(time * 1000 / time_of_song);
    getFormattedTime(time_of_song - time);
    emit formatted_timeChanged(timestring);

    if (!read) {
        if (count >= frames * channels)
            return false;

        decoder.read(double_buffer, FRAMES_PER_BUFFER * channels);
        for (int i = 0; i < FRAMES_PER_BUFFER * channels; i++) {
            buffer[i] = double_buffer[i];
        }
        read = true;
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
    }

    if ((int)((count / channels) / samplerate) != time) {
        time = ((count / channels) / samplerate);
        //            printf("%i \n",  time);
    }
    return Pa_IsStreamActive(stream);
}

bool AudioFile::play() {
    if (start())
        while (foreground())
            ;
    qDebug() << "HEU";
    return true;
}

bool AudioFile::stop() {
    if (err != 0) {
        printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
    }

    PaError err = Pa_StopStream(stream);
    if (err != 0) {
        printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
    }

    err = Pa_Terminate();
    if (err != 0) {
        printf("An error has occured:\n%s\n", Pa_GetErrorText(err));
        return false;
    }

    return true;
}

int AudioFile::paCallbackFunction(const void *input,
                                  void *output,
                                  unsigned long frameCount,
                                  const PaStreamCallbackTimeInfo *timeInfo,
                                  PaStreamCallbackFlags statusFlags,
                                  void *userData) {
    float *out = (float *)output;

    unsigned long i;
    (void)input;

    for (i = 0; i < frameCount * channels; i++) {
        *out++ = buffer[i];
        count++;
    }

    read = false;

    return count >= frames * channels ? paComplete : paContinue;
}

void AudioFile::paStreamFinished(void *userData) { printf("End of song!"); }

int AudioFile::paCallback(const void *input, void *output,
                          unsigned long frameCount,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData) {
    return ((AudioFile *)userData)
            ->paCallbackFunction(input, output, frameCount, timeInfo, statusFlags,
                                 userData);
}
