/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "levelmeter.h"

#include <math.h>

#include <QPainter>
#include <QTimer>
#include <QDebug>


// Constants
const int RedrawInterval = 100; // ms
const float PeakDecayRate = 0.001;
const int PeakHoldLevelDuration = 2000; // ms


LevelMeter::LevelMeter(QWidget *parent)
    :   QWidget(parent)
    ,   rmsLevel(0.0)
    ,   peakLevel(0.0)
    ,   decayedPeakLevel(0.0)
    ,   peakDecayRate(PeakDecayRate)
    ,   peakHoldLevel(0.0)
    ,   redrawTimer(new QTimer(this))
    ,   rmsColor(Qt::red)
    ,   peakColor(255, 200, 200, 255)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(30);

    connect(redrawTimer, SIGNAL(timeout()), this, SLOT(redrawTimerExpired()));
    redrawTimer->start(RedrawInterval);
}

LevelMeter::~LevelMeter()
{

}

void LevelMeter::reset()
{
    rmsLevel = 0.0;
    peakLevel = 0.0;
    update();
}

void LevelMeter::levelChanged(float rmsLevel, float peakLevel, int numSamples)
{
    // Smooth the RMS signal
    const float smooth = pow(float(0.9), static_cast<float>(numSamples) / 256); // TODO: remove this magic number

    rmsLevel = (rmsLevel * smooth) + (rmsLevel * (1.0 - smooth));

    if (peakLevel > decayedPeakLevel) {
        peakLevel = peakLevel;
        decayedPeakLevel = peakLevel;
        peakLevelChanged.start();
    }

    if (peakLevel > peakHoldLevel) {
        peakHoldLevel = peakLevel;
        peakHoldLevelChanged.start();
    }

    update();
}

void LevelMeter::redrawTimerExpired()
{
    // Decay the peak signal
    const int elapsedMs = peakLevelChanged.elapsed();
    const float decayAmount = peakDecayRate * elapsedMs;
    if (decayAmount < peakLevel)
        decayedPeakLevel = peakLevel - decayAmount;
    else
        decayedPeakLevel = 0.0;

    // Check whether to clear the peak hold level
    if (peakHoldLevelChanged.elapsed() > PeakHoldLevelDuration)
        peakHoldLevel = 0.0;

    update();
}

void LevelMeter::paintEvent(QPaintEvent *event)
{




    Q_UNUSED(event)



    QPainter painter(this);
//    painter.fillRect(rect(), Qt::black);

    QRect bar = rect();

    bar.setTop(rect().top() + (1.0 - peakHoldLevel) * rect().height());
    bar.setBottom(bar.top() + 5);
    painter.fillRect(bar, rmsColor);
    bar.setBottom(rect().bottom());

    bar.setTop(rect().top() + (1.0 - decayedPeakLevel) * rect().height());
    painter.fillRect(bar, peakColor);

    bar.setTop(rect().top() + (1.0 - rmsLevel) * rect().height());
    painter.fillRect(bar, rmsColor);


//    painter.fillRect(rect(), Qt::black);

//    QRect bar = rect();

////    bar.setTop(rect().top() + (1.0 - peakHoldLevel) * rect().height());
////    bar.setBottom(bar.top() + 5);
////    bar.setBottom(rect().bottom());

////    bar.setTop(rect().top() + (1.0 - decayedPeakLevel) * rect().height());

//    bar.setTop(rect().top() + (1.0 - rmsLevel) * rect().height());
//    bar.setBottom(rect().bottom());
//    QPainter painter(this);
////    painter.fillRect(bar, rmsColor);
////    painter.fillRect(bar, peakColor);

//    painter.fillRect(bar, rmsColor);
}
