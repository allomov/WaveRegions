#include "RiffFile.h"

RiffFile::RiffFile(QFile* file)
{
//    this->file = new QFile(fileName);
    this->file = file;
    errorsList = new QStringList;
    rootChunk = NULL;
}

RiffFile::~RiffFile(void){
    if (file->isOpen()) close();
    //  delete file;
    delete errorsList;
}

bool RiffFile::open(QIODevice::OpenModeFlag flag){
    if (file->isOpen()) {
        processError("RiffFile::open error: file already opened");
        return false;
    }
    bool ok = file->open(flag);
    if (!ok) processError("RiffFile::open error: can't open file");
    return ok;
}

bool RiffFile::close(){
    if (!file->isOpen()) {processError("RiffFile::open error: file is not opened"); return false;}
    file->close();
    if (rootChunk) delete rootChunk; rootChunk = NULL;
    return true;
}

bool RiffFile::setRootChunk(Chunk* rootChunk){
    if (file->openMode() & QIODevice::ReadOnly){
        processError("RiffFile::setRootChunk error:: file can't be opened for reading");
        return false;
    }
    return this->rootChunk = rootChunk;
}

QStringList RiffFile::getErrors(){
    QStringList list(*errorsList);
    list += rootChunk->getErrors();
    return list;
}

void RiffFile::processError(QString errorMessage){
    qDebug() << errorMessage;
    errorsList->append(errorMessage);
}

Chunk* RiffFile::getRootChunk(){
    if (rootChunk)
        return rootChunk;
    else
        return createRootChunk();
}


Chunk* RiffFile::createRootChunk(){
	qDebug() << file->fileName() << !file->isOpen();
    if (!file->isOpen()){
        processError("RiffFile::createRootChunk error: file is not opened"); return NULL;
    }
    if (!file->openMode() & QIODevice::ReadOnly & QIODevice::WriteOnly){
        processError("RiffFile::createRootChunk error: doesn't support reading and writing in a same time"); return NULL;
    }

    if (rootChunk ) delete rootChunk ;
    rootChunk = new Chunk(file);

    if (file->openMode() & QIODevice::ReadOnly){
        rootChunk->read();
    }
    if (file->openMode() & QIODevice::WriteOnly){

    }
    return rootChunk;
}


Chunk* RiffFile::findChunkByName(QString name, uint pos){
    if (rootChunk) {
        ChunkList* list = rootChunk->getChildren();
        Chunk* ch;
        int n = 0;
        foreach(ch, *list){
            if (ch->getName()==name) {
                if (n==pos) return ch; else n++;
            }
        }
    }
    return NULL;
}

bool RiffFile::readChunks(void){
    getRootChunk();
    return true;
}
