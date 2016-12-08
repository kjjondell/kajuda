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
        buffer = new Buffer(2, FRAMES_PER_BUFFER * channels);

    }
    else {
        printf("The file does not exist.");
    }

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



    for (int i = 0; i < buffer->getNbUnits(); i += 1){
        decoder.read(buffer->getUnit(i), buffer->getUnitSize());
        buffer->setFull(i);
    }
    buffer_index_read = 0;
    buffer_index_write = 0;

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

int AudioFile::getSampleRate(){
    return samplerate;
}

void AudioFile::setTime(int fraction) {
    // printf("%i ",fraction);
    count = (frames * channels * fraction) / 1000;
    sf_count_t cnt = count/2;
    decoder.seek(cnt, SEEK_SET);

    time = ((count / channels) / samplerate);
    getFormattedTime(time_of_song - time);
    // qDebug() << timestring;
    // printf("%i %i %i \n",count, time, (int)((count/channels)  / samplerate));
}

bool AudioFile::foreground() {
    emit timeChanged(time * 1000 / time_of_song);
    getFormattedTime(time_of_song - time);
    emit formatted_timeChanged(timestring);

    if (!buffer->isFull(buffer_index_write)) {
        if (count >= frames * channels)
            return false;

        float* bufferUnit = buffer->getUnit(buffer_index_write);
        decoder.read(bufferUnit, buffer->getUnitSize());

        buffer->setFull(buffer_index_write);

        float l_sum = 0.0;
        float r_sum = 0.0;

        for (int i = 1; i < buffer->getUnitSize(); i += channels) {
            float val = bufferUnit[i];

            r_sum += val * val;

            if (max_r < val)
                max_r = val;

            if (min_r > val)
                min_r = val;
        }

        for (int i = 0; i < buffer->getUnitSize(); i += channels) {
            float val = bufferUnit[i];

            l_sum += val * val;

            if (max_l < val)
                max_l = val;

            if (min_l > val)
                min_l = val;
        }

        float r_rms = sqrt(r_sum / FRAMES_PER_BUFFER);
        float l_rms = sqrt(l_sum / FRAMES_PER_BUFFER);

        r_rms = qMax(0.0f, r_rms);
        r_rms = qMin(1.0f, r_rms);
        l_rms = qMax(0.0f, l_rms);
        l_rms = qMin(1.0f, l_rms);

        max_r = max_r * max_r;
        max_l = max_l * max_l;

        emit r_amplitude(r_rms, max_r);
        emit l_amplitude(l_rms, max_l);

        max_r = 0;
        min_r = 0;
        max_l = 0;
        min_l = 0;
    }

    buffer_index_write = (buffer_index_write + 1) % buffer->getNbUnits();
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
    if(buffer->isFull(buffer_index_read)){
        float* bufferUnit = buffer->getUnit(buffer_index_read);
        for (i = 0; i < buffer->getUnitSize(); i++) {
            *out++ = bufferUnit[i];
            count++;
        }

        buffer->setEmpty(buffer_index_read);
    }
    buffer_index_read = (buffer_index_read + 1) % buffer->getNbUnits();
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
