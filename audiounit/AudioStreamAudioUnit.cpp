#include "AudioStreamAudioUnit.h"

namespace oboe {

AudioStreamAudioUnit::AudioStreamAudioUnit() {

}

AudioStreamAudioUnit::AudioStreamAudioUnit(const AudioStreamBuilder &builder) {

}

bool AudioStreamAudioUnit::isSupported() {
    //TODO: XIAOCHUN
    //判断是否支持AudioUnit
}

Result AudioStreamAudioUnit::open() {
    //TODO: XIAOCHUN
    //创建一个流
}

Result AudioStreamAudioUnit::close() {
    //关闭流
}

Result AudioStreamAudioUnit::requestStart() {
    //流行为开始，录音，播放
}

Result AudioStreamAudioUnit::requestPause() {
    //暂停
}

Result AudioStreamAudioUnit::requestFlush() {
    //清空缓冲区
}

Result AudioStreamAudioUnit::requestStop() {

}

ResultWithValue<int32_t> AudioStreamAudioUnit::write(const void *buffer, int32_t numFrames, int64_t timeoutNanoseconds) {

}


ResultWithValue<int32_t> AudioStreamAudioUnit::read(void *buffer, int32_t numFrames, int64_t timeoutNanoseconds){

}

ResultWithValue<int32_t> AudioStreamAudioUnit::setBufferSizeInFrames(int32_t requestedFrames) {

}
int32_t AudioStreamAudioUnit::getBufferSizeInFrames() {

}

int32_t AudioStreamAudioUnit::getFramesPerBurst() {
}

ResultWithValue<int32_t> AudioStreamAudioUnit::getXRunCount() const {
};

ResultWithValue<double> AudioStreamAudioUnit::calculateLatencyMillis() {

}

Result AudioStreamAudioUnit::waitForStateChange(StreamState currentState, StreamState *nextState, int64_t timeoutNanoseconds) {

}

Result AudioStreamAudioUnit::getTimestamp(clockid_t clockId, int64_t *framePosition, int64_t *timeNanoseconds){

}

StreamState AudioStreamAudioUnit::getState() const {

}


DataCallbackResult onDefaultCallback(void *audioData, int numFrames) {}



}
