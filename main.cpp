#include <QtCore>
#include "WaveRegionFile.h"

using namespace RiffFileNamespace;

double samples2time(int samples, WaveFileInfo* info){
        return ((double)samples/(double)info->sampleRate)*1000.;
}

int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

    //	argc = 2;
    //  argv = {"", ""};
    //	argv[0] = "ZH102Y220.wav";
    //	argv[1] = "regions.txt";
    if (argc==3){
        QFile file(argv[1]);
        WaveRegionFile waveFile(&file);
        waveFile.open(QIODevice::ReadOnly);

        waveFile.read();

        QFile outputfile(QString::fromLocal8Bit(argv[2]));
        outputfile.open(QIODevice::WriteOnly);
        QTextStream out(&outputfile);
        WaveFileInfo* info = waveFile.getInfo();
        RegionList* regions = waveFile.getRegions();
        Region* r;
        foreach(r, *regions){
                int startInMSec		 = samples2time(r->begin, info)*10000.;
                int endInMSec		 = samples2time(r->end  , info)*10000.;
                QString s1 = QString::number(startInMSec);
                QString s2 = QString::number(endInMSec);`

                out << s1 << " " << s2 << " " << r->name << "\n";
            //  QString str = durInMMs
            //  outputfile.write();
        }

        waveFile.close();
        outputfile.close();
//        getRootChunk
    } else {
        qDebug() << "there should be two paramentres: "<<
                "wav file with regions and text file where to place regions info";
    }
	return 0;
//    return a.exec();
}
