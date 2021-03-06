/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Streamer.h
 * Author: ignas
 *
 * Created on November 20, 2017, 12:29 PM
 */

#ifndef STREAMER_H
#define STREAMER_H

#include "dataTypes.h"
#include "fifo.h"

namespace lime
{
   
class IConnection;
class StreamChannel;
class FPGA;
class LMS7002M;

/*!
 * The stream config structure is used with the SetupStream() API.
 */
struct LIME_API StreamConfig
{
    StreamConfig(void){};

    //! True for transmit stream, false for receive
    bool isTx;

    uint8_t channelID;

    float performanceLatency;

    //! Possible stream data formats
    enum StreamDataFormat
    {
        FMT_INT16,
        FMT_INT12,
        FMT_FLOAT32,
    };

    /*!
     * The buffer length is a size in samples
     * that used for allocating internal buffers.
     * Default: 0, meaning automatic selection
     */
    size_t bufferLength;

    //! The format of the samples in Read/WriteStream().
    StreamDataFormat format;

    /*!
     * The format of samples over the wire.
     * This is not the format presented to the API caller.
     * Choosing a compressed format can decrease link use
     * at the expense of additional processing on the PC
     * Default: STREAM_12_BIT_IN_16
     */
    StreamDataFormat linkFormat;
};
    
class Streamer
{
public:
    Streamer(FPGA* f, LMS7002M* chip, int id);
    ~Streamer();

    StreamChannel* SetupStream(const StreamConfig& config);
    int CloseStream(StreamChannel* streamID);
    int GetStreamSize(bool tx);

    uint64_t GetHardwareTimestamp(void);
    void SetHardwareTimestamp(const uint64_t now);
    int UpdateThreads(bool stopAll = false);

    std::atomic<uint32_t> rxDataRate_Bps;
    std::atomic<uint32_t> txDataRate_Bps;
    IConnection* dataPort;
    std::thread rxThread;
    std::thread txThread;
    std::atomic<bool> terminateRx;
    std::atomic<bool> terminateTx;

    StreamChannel* mRxStreams[2];
    StreamChannel* mTxStreams[2];
    std::atomic<uint64_t> rxLastTimestamp;
    std::atomic<uint64_t> txLastTimestamp;
    uint64_t mTimestampOffset;
    int streamSize;
    unsigned txBatchSize;
    unsigned rxBatchSize;
    StreamConfig::StreamDataFormat dataLinkFormat;
    void ReceivePacketsLoop();
    void TransmitPacketsLoop();
private:
    void AlignRxTSP();
    void AlignRxRF(bool restoreValues);
    void AlignQuadrature(bool restoreValues);
    void RstRxIQGen();
    double GetPhaseOffset(int bin);
    FPGA* fpga;
    LMS7002M* lms;
    int chipId;
};

class LIME_API StreamChannel 
{
public:
    struct Frame
    {
        uint64_t timestamp;
        static const uint16_t samplesCount = samples12InPkt;
        complex16_t samples[samplesCount];
    };
    
    struct Metadata
    {
        uint64_t timestamp;
        uint32_t flags;
    };
    
    struct Info
    {
        int fifoSize;
        int fifoItemsCount;
        int overrun;
        int underrun;
        bool active;
        float linkRate;
        int droppedPackets;
        uint64_t timestamp;
    };
    
    StreamChannel(Streamer* streamer, StreamConfig config);
    ~StreamChannel();
    
    int Read(void* samples, const uint32_t count, Metadata* meta, const int32_t timeout_ms = 100);
    int Write(const void* samples, const uint32_t count, const Metadata* meta, const int32_t timeout_ms = 100);
    StreamChannel::Info GetInfo();
    int GetStreamSize();

    bool IsActive() const;
    int Start();
    int Stop();
    StreamConfig config;
    Streamer* mStreamer;
    unsigned overflow;
    unsigned underflow;
    unsigned pktLost;
    bool mActive;
       
protected:
    RingFIFO* fifo;  
private:
    StreamChannel() = default;
};



}

#endif /* STREAMER_H */

