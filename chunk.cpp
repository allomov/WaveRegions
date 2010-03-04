#include "chunk.h"

using namespace RiffChunk;

QString RiffChunk::FOURCC2QString(FOURCC d){
    char* chars = new char[5];
    *(chars+4) = '\0';
    memcpy(chars, &d, 4);
    return QString::fromAscii(chars);
}

QString RiffChunk::DWORD2QString(myDWORD d){
    char* chars = new char[5];
    *(chars+4) = '\0';
    memcpy(chars, &d, 4);
    return QString::fromAscii(chars);
}

ByteBuffer* RiffChunk::Bytes2ByteBuffer(Byte* b , uint size){
    ByteBuffer* buffer = new ByteBuffer;
    for (int i=0; i<size; i++) {
        buffer->append(*(b+i));
    }
    return buffer;
}


Chunk* Chunk::copy(void){
//    this->errorsList = new QStringList(ch->getErrors());
//    this->children = ch->isList() ? new ChunkList(ch->getChildren()) : NULL;
//    this->ckID = ch->getID();
//    this->ckSize = ch->getDataSize();
//    this->file = file;
//    this->offset = ch->getHeaderOffset();
    return NULL;
}

Chunk::Chunk(QString name, QFile* file){
    this->file = file;
    setName(name);
    init();
}

Chunk::Chunk(QFile* file)
{
    this->file = file;
    init();
}

void Chunk::init(){
    errorsList = new QStringList;
    children = NULL;
    offset = 0;
    ckSize = 0;
    headerSize = 8;
}

Chunk::~Chunk(void){
    //	if (errorsList) delete errorsList;
    //	if (children) delete children;
}

bool Chunk::isList(){
    QString name = getName().toUpper();
    if (name == "RIFF" || name == "LIST") return true;
    return false;
}

QString Chunk::getName(){
    return FOURCC2QString(ckID);
}

void Chunk::setName(QString s){
    char* bytes = s.toLocal8Bit().data();
    memcpy(&ckID, bytes, 4);
    if (isList()) {
        children = new ChunkList;
        if (getName().toUpper()=="RIFF") typeID = 0x57415645; else typeID = 0x6164746C;
    }

}

ChunkList* Chunk::getChildren(){
    return children;
}


bool Chunk::write(){
    ckSize = data->size() + (isList() ? 4 : 0);
    file->write((const char*) &ckID, 4);
    file->write((const char*) &ckSize, 4);
    file->write(data->data(), data->size());
    if (isList()){
        Chunk* ch;
        foreach(ch, *children){
            ch->write();
        }
    }
    return true;
}

void Chunk::addChild(Chunk* ){

}


//  считывает заголовок чанка, и, если чанк является списком, считывает потомков
bool Chunk::read(){
    bool ok = readHeader();

    qDebug() << "Readed header:"
            <<"name:" << getName()
            << "getDataSize:" << getDataSize()
            << "headerOffset:" << getHeaderOffset() 
            << "dataOffset:" << getDataOffset() ;
//            << "\tdataOffset:" << getDataOffset() << "\n";
    if (ok && isList()){
        ok = readEmbeddedList();
    }
    return ok;
}

/*
    если размер не указан, то на месте байт создается новый массив, который будет содержать все данные из чанка
    считывает поле data в чанке, то есть то что идет после заголовка.
    возвращает количество прочитанных байт.
*/
int Chunk::readData(Byte* data , myDWORD size){
    if (size==-1 && isOpenedForReading()) {
        size = getDataSize();
        data = new char[size];
    }

    if (!(file->openMode() & QIODevice::ReadOnly))
        ERROR_MACRO("Chunk::readData error: file should be opened for reading", -1);
    if (!checkFilePosition(getDataOffset()))
        ERROR_MACRO("Chunk::readData error: there is no good in this message", -1);
    return file->read(data, size);
}

bool Chunk::readHeader() {
    Byte* bytes = new Byte[8];
    if (!checkFilePosition(getHeaderOffset()))
        ERROR_MACRO("Chunk::readHeader error: there is no good in this message", false);
    int bytesReaded = file->read(bytes, 8);
    if (bytesReaded != 8)
        ERROR_MACRO("Chunk::readHeader error: readed not right bytes count", false);
    memcpy(&ckID,   bytes,    4);
    memcpy(&ckSize, bytes+4 , 4);
//    if (isList()) {
//        bytesReaded = file->read(bytes, 4);
//        if (bytesReaded != 4) ERROR_MACRO("Chunk::readHeader error: readed not right bytes count", false);
//        memcpy(&typeID,   bytes,    4);
//        headerSize+=4;
//    }
    //  qDebug() << getName();
    return true;
}

bool Chunk::readEmbeddedList(){
    //  read typeID
    Byte* bytes = new Byte[4];
    int bytesReaded = file->read(bytes, 4);
    if (bytesReaded != 4)
        ERROR_MACRO("Chunk::readHeader error: readed not right bytes count", false);
    memcpy(&typeID,   bytes,    4);
    this->children = new ChunkList;
    int size = 0;
    myDWORD cursorPosition = this->getDataOffset() + 4;
    myDWORD endPosition = this->getDataOffset() + this->getDataSize() - 4;
    while (cursorPosition < endPosition) {
        Chunk* chunk = new Chunk(file);
        chunk->setOffset(cursorPosition);
        this->children->append(chunk);
        chunk->read();
        cursorPosition += chunk->getHeaderSize()+chunk->getDataSize();
    }
    return true;
}

myDWORD Chunk::getHeaderSize(){
    return headerSize;
}

myDWORD Chunk::getHeaderOffset(){
    return offset;
}


myDWORD Chunk::getDataOffset(){
    return offset + 8;
}

/*
    возвращает true если указатель счиывания установлен на нужной позиции
    false если возникла ошибка
*/

bool Chunk::checkFilePosition(myDWORD pos){
      //    qDebug() << "pos" << pos << "file->pos()" << file->pos();
    if (file->pos() != pos){
        if (!file->seek(pos)) ERROR_MACRO(QString("Chunk::checkFilePosition error: error while trying to enter ")+
                                         QString::number(pos)+QString(" position"), false);
    }
    return true;
}

QStringList Chunk::getErrors(){
    QStringList list(*errorsList);
    if (isList()){
        Chunk* ch;
        foreach(ch, *children){
            list += ch->getErrors();
        }
    }
    return list;
}


myDWORD Chunk::getDataSize() {
    if (isOpenedForReading())
        return ckSize%2 ? ckSize+1 : ckSize;
    else
        return data->size();
}

FOURCC Chunk::getID() {return ckID;}

QString Chunk::toString(){
    QString res = getName() + "\n";
    if (isList()) {
        Chunk* ch;
        foreach(ch, *children){
            res += "p\t" + ch->toString();
        }
    }
    return res;
}

myDWORD Chunk::getOffset(void){ return offset; }
void Chunk::setOffset(myDWORD d){offset = d;}

bool Chunk::isOpenedForReading(){
    return file->openMode() & QIODevice::ReadOnly;
}


bool Chunk::addToData(Byte* bytes, myDWORD size){
    for (uint i=0; i<size; i++)
        data->append(*(bytes+i));
    return true;
}

int Chunk::getSize(){
    return ckSize;
}

ByteBuffer* Chunk::readWholeData(){
    int dataSize = getDataSize();
    Byte* bytes = new Byte[dataSize];
    readData(bytes, dataSize);
    ByteBuffer* bb = Bytes2ByteBuffer(bytes, dataSize);
    delete bytes;
    return bb;
}


Chunk* Chunk::findChunkByName(QString name, uint pos){
    //  ChunkList* list = rootChunk->getChildren();
    Chunk* ch;
    int n = 0;
    foreach(ch, *children){
        if (ch->getName().toUpper()==name.toUpper()) {
            if (n==pos) return ch; else n++;
        }
    }
    return NULL;
}

myDWORD Chunk::readDWORD(myDWORD pos){
    if (pos>getDataSize())
        ERROR_MACRO("pos couldn`t be more than size", -1);
    myDWORD positionInFile = getDataOffset() + pos;
    checkFilePosition(positionInFile);
    myDWORD res;
    file->read((char*) &res, 4);
    return res;
}

QString Chunk::getType(){
    return DWORD2QString(typeID);
}
