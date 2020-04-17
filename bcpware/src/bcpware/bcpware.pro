include (../general.pri)
#CONFIG += debug_and_release
DESTDIR = ../distrib
EXIF_DIR = ../external/jhead-2.95

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

INCLUDEPATH *= . \
	.. \
        ../../ \
    $$VCGDIR \
    $$GLEWDIR/include \
	$$EXIF_DIR
DEPENDPATH += $$VCGDIR \
    $$VCGDIR/vcg \
    $$VCGDIR/wrap
HEADERS = ../common/interfaces.h \
    mainwindow.h \
    glarea.h \
    multiViewer_Container.h \
    glarea_setting.h \
    plugindialog.h \
    customDialog.h \
    filterScriptDialog.h \
    saveSnapshotDialog.h \
    savemaskexporter.h \
    changetexturename.h \
    layerDialog.h \
    stdpardialog.h \
	xmlstdpardialog.h \
	additionalgui.h \
	xmlgeneratorgui.h \
	snapshotsetting.h \
	rendermodeactions.h \
    $$VCGDIR/wrap/gui/trackball.h \
    $$VCGDIR/wrap/gui/trackmode.h \
    $$VCGDIR/wrap/gl/trimesh.h \
	filterthread.h \    
    meshcheck.h \
    customizewidget.h \
    gl_label_SKT.h \
    skt_function.h \
    process_wrapper.h \
    transformPreview.h \
    printerJobInfo.h \   
    alnParser.h \
    maskSave.h \
    meshlabeditdockwidget.h \
    monitor.h \
    printer_info.h \
    SaveDialogTest.h \
    ui_aboutDialog.h \
    ui_congratsDialog.h \
    ui_customDialog.h \
    ui_filtercreatortab.h \
    ui_filtergui.h \
    ui_filterScriptDialog.h \
    ui_layerDialog.h \
    ui_meshcheck.h \
    ui_renametexture.h \
    ui_savemaskexporter.h \
    ui_savesnapshotDialog.h \
    ui_printdialog.h\
    chipcommunication.h \
    json_batchallocator.h \
    TcpClient.h \
    widgetmonitor.h \
    gui_launcher.h \
	saveproject.h \
    my_thread.h
SOURCES = main.cpp \
    mainwindow_Init.cpp \
    mainwindow_RunTime.cpp \
    glarea.cpp \
	multiViewer_Container.cpp \
    plugindialog.cpp \
    customDialog.cpp \
    filterScriptDialog.cpp \
    saveSnapshotDialog.cpp \
    layerDialog.cpp \
    savemaskexporter.cpp \
    changetexturename.cpp \
    stdpardialog.cpp \
    xmlstdpardialog.cpp \
	additionalgui.cpp \
	xmlgeneratorgui.cpp \
	rendermodeactions.cpp \
	$$VCGDIR/wrap/gui/trackball.cpp \
    $$VCGDIR/wrap/gui/trackmode.cpp \
    glarea_setting.cpp \
    filterthread.cpp \
    meshcheck.cpp \
    customizewidget.cpp \
    process_wrapper.cpp \
    transformPreview.cpp \
    printerJobInfo.cpp \    
    glarea.cpp \
    maskSave.cpp \
    meshcheck.cpp \
    monitor.cpp \
    SaveDialogTest.cpp \
    chipcommunication.cpp \
    json_reader.cpp \
    json_value.cpp \
    json_writer.cpp \
    TcpClient.cpp \
	saveproject.cpp   \ 
    widgetmonitor.cpp

FORMS = ui/layerDialog.ui \
    ui/filterScriptDialog.ui \
    ui/customDialog.ui \
    ui/savesnapshotDialog.ui \
    ui/renametexture.ui \
    ui/savemaskexporter.ui \
    ui/congratsDialog.ui \
	ui/filtergui.ui \
	ui/filtercreatortab.ui \
    ui/meshcheck.ui \
    ui/printdialog.ui \
    ui/slice_slider_ui.ui \
    ui/checklist.ui \
    ui/objectList_ui.ui \
    ui/printOption.ui \
    ui/PrintOptionDia_ui.ui \
    ui/setting_UI.ui \
    ui/dockInfoWidgetUI.ui \
    ui/meshCheck2_ui.ui \
    ui/palette.ui \
    ui/support.ui \
    ui/meshcheck3.ui \
    login.ui \
    ui/About.ui
	
TRANSLATIONS += picasso_zh.ts
	
win32-msvc2013: RCC_DIR = $(ConfigurationName)


RESOURCES = meshlab.qrc
#20150413 ???????????ùrc??±ug
RCC_DIR = ./qrc


# to add windows icon
win32:RC_FILE = meshlab.rc

# ## the xml info list
# ## the next time the app open a new extension
QMAKE_INFO_PLIST = ../install/info.plist

# to add MacOS icon
ICON = images/meshlab.icns

# note that to add the file icons on the mac the following line does not work.
# You have to copy the file by hand into the meshlab.app/Contents/Resources directory.
# ICON += images/meshlab_obj.icns
QT += opengl
QT += xml
QT += xmlpatterns
QT += network
QT += script
QT += printsupport#20150901
QT += serialport
QT += concurrent

LIBS += WS2_32.Lib
LIBS += shell32.Lib
# the following line is needed to avoid mismatch between
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

# the following line is to hide the hundred of warnings about the deprecated
# old printf are all around the code
win32-msvc2013:DEFINES += _CRT_SECURE_NO_DEPRECATE



CONFIG += stl

TARGET = picasso


win32-msvc2013:LIBS		+= -L../external/lib/win32-msvc2013 -ljhead -L../distrib -lcommon

win32-g++:LIBS        	+= -L../external/lib/win32-gcc -ljhead -L../distrib -lcommon

#CONFIG(release,debug | release) {
#	win32-msvc2005:release:LIBS     += -L../common/release -lcommon
#	win32-msvc2008:release:LIBS     += -L../common/release -lcommon
#	win32-msvc2013:release:LIBS     += -L../common/release -lcommon
#	win32-g++:release:LIBS 			+= -L../common/release -lcommon
#}

#Debug:OBJECTS_DIR =x64/Debug

# uncomment in your local copy only in emergency cases.
# We should never be too permissive
# win32-g++:QMAKE_CXXFLAGS += -fpermissive

# The following define is needed in gcc to remove the asserts
win32-g++:DEFINES += NDEBUG
CONFIG(debug, debug|release):win32-g++:release:DEFINES -= NDEBUG
