/*
 * File:   lms7_device.h
 * Author: ignas
 *
 * Created on March 9, 2016, 12:54 PM
 */

#ifndef LMS7_DEVICE_H
#define	LMS7_DEVICE_H
#include "LMS7002M.h"
#include "lime/LimeSuite.h"
#include <mutex>
#include <vector>
#include <map>
#include <string>
#include "Streamer.h"
#include "IConnection.h"

class LIME_API LMS7_Device
{
    class lms_channel_info
    {
    public:
        lms_channel_info():lpf_bw(5e6),cF_offset_nco(0),sample_rate(30e6),freq(0){}
        double lpf_bw;
        double cF_offset_nco;
        double sample_rate;
        double freq;
    };
            
public:
    LMS7_Device(LMS7_Device *obj = nullptr);
    virtual ~LMS7_Device();
    int SetConnection(lime::IConnection* conn);
    lime::IConnection* GetConnection(unsigned chan =0);
    lime::FPGA* GetFPGA();
    virtual int Init();
    int EnableChannel(bool dir_tx, size_t chan, bool enabled);
    int Reset();
    virtual unsigned GetNumChannels(const bool tx=false) const;
    virtual unsigned GetLMSCnt() const;
    virtual int SetRate(double f_MHz, int oversample);
    virtual int SetRate(bool tx, double f_MHz, unsigned oversample = 0);
    virtual int SetRate(unsigned ch, double rxRate, double txRate, unsigned oversample = 0);
    virtual double GetRate(bool tx, unsigned chan, double *rf_rate_Hz = NULL);
    lms_range_t GetRateRange(bool dir = false, const unsigned chan = 0)const;
    virtual std::vector<std::string> GetPathNames(bool dir_tx, unsigned chan = 0) const;
    virtual int SetPath(bool tx,unsigned chan, unsigned path);
    int GetPath(bool tx, unsigned chan);
    virtual int SetRxFrequency(size_t chan, float_type f_Hz);
    virtual int SetTxFrequency(size_t chan, float_type f_Hz);
    float_type GetTRXFrequency(bool tx, size_t chan);
    lms_range_t GetFrequencyRange(bool tx) const;
    virtual lms_range_t GetRxPathBand(size_t path, size_t chan) const;
    lms_range_t GetTxPathBand(size_t path, size_t chan) const;
    int SetLPF(bool tx, size_t chan, bool f, bool en, float_type bandwidth=-1);
    float_type GetLPFBW(bool tx,size_t chan, bool filt);
    lms_range_t GetLPFRange(bool tx,size_t chan,bool f);
    int SetGFIRCoef(bool tx, size_t chan, lms_gfir_t filt, const float_type* coef,size_t count);
    int GetGFIRCoef(bool tx, size_t chan, lms_gfir_t filt, float_type* coef);
    int SetGFIR(bool tx, size_t chan, lms_gfir_t filt, bool enabled);
    int SetGain(bool dir_tx, int chan, double value, const std::string &name = "");
    double GetGain(bool dir_tx, int chan, const std::string &name = "") const;
    lms_range_t GetGainRange(bool dir_tx, int chan, const std::string &name = "") const;
    int SetTestSignal(bool dir_tx, size_t chan, lms_testsig_t sig, int16_t dc_i = 0, int16_t dc_q = 0);
    int GetTestSignal(bool dir_tx, size_t chan);
    int SetNCOFreq(bool tx,size_t ch, const float_type *freq, float_type pho);
    int SetNCO(bool tx,size_t ch,int ind,bool down);
    int GetNCOFreq(bool tx,size_t ch, float_type * freq,float_type *pho);
    int SetNCOPhase(bool tx,size_t ch, const float_type *phase, float_type fcw);
    int GetNCOPhase(bool tx,size_t ch, float_type * phase,float_type *fcw);
    int GetNCO(bool tx,size_t ch);
    int Calibrate(bool dir_tx, size_t chan, double bw, unsigned flags);
    int Program(const char* data, size_t len, lms_prog_trg_t target, lms_prog_md_t mode, lime::IConnection::ProgrammingCallback callback);
    int ProgramUpdate(const bool download, lime::IConnection::ProgrammingCallback callback);
    int DACWrite(uint16_t val);
    int DACRead();
    float_type GetClockFreq(size_t clk_id, int channel = -1);
    int SetClockFreq(size_t clk_id, float_type freq, int channel = -1);
    lms_dev_info_t* GetInfo();
    int Synchronize(bool toChip);
    int SetLogCallback(void(*func)(const char* cstr, const unsigned int type));
    int EnableCalibCache(bool enable);
    double GetChipTemperature(size_t ind = 0);
    int LoadConfig(const char *filename, int ind = -1);
    int SaveConfig(const char *filename, int ind = -1);
    int ReadLMSReg(uint16_t address, uint16_t *val, int ind = -1);
    int WriteLMSReg(uint16_t address, uint16_t val, int ind = -1);
    uint16_t ReadParam(struct LMS7Parameter param, int channel = -1, bool forceReadFromChip = false);
    int WriteParam(struct LMS7Parameter param, uint16_t val, int channel = -1);
    int SetActiveChip(unsigned ind);
    lime::LMS7002M* GetLMS(int index = -1);
    int UploadWFM(const void **samples, uint8_t chCount, int sample_count, lime::StreamConfig::StreamDataFormat fmt);
    static LMS7_Device* CreateDevice(lime::IConnection* conn, LMS7_Device *obj = nullptr);
    std::map<std::string, double> extra_parameters;
    
    lime::StreamChannel* SetupStream(const lime::StreamConfig &config);
    int DestroyStream(lime::StreamChannel* streamID);
    uint64_t GetHardwareTimestamp(void);
    void SetHardwareTimestamp(const uint64_t now);
    
    int MCU_AGCStart(uint8_t rssiMin, uint8_t pgaCeil);
    int MCU_AGCStop();

protected:
    const double maxTxGain = 64.0;
    lms_dev_info_t devInfo;
    std::vector<lms_channel_info> tx_channels;
    std::vector<lms_channel_info> rx_channels;
    static const double LMS_CGEN_MAX;
    lime::IConnection* connection;
    std::vector<lime::LMS7002M*> lms_list;
    int ConfigureRXLPF(bool enabled,int ch,float_type bandwidth);
    int ConfigureTXLPF(bool enabled,int ch,float_type bandwidth);
    int ConfigureGFIR(bool enabled,bool tx, float_type bandwidth,size_t ch);
    void _Initialize(lime::IConnection* conn);
    unsigned lms_chip_id;
    std::vector<lime::Streamer*> mStreamers;
    lime::FPGA* fpga;
};

#endif	/* LMS7_DEVICE_H */

