#ifndef WAVEFILE_H
#define WAVEFILE_H

#include "RiffFile.h"

//  using namespace RiffChunk;

//  typedef unsigned int int4b;
//  typedef unsigned int int2b;

struct WaveFileInfo {

    int2b compressionCode;
    int2b channelsCount;
    int4b sampleRate;
    int4b bytesPerSecond;
    int2b blockAllign;
    int2b bitsPerSamle;

    WaveFileInfo(int2b compressionCode = 1,
                 int2b channelsCount = 1,
                 int4b sampleRate = 22050,
                 int4b bytesPerSecond = 44100,
                 int2b blockAllign = 2,         //  количество байт в семпле
                 int2b bitsPerSamle = 16){
        this->compressionCode = compressionCode;
        this->channelsCount = channelsCount;
        this->sampleRate = sampleRate;
        this->bytesPerSecond = bytesPerSecond;
        this->blockAllign = blockAllign;
        this->bitsPerSamle = bitsPerSamle;
    }



    Byte* toBytes(){
        Byte* bytes = new Byte(16);
        memcpy(bytes,   &compressionCode,   2);
        memcpy(bytes+2, &channelsCount,     2);
        memcpy(bytes+4, &sampleRate,        4);
        memcpy(bytes+8, &bytesPerSecond,    4);
        memcpy(bytes+12,&blockAllign,       2);
        memcpy(bytes+14,&bitsPerSamle,      2);
        return bytes;
    }

    myDWORD getBytesCount(){
        return 16;
    }
};


class WaveFile : public RiffFile
{

protected:
    myDWORD WaveDataCursorPosition;
    WaveFileInfo* info;
    ByteBuffer* waveData;

public:
    //  WaveFile(QString fileName);
    WaveFile(QFile* fileName);

    bool write();

    WaveFileInfo* getInfo();
    bool readWaveData(Byte* bytes, myDWORD bytesCount);

    bool setInfo(WaveFileInfo*);

    bool addWaveData(Byte* bytes, myDWORD bytesCount);

    bool open(QIODevice::OpenModeFlag);


protected:
    Chunk* getDataChunk();
    Chunk* getFMTChunk();
    void createChunks();
    WaveFileInfo* readInfo();
    bool writing();
    bool reading();

};

#endif // WAVEFILE_H
