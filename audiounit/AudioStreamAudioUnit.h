#ifndef OBOE_STREAM_AUDIOUNIT_H_
#define OBOE_STREAM_AUDIOUNIT_H_


#include "oboe/AudioStreamBuilder.h"
#include "oboe/AudioStream.h"
#include "oboe/Definitions.h"

namespace oboe {

class AudioStreamAudioUnit : public AudioStream {
public:
    AudioStreamAudioUnit();
    explicit AudioStreamAudioUnit(const AudioStreamBuilder &builder);
    virtual ~AudioStreamAudioUnit() = default;

public:
    /* return true if AudioUnit is support */
    static bool isSupported();

public:
    /* AudioStream virtual method */
    Result open() override;
    Result close() override;

    Result requestStart() override;
    Result requestPause() override;
    Result requestFlush() override;
    Result requestStop() override;

    ResultWithValue<int32_t> write(const void *buffer, int32_t numFrames, int64_t timeoutNanoseconds) override;
    ResultWithValue<int32_t> read(void *buffer, int32_t numFrames, int64_t timeoutNanoseconds) override;
    
    ResultWithValue<int32_t> setBufferSizeInFrames(int32_t requestedFrames) override;
    int32_t getBufferSizeInFrames() override;
    int32_t getFramesPerBurst() override;
    ResultWithValue<int32_t> getXRunCount() const override;
    bool isXRunCountSupported() const override { return true; }

    ResultWithValue<double> calculateLatencyMillis() override;

    Result waitForStateChange(StreamState currentState, StreamState *nextState, int64_t timeoutNanoseconds) override;

    Result getTimestamp(clockid_t clockId, int64_t *framePosition, int64_t *timeNanoseconds) override;

    StreamState getState() const override;

    AudioApi getAudioApi() const override {
        return AudioApi::AudioUnit;
    }

    DataCallbackResult callOnAudioReady(udioStream *stream, void *audioData, int32_t numFrames);

};

}

#endif 
