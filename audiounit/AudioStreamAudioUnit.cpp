#include "AudioStreamAudioUnit.h"

namespace oboe {

AudioStreamAudioUnit::AudioStreamAudioUnit() {

}

AudioStreamAudioUnit::AudioStreamAudioUnit(const AudioStreamBuilder &builder) {
    streamBuider = builder;
}

bool AudioStreamAudioUnit::isSupported() {
    /* TODO: judge wether AudioUnit is support */
    return true;
}

Result AudioStreamAudioUnit::open() {
    /* create stream */
    AudioComponentDescription acd;
    acd.componentType = kAudioUnitType_Output;
    acd.componentSubType = kAudioUnitSubType_RemoteIO;
    acd.componentManufacturer = kAudioUnitManufacturer_Apple;
    acd.componentFlags = 0;
    acd.componentFlagsMask = 0;
    
    /* 通过组件描述找到对应的音频组件 */
    component = AudioComponentFindNext(NULL, &acd);
    
    OSStatus status = noErr;
    
    /* initial */
    status = AudioComponentInstanceNew(component, &componetInstance);
    
    if (noErr != status) {
        //   [self handleAudioComponentCreationFailure];
    }
    
    UInt32 flagOne = 1;
    //设置的Element1输入开启及标识
    AudioUnitSetProperty(componetInstance, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &flagOne, sizeof(flagOne));
    //设置输出数据的格式
    AudioStreamBasicDescription desc = {0};
    desc.mSampleRate = streamBuider.mSampleRate;//采样率
    desc.mFormatID = kAudioFormatLinearPCM;//数据格式PCM
    desc.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;//标签格式
    desc.mChannelsPerFrame = streamBuider.mChannelCount;//每帧的频道
    desc.mFramesPerPacket = 1;//每个Packet包含多少帧
    desc.mBitsPerChannel = 16;//位/通道(数据帧中每个通道的采样数据位数。)
    desc.mBytesPerFrame = desc.mBitsPerChannel / 8 * desc.mChannelsPerFrame;//每帧字节
    desc.mBytesPerPacket = desc.mBytesPerFrame * desc.mFramesPerPacket;//每个Packet包含多少字节
    
    AURenderCallbackStruct cb;
    cb.inputProcRefCon = this;
    //TODO: zhouqing
    cb.inputProc = handleInputBuffer;
    //设置Element1输出参数
    AudioUnitSetProperty(componetInstance, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &desc, sizeof(desc));
    //设置Element1数据输出回调及参数
    AudioUnitSetProperty(componetInstance, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, 1, &cb, sizeof(cb));
    //初始化
    status = AudioUnitInitialize(componetInstance);
    
    if (noErr != status) {
        // [self handleAudioComponentCreationFailure];
    }
    
    //使用AudioQueue播放
    AudioStreamBasicDescription queueDesc = {0};
    queueDesc.mSampleRate = streamBuider.mSampleRate;
    queueDesc.mFormatID = kAudioFormatLinearPCM;
    queueDesc.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagsNativeEndian ;
    queueDesc.mChannelsPerFrame = streamBuider.mChannelCount;
    queueDesc.mFramesPerPacket = 1;
    queueDesc.mBitsPerChannel = 16;
    queueDesc.mBytesPerFrame = queueDesc.mBitsPerChannel / 8 * queueDesc.mChannelsPerFrame;
    queueDesc.mBytesPerPacket = queueDesc.mBytesPerFrame * queueDesc.mFramesPerPacket;
    //Creates a new playback audio queue object.
    //TODO: zhouqing
    OSStatus createQueueResult =  AudioQueueNewOutput(&queueDesc, BufferCallback, this, nil, nil, 0, &audioQueue);
    if (createQueueResult != noErr) {
        printf("set AudioQueue fail");
    }
    
    //初始化音频缓冲区3个
    for (int i = 0; i < 3; i++) {
        //创建buffer
        OSStatus  result = AudioQueueAllocateBuffer(audioQueue, 4096, &audioQueueBuffer[i]);
        if (result != noErr) {
            printf("creat AudioQueue fail");
        }
        //初始化
        memset(audioQueueBuffer[i]->mAudioData, 0, 4096);
    }
    //设置AudioQueue
    AudioQueueSetParameter(audioQueue, kAudioQueueParam_Volume, 1.0);
}

static OSStatus handleInputBuffer(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
    AudioStreamAudioUnit *source = inRefCon;
    AudioBuffer buffer;
    buffer.mData = NULL;
    buffer.mDataByteSize = 0;
    buffer.mNumberChannels = 1;
    //将清空的缓存区加入第一个缓冲列表中，单声道只需要一个缓冲区
    AudioBufferList buffers;
    buffers.mNumberBuffers = 1;
    buffers.mBuffers[0] = buffer;
    //播放录制的人声,将数据存入缓冲列表中
    OSStatus status = AudioUnitRender(source.componetInstance,
                                      ioActionFlags,
                                      inTimeStamp,
                                      inBusNumber,
                                      inNumberFrames,
                                      &buffers);
    return status;
    
}

static void BufferCallback(void *inUserData, AudioQueueRef inAQ,
                           AudioQueueBufferRef buffer){
    
}

Result AudioStreamAudioUnit::close() {
    //关闭流
    dispatch_sync(playerTaskQueue, ^{
        if (componetInstance) {
            AudioOutputUnitStop(componetInstance);
            AudioComponentInstanceDispose(componetInstance);
            componetInstance = nil;
            component = nil;
        }
    });
}

Result AudioStreamAudioUnit::requestStart() {
    //流行为开始，录音，播放
    dispatch_sync(playerTaskQueue, ^{
        AudioOutputUnitStart(componetInstance);
        AudioQueueStart(audioQueue, NULL);
    });
}

Result AudioStreamAudioUnit::requestPause() {
    //暂停
}

Result AudioStreamAudioUnit::requestFlush() {
    //清空缓冲区
    for (int i = 0; i < 3; i++) {
        //创建buffer
        OSStatus  result = AudioQueueAllocateBuffer(audioQueue, 4096, &audioQueueBuffer[i]);
        if (result != noErr) {
            printf("creat AudioQueue fail");
        }
        //初始化
        memset(audioQueueBuffer[i]->mAudioData, 0, 4096);
    }
}

Result AudioStreamAudioUnit::requestStop() {
    dispatch_sync(playerTaskQueue, ^{
        AudioOutputUnitStop(componetInstance);
        AudioQueueStop(audioQueue, ture);
    });

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


 DataCallbackResult callOnAudioReady(audioStream *stream, void *audioData, int32_t numFrames);
    

}
