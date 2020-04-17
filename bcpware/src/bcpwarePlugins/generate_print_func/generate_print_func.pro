include (../../shared.pri)

HEADERS       += \
    convertQI_CV.h \    
    slice_function.h \
    zxgen.h \
    IAPI.h \
    sirius_print_job.h \
    generate_print_func.h \
    printthread.h \
    image_paralle.h \
    image_process.h \



SOURCES       += \ 
    generate_print_func.cpp \
    printthread.cpp \
    winapi_io.cpp
		
TARGET        = generate_print_func

RESOURCES += \
    generate_print_func.qrc
RCC_DIR = ./qrc
QT += opengl concurrent widgets


INCLUDEPATH += C:/OpenCV_3_0_0/opencv/build/include

LIBS += WS2_32.Lib
LIBS += SETUPAPI.Lib
LIBS += opencv_ts300.lib
LIBS += opencv_world300.lib
LIBS += C:/OpenCV_3_0_0/opencv/build/x64/vc12/lib/opencv_ts300d.lib
LIBS += C:/OpenCV_3_0_0/opencv/build/x64/vc12/lib/opencv_world300d.lib
LIBS += C:/OpenCV_3_0_0/opencv/build/x64/vc12/lib/opencv_ts300.lib
LIBS += C:/OpenCV_3_0_0/opencv/build/x64/vc12/lib/opencv_world300.lib

LIBS += ../../distrib/IAPI.lib
#LIBS += C:/"Program Files (x86)"/"Microsoft Visual Studio 12.0"/VC/lib/glut32.lib
