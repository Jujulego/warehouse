QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG         += c++14
QMAKE_CXXFLAGS += -g3 -Wall -Wextra
INCLUDEPATH    += src
DEFINES        += QTCREATOR QT_DEPRECATED_WARNINGS

DISTFILES += \ 
    carte.txt \ 
    niveaux.xsb \ 
    xsokoban.xsb \ 
    yoshio.xsb \ 
    microban1.xsb \ 
    microban2.xsb

RESOURCES += \
	res/tileset/res-iso.qrc

HEADERS += src/moteur/carte.hpp \
	src/moteur/deplacable.hpp \
	src/moteur/emplacement.hpp \
	src/moteur/immuable.hpp \
	src/moteur/objet.hpp \
	src/moteur/obstacle.hpp \
	src/moteur/personnage.hpp \
	src/moteur/poussable.hpp \
	src/moteur/sortie.hpp \
	src/ia/ia.hpp \
	src/ia/chemin.hpp \
	src/ia/hashtable.hpp \
	src/ia/noeud.hpp \
	src/ia/solveur.hpp \
	src/ia/solveur2.hpp \
	src/ia/solveur3.hpp \
	src/affichage.hpp \
	src/devscreen.hpp \
	src/fichierxsb.hpp \
	src/menu.hpp \
	src/niveau.hpp \
	src/outils.hpp \
	src/outils/console.hpp \
	src/outils/coord.hpp \
	src/outils/hongrois.hpp \
	src/outils/infini.hpp \
	src/outils/manip.hpp \
	src/outils/matrice.hpp \
	src/outils/nombre.hpp \
	src/outils/style.hpp \
	src/qt/fenetremenu.h \
	
SOURCES += src/moteur/carte.cpp \
	src/moteur/deplacable.cpp \
	src/moteur/emplacement.cpp \
	src/moteur/immuable.cpp \
	src/moteur/objet.cpp \
	src/moteur/obstacle.cpp \
	src/moteur/personnage.cpp \
	src/moteur/poussable.cpp \
	src/moteur/sortie.cpp \
	src/ia/ia.cpp \
	src/ia/chemin.cpp \
	src/ia/hashtable.cpp \
	src/ia/noeud.cpp \
	src/ia/solveur.cpp \
	src/ia/solveur2.cpp \
	src/ia/solveur3.cpp \
	src/affichage.cpp \
	src/devscreen.cpp \
	src/fichierxsb.cpp \
	src/main.cpp \
	src/menu.cpp \
	src/niveau.cpp \
	src/outils/console.cpp \
	src/outils/coord.cpp \
	src/outils/infini.cpp \
	src/outils/manip.cpp \
	src/outils/style.cpp \
	src/qt/fenetremenu.cpp \
	
