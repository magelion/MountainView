GLM_PATH  = ../../ext/glm-0.9.4.1

TEMPLATE  = app
TARGET    = mountainview

LIBS     += -lGLEW -lGL -lGLU -lm
INCLUDEPATH  +=  $${GLM_PATH}

SOURCES   = ./src/*.cpp
HEADERS   = ./src/*.h

CONFIG   += qt opengl warn_on thread uic4 release
QT       *= xml opengl core
