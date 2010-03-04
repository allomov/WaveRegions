#include "wavefile.h"

WaveFile::WaveFile(QFile* file): RiffFile(file)
{
    info = NULL;
    waveData = NULL;
    WaveDataCursorPosition = 0;
}

//  don't work yet
bool WaveFile::write(){
    Chunk* dataChunk = getDataChunk();
    if (dataChunk){
    } else {
        ERROR_MACRO("there is no data chunk", false);
    }
    return true;
}

WaveFileInfo* WaveFile::getInfo(){
    if (info) return info;
    if (file->openMode() & QIODevice::ReadOnly) {
        return readInfo();
    }
    return NULL;
}

bool WaveFile::readWaveData(Byte* bytes, myDWORD bytesCount){
    Chunk* dataChunk = getDataChunk();
    if (dataChunk ){
        return dataChunk->readData(bytes, bytesCount);
    }
    return false;
}

bool WaveFile::setInfo(WaveFileInfo* w) {
    info = w;
    if (file->openMode() & QIODevice::ReadOnly)
        ERROR_MACRO("WaveFile::setInfo error: file is read only", false);
    return true;
}

bool WaveFile::addWaveData(Byte* bytes, myDWORD bytesCount){
//    qDebug("GLFunctionGraph::getValues()");

//    QString str = QString::fromLocal8Bit("sasha molodec");
//    qDebug() << str;

    qDebug() << "Items in list";

    for (myDWORD i=0; i<bytesCount; i++){
        Byte b = *(bytes+i);
        waveData->append(b);
    }
    //  waveData->push_back(ByteBuffer(bytes, bytesCount));
    return true;
}

Chunk* WaveFile::getDataChunk(){
    return findChunkByName("data");
}

Chunk* WaveFile::getFMTChunk(){
    return findChunkByName("fmt ");
}

WaveFileInfo* WaveFile::readInfo(){
    Chunk* fmtChunk = getFMTChunk();
    if (fmtChunk) {
        Byte* bytes = new Byte[16];
        bool ok = fmtChunk->readData(bytes, 16);
        if (ok) {
            if (!info) info = new WaveFileInfo;
            memcpy(&(info->compressionCode) , bytes,      2);
            memcpy(&(info->channelsCount)   , bytes + 2,  2);
            memcpy(&(info->sampleRate)      , bytes + 4,  4);
            memcpy(&(info->bytesPerSecond)  , bytes + 8,  4);
            memcpy(&(info->blockAllign)     , bytes + 12, 2);
            memcpy(&(info->bitsPerSamle)    , bytes + 14, 2);
            return info;
        }
    }
    return NULL;
}


void WaveFile::createChunks(){
    if (!writing()){
        rootChunk = new Chunk("RIFF", file);
        ChunkList* children = rootChunk -> getChildren();
        children -> push_back(new Chunk("fmt ", file));
        children -> push_back(new Chunk("data", file));
    } else
        qDebug()<<"oyjojojoyjoy";
//        ERROR_MACRO("WaveFile::createChunks error: need write mode ", );
}

bool WaveFile::writing() {return file->openMode() & QIODevice::WriteOnly; }
bool WaveFile::reading() {return file->openMode() & QIODevice::ReadOnly; }

bool WaveFile::open(QIODevice::OpenModeFlag fl){
	//	qDebug() << file->openMode() << (file->openMode() == QIODevice::NotOpen);
    if (file->openMode() == QIODevice::NotOpen){
        bool ok = RiffFile::open(fl);
        if (fl & QIODevice::WriteOnly)
            this->waveData = new ByteBuffer;
		qDebug() <<  file->fileName() << file->isOpen();
        return ok;
    } else {

    }
}
