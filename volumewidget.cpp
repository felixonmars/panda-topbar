#include "volumewidget.h"
#include <QDebug>

#include <cmath>
#include <QTimer>

static const auto success_cb = [](pa_context *, int, void *) {};

VolumeWidget::VolumeWidget(QWidget *parent)
    : QWidget(parent)
{
    mainloop = pa_threaded_mainloop_new();
    pa_threaded_mainloop_lock(mainloop);
    pa_mainloop_api *api = pa_threaded_mainloop_get_api(mainloop);
    context = pa_context_new(api, "panda");

    if (pa_threaded_mainloop_start(mainloop)) {
        qDebug() << "Unable to start pulseaudio mainloop";
        pa_threaded_mainloop_free(mainloop);
        mainloop = nullptr;
        return;
    }

    pa_context_set_state_callback(context, [](pa_context *context, void *volume_) {
        VolumeWidget *volume = (VolumeWidget *)volume_;
        switch(pa_context_get_state(context)) {
            case PA_CONTEXT_READY:
                volume->state = CONNECTED;
                volume->setupSubscription();
                volume->setupDefaultSink();
                break;
            case PA_CONTEXT_FAILED:
                volume->state = ERROR;
                break;
            case PA_CONTEXT_UNCONNECTED:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_TERMINATED:
                break;
        }
    }, this);

    state = CONNECTING;
    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0 ||
        state == ERROR) {
        qDebug() << "Connection error\n";
        return;
    }
    pa_threaded_mainloop_unlock(mainloop);

	QTimer::singleShot(1000, this, [=] { setVolume(70); });
}

VolumeWidget::~VolumeWidget()
{
    pa_threaded_mainloop_stop(mainloop);
    pa_threaded_mainloop_free(mainloop);
}

void VolumeWidget::setVolume(int percent)
{
    if(percent < 0) percent = 0;
    pa_volume_t new_volume = round((double)percent * (double)PA_VOLUME_NORM / 100.0);
    if (new_volume > PA_VOLUME_MAX) new_volume = PA_VOLUME_MAX;
    pa_cvolume *new_cvolume = pa_cvolume_set(&sinfo.volume, sinfo.volume.channels, new_volume);
    pa_operation *op = pa_context_set_sink_volume_by_name(context, sink.c_str(), new_cvolume, success_cb, nullptr);
    iterate(op);
    pa_operation_unref(op);
    populateSinkInfo();
}

int VolumeWidget::volume() const
{
    if(isMute())
        return -1;

    int volume_avg = pa_cvolume_avg(&sinfo.volume);
    int percent = (int)round((double)volume_avg * 100. / PA_VOLUME_NORM);

    return percent;
}

bool VolumeWidget::isMute() const
{
    return (bool)sinfo.mute;
}

void VolumeWidget::mute()
{
    pa_operation *op = pa_context_set_sink_mute_by_name(context, sink.c_str(), (int)(!isMute()), success_cb, nullptr);
    iterate(op);
    pa_operation_unref(op);
    populateSinkInfo();
}

void VolumeWidget::populateSinkInfo()
{
    qDebug() << "populate...";
    pa_operation *op = pa_context_get_sink_info_by_name(context, sink.c_str(), [](pa_context *, const pa_sink_info *i, int eol, void *volume_) {
        if(eol != 0) return;
        VolumeWidget *volume = (VolumeWidget *)volume_;
        emit volume->sinkInfoChanged(i);
        emit volume->update();
    }, this);
    pa_operation_unref(op);
}

void VolumeWidget::update()
{
    const int percent = volume();

    qDebug() << percent;

    QWidget::update();
}

void VolumeWidget::setupSubscription()
{
    pa_operation *op;
        // subscribe to events
    pa_context_set_subscribe_callback(context, [](pa_context *, pa_subscription_event_type_t t, uint32_t idx, void *volume_) {
        qDebug() << "subscribe";
        VolumeWidget *volume = (VolumeWidget *)volume_;
        if(t == PA_SUBSCRIPTION_EVENT_CHANGE && idx == volume->sinfo.index) {
            volume->populateSinkInfo();
        }
    }, this);
    if(!(op = pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_SINK, success_cb, nullptr)))
        qDebug() << "Can't subscribe";
}

void VolumeWidget::setupDefaultSink()
{
    pa_operation *op = pa_context_get_server_info(context, [](pa_context *, const pa_server_info *i, void *volume_) {
        VolumeWidget *volume = (VolumeWidget *)volume_;
        emit volume->sinkChanged(i->default_sink_name);
    }, this);

    pa_operation_unref(op);
}

void VolumeWidget::sinkInfoChanged(const pa_sink_info *i)
{
    sinfo.index = i->index;
    sinfo.mute = i->mute;
    sinfo.volume = i->volume;
}

void VolumeWidget::sinkChanged(const char *s)
{
    if(!sink.empty())
        return;
    sink = s;

    populateSinkInfo();
}

void VolumeWidget::iterate(pa_operation *op)
{
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(mainloop);
}
