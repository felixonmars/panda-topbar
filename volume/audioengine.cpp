#include "audioengine.h"

#include "audiodevice.h"

#include <QMetaType>
#include <QtDebug>

AudioEngine::AudioEngine(QObject *parent) :
    QObject(parent)
{
}

AudioEngine::~AudioEngine()
{
    qDeleteAll(m_sinks);
    m_sinks.clear();
}

int AudioEngine::volumeBounded(int volume, AudioDevice* device) const
{
    int maximum = volumeMax(device);
    double v = ((double) volume / 100.0) * maximum;
    double bounded = qBound<double>(0, v, maximum);
    return qRound((bounded / maximum) * 100);
}


void AudioEngine::mute(AudioDevice *device)
{
    setMute(device, true);
}

void AudioEngine::unmute(AudioDevice *device)
{
    setMute(device, false);
}

void AudioEngine::setIgnoreMaxVolume(bool ignore)
{
    Q_UNUSED(ignore)
}
