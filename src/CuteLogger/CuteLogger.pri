INCLUDEPATH += CuteLogger/include

SOURCES += CuteLogger/src/Logger.cpp \
           CuteLogger/src/AbstractAppender.cpp \
           CuteLogger/src/AbstractStringAppender.cpp \
           CuteLogger/src/ConsoleAppender.cpp \
           CuteLogger/src/FileAppender.cpp \
           CuteLogger/src/RollingFileAppender.cpp

HEADERS += CuteLogger/include/Logger.h \
           CuteLogger/include/CuteLogger_global.h \
           CuteLogger/include/AbstractAppender.h \
           CuteLogger/include/AbstractStringAppender.h \
           CuteLogger/include/ConsoleAppender.h \
           CuteLogger/include/FileAppender.h \
           CuteLogger/include/RollingFileAppender.h

win32 {
    SOURCES += CuteLogger/src/OutputDebugAppender.cpp
    HEADERS += CuteLogger/include/OutputDebugAppender.h
}

android {
    SOURCES += src/AndroidAppender.cpp
    HEADERS += include/AndroidAppender.h
}
