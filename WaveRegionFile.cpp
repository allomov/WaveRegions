#include "WaveRegionFile.h"

using namespace RiffFileNamespace;


WaveRegionFile::WaveRegionFile(QFile* f) : WaveFile(f){
    regionList = new RegionList;
    markerList = new MarkerList;
}

WaveRegionFile::~WaveRegionFile(){}

void WaveRegionFile::addRegion(myDWORD begin, myDWORD end, QString name){
    regionList->append(new Region(begin, end, name));
}

void WaveRegionFile::addMark(myDWORD position){
    markerList->push_back(new Mark(position));
}
//  void WaveRegionFile::createChunks();
void WaveRegionFile::write(){
    Chunk* riff = new Chunk("RIFF", file);
    ChunkList* riffChildren = riff->getChildren();
    Chunk* fmt = new Chunk("fmt ", file);
    fmt->addToData(info->toBytes(), info->getBytesCount());
    riffChildren->append(fmt);
    Chunk* data = new Chunk("data", file);
    data->addToData(waveData->data(), waveData->size());
    riffChildren->append(data);
    Chunk* cue = new Chunk("cue ", file);
    riffChildren->append(cue);

    Chunk* list = new Chunk("LIST", file);
    ChunkList* listChildren = list->getChildren();

    Region* r;


    int freeID=1;
    ChunkList lables;
    foreach(r, *regionList){
        myDWORD cueID = freeID++;
        CuePoint cuePoint1(cueID, r->begin);
        cue->addToData(cuePoint1.toBytes(cueID), cuePoint1.getBytesCount());

        Chunk* ch = new Chunk("ltxt", file);
        Byte* bytes = r->toLtxtChunkBytes(cueID);
        ch->addToData(bytes, r->getLtxtByteCount());
        listChildren->append(ch);

        ch = new Chunk("labl", file);
        Byte* bytes2 = r->toLablChunkBytes(cueID);
        ch->addToData(bytes2, r->getLablByteCount());
        lables.append(ch);
    }
    *(listChildren) += lables;


    Mark* m;
    foreach(m, *markerList){
        myDWORD cueID = freeID++;
        CuePoint cuePoint1(cueID, m->position);                     //  создает объект cue элкмента
        cue->addToData(cuePoint1.toBytes(cueID), cuePoint1.getBytesCount());
        Chunk* ch = new Chunk("labl", file);
        Byte* bytes = m->toLablChunkBytes(cueID);
        ch->addToData(bytes, m->getLablByteCount());
        listChildren->append(ch);
    }

//    вставить cue Number

    riff->write();
}

RegionList* WaveRegionFile::getRegions(){
    return regionList;
}

MarkerList* WaveRegionFile::getMarkers(){
    return markerList;
}

bool WaveRegionFile::read(){
    readChunks();
    parseCueChunk();

    int i=0;
    Chunk* list;
    do {
        list = rootChunk->findChunkByName("LIST", i++);
    } while (list!=NULL && !isRegionAndMarksList(list));
    if (list==NULL) {
        ERROR_MACRO("there is no list of regions", false);
    }

    ChunkList* labels = list->getChildren();
    Chunk* ch1;

    foreach(ch1, *labels){
        if (ch1->getName()=="ltxt"){
            myDWORD cueID = ch1->readDWORD();
            CuePoint* cp = findCueById(cueID);
            myDWORD sampleLength = ch1->readDWORD(4);
            myDWORD begin = cp->sampleOffset;
            Region* r = new Region(begin, begin + sampleLength, cueID);
            regionList->append(r);
        }
        if (ch1->getName()=="labl"){
            myDWORD cueID = ch1->readDWORD();

			Byte* bytes;
			int textSize = ch1->getDataSize();
			ch1->readData(bytes, textSize);
			char* textChars = new char[textSize-4];
			memcpy(textChars, bytes+4, textSize-4);
			QString text = QString::fromAscii(textChars);

			bool isConnectedWithRegion = false;
            Region* r;
            foreach(r, *regionList){
                if (cueID==r->cueID) {
                    r->name = text;
                    isConnectedWithRegion = true;
                }
            }
            if (!isConnectedWithRegion){
                CuePoint* cp = findCueById(cueID);
                Mark* m = new Mark(cp->sampleOffset);
                markerList->append(m);
            }
        }
    }
}

void WaveRegionFile::parseCueChunk(){
    cuePointList = new CuePointList;
    Chunk* chunk = findChunkByName("cue ");
    int size = chunk->getDataSize();
	Byte* bytes = new Byte[size];
    chunk->readData(bytes, size);
    myDWORD cuePointsCount;
    memcpy((char*)&cuePointsCount, bytes, 4);

    for (int i=0; i<cuePointsCount; i++){
        CuePoint* cp = CuePoint::fromBytes(bytes + i*24 + 4);
        cuePointList->append(cp);
    }
}

bool WaveRegionFile::isRegionAndMarksList(Chunk* ch){
    if (ch->getName().toUpper()=="LIST" && ch->getType() == "adtl"){
        ChunkList* chl = ch->getChildren();
        Chunk* ch;
        foreach(ch,*chl){
			qDebug() << ch->getName();
			if (!(ch->getName()=="ltxt" || ch->getName()=="labl")){
                return false;
            }
        }
        return true;
    }
    return false;
}

CuePoint* WaveRegionFile::findCueById(myDWORD cueID){
    CuePoint* cp;
    foreach(cp, *cuePointList){
        if (cp->cueID==cueID) return cp;
    }
    return NULL;
}

