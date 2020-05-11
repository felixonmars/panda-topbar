#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QWidget>

#include <pulse/pulseaudio.h>
#include <pulse/volume.h>
#include <pulse/context.h>

class VolumeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeWidget(QWidget *parent = nullptr);
    ~VolumeWidget();

    void setVolume(int percent);
    int volume() const;

    bool isMute() const;
    void mute();

    void populateSinkInfo();

    void update();

private:
    void setupSubscription();
    void setupDefaultSink();

    void sinkInfoChanged(const pa_sink_info *);
    void sinkChanged(const char *s);

private:
    enum {
        CONNECTING, CONNECTED, ERROR
    } state;

    pa_threaded_mainloop *mainloop;
    pa_context *context;
    int retval;
    pa_sink_info sinfo;
    void iterate(pa_operation *op);
    std::string sink;
    pa_cvolume *cvolume;
};

#endif // VOLUMEWIDGET_H
