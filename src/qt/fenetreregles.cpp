#include "fenetreregles.h"
#include <QWidget>
#include <QGraphicsView>

FenetreRegles::FenetreRegles()
{
    //Taille de la fenêtre
    setMinimumSize(600, 400);
    setWindowTitle("RULES");

    setScene(new QGraphicsScene(this));
    scene()->addText("TEST POUR ECRIRE LES REGLES");



}
