QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++23
LIBS += \
  -ldcmpstat \
  -ldcmnet \
  -ldcmdata \
  -ldcmimgle \
  -ldcmdsig \
  -ldcmsr \
  -ldcmtls \
  -ldcmqrdb \
  -ldcmjpls \
  -ldcmjpeg \
  -ldcmimage \
#-ldcmect \
  -ldcmfg \
  -ldcmiod \
  -ldcmpmap \
  -ldcmqrdb \
  -ldcmrt \
  -ldcmseg \
  -ldcmwlm \
  -lxml2 \
  -loflog \
  -lofstd \
  -lz


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    qt_widgets_dcmtk_viewer_pt_BR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	Dockerfile \
	README.md
