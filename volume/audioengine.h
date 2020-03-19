#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <QObject>
#include <QList>
#include <QTimer>

class AudioDevice;

class AudioEngine : public QObject
{
    Q_OBJECT

public:
    AudioEngine(QObject *parent = nullptr);
    ~AudioEngine();

    const QList<AudioDevice *> &sinks() const { return m_sinks; }
    virtual int volumeMax(AudioDevice *device) const = 0;
    virtual int volumeBounded(int volume, AudioDevice *device) const;
    virtual const QString backendName() const = 0;

public slots:
    virtual void commitDeviceVolume(AudioDevice *device) = 0;
    virtual void setMute(AudioDevice *device, bool state) = 0;
    void mute(AudioDevice *device);
    void unmute(AudioDevice *device);
    virtual void setIgnoreMaxVolume(bool ignore);

signals:
    void sinkListChanged();

protected:
    QList<AudioDevice*> m_sinks;
};

#endif // AUDIOENGINE_H
