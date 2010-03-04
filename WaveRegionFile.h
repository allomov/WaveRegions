#ifndef WAVEREGIONFILE_H

#define WAVEREGIONFILE_H
#include "WaveFile.h"

namespace RiffFileNamespace {

    struct Region {
        QString name;
        myDWORD begin;
        myDWORD end;
        myDWORD cueID;
        Region(myDWORD begin, myDWORD end, myDWORD cueID){this->begin=begin; this->end=end;this->cueID=cueID;}
        Region(myDWORD begin, myDWORD end, QString name=""){this->begin=begin; this->end=end; this->name=name; }
        Byte* toLtxtChunkBytes(myDWORD cueID){
            Byte* bytes = new Byte(getLtxtByteCount());
            myDWORD dur = end - begin;
            int2b zero = 0;
            memcpy(bytes   , &cueID , 4);
            memcpy(bytes+4 , &dur   , 4);
            memcpy(bytes+8 , "rgn " , 4);
            memcpy(bytes+12, &zero  , 2);
            memcpy(bytes+14, &zero  , 2);
            memcpy(bytes+16, &zero  , 2);
            memcpy(bytes+18, &zero  , 2);
            return bytes;
        }


        Byte* toLablChunkBytes(myDWORD cueID){
            Byte* bytes = new Byte(getLablByteCount());
            QByteArray text = name.toLocal8Bit();
            memcpy(bytes   , &cueID , 4);
            memcpy(bytes+4 , text.data(), text.size());
            return bytes;
        }

        myDWORD getLtxtByteCount(){
            return 20;
        }

        myDWORD getLablByteCount(){
            return 20 + name.length();
        }

    };

    struct CuePoint {
        CuePoint() {}
        CuePoint(myDWORD id, myDWORD sampleOffset) {cueID=id; this->sampleOffset=sampleOffset;}
        myDWORD cueID;
        myDWORD sampleOffset;
        Byte* toBytes(myDWORD cueID){      //LablChunkBytes
            Byte* bytes = new Byte(24);
            myDWORD zero = 0;
            memcpy(bytes,    &cueID, 4);
            memcpy(bytes+4,  &zero, 4);
            memcpy(bytes+8,  "data", 4);
            memcpy(bytes+12, &zero, 4);
            memcpy(bytes+16, &zero, 4);
            memcpy(bytes+20, &sampleOffset, 4);
            return bytes;
        }

        myDWORD getBytesCount(){
//            myDWORD d = name.length()
//            return 20 + name.length() + (name.length()%2)?1:0;
            return 24;
        }

        static CuePoint* fromBytes(Byte* bytes){
            CuePoint* res = new CuePoint;
            memcpy(&res -> cueID,    bytes, 4);
            memcpy(&res -> sampleOffset, bytes+20, 4);
            return res;
        }

    };

    struct Mark {
        myDWORD position;
        myDWORD cueID;
        QString text;
        Mark(myDWORD pos){
            this->position = pos;
        }

        Byte* toLablChunkBytes(myDWORD cueID){
            Byte* bytes = new Byte(getLablByteCount());
            char ch = '\0';
            memcpy(bytes, &cueID, 4);
            memcpy(bytes+4, &ch, 1);
            return bytes;
        }

        myDWORD getLablByteCount(){
            return 6;
        }

    };

    typedef QVector<Region*> RegionList;
    typedef QVector<Mark*>   MarkerList;
    typedef QVector<CuePoint*>  CuePointList;

    class WaveRegionFile : public WaveFile {
    private:
        RegionList* regionList;
        MarkerList* markerList;
        CuePointList* cuePointList;
        void parseCueChunk();
        CuePoint* findCueById(myDWORD cueID);

    public:
        WaveRegionFile(QFile*);
        ~WaveRegionFile();
        void addRegion(myDWORD begin, myDWORD end, QString name);
        void addMark(myDWORD position);
        void write();
        bool read();

        MarkerList* getMarkers();
        RegionList* getRegions();
        bool isRegionAndMarksList(Chunk*);
        //  void createChunks();
    };
}

#endif
