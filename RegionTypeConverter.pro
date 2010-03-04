#-------------------------------------------------
#
# Project created by QtCreator 2010-03-01T12:57:49
#
#-------------------------------------------------

QT       -= gui

TARGET = RegionTypeConverter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS +=	RiffFile.h			\
                chunk.h				\
                wavefile.h			\
                WaveRegionFile.h

SOURCES +=  main.cpp            \
            chunk.cpp           \
            RiffFile.cpp        \
            wavefile.cpp        \
            WaveRegionFile.cpp
