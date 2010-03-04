#ifndef CHUNK_H
#define CHUNK_H

#include <QtCore>


#define ERROR_MACRO(message,retVal)  {  \
        errorsList->append(message);    \
        qDebug() << message;            \
        return retVal;                  \
    }



namespace RiffChunk {

    class Chunk;
    typedef QVector<Chunk*> ChunkList;
    typedef int FOURCC;                 //  32-bit number
    typedef unsigned long myDWORD;
    typedef char Byte;
    typedef QVector<char> ByteBuffer;
    //  typedef ByteBuffer::iterator BBIterator;



    class Chunk
    {
    private:
        QFile* file;
        QStringList* errorsList;

        FOURCC ckID;
        myDWORD ckSize;         //  myDWORD has my definition
        myDWORD offset;
        ChunkList* children;
        myDWORD typeID;
        myDWORD headerSize;
        ByteBuffer* data;

    public:

        Chunk(QFile* file);
        Chunk(QString name, QFile* file);
        ~Chunk(void);
        bool isList();
        ChunkList* getChildren();
        QString getName();
        void setName(QString);

        bool write();
        void addChild(Chunk* );

        bool read(void);
        int readData(Byte* data , myDWORD size=-1);

        myDWORD getHeaderOffset(void);
        myDWORD getDataOffset(void);

        QStringList getErrors(void);

        myDWORD getHeaderSize(void);
        myDWORD getDataSize(void);
        FOURCC getID(void);
        myDWORD getOffset(void);
        void setOffset(myDWORD);

        int getSize();

        QString toString(void);
        Chunk* copy(void);
        bool isOpenedForReading();

        bool addToData(Byte* bytes, myDWORD size);
        ByteBuffer* readWholeData();

        Chunk* findChunkByName(QString name, uint pos=0);

        myDWORD readDWORD(myDWORD pos=0);

        QString getType();

    protected:
        bool readHeader();
        bool readEmbeddedList();
        bool checkFilePosition(myDWORD pos);



    private:
        void init();
    };

    QString FOURCC2QString(FOURCC d);
    QString DWORD2QString(myDWORD d);
    ByteBuffer* Bytes2ByteBuffer(Byte* b , uint size);
}



#endif // CHUNK_H
