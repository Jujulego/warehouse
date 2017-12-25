#include "fenetreniveau.h"
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMessageBox>
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/personnage.hpp"


#include <QPushButton>
#include <QGraphicsProxyWidget>

#define HAUTEUR_IMAGE 64
#define LARGEUR_IMAGE 64

FenetreNiveau::FenetreNiveau(std::shared_ptr<moteur::Carte> _carte)
              :m_carte(_carte)
{
//Initialisation graphics
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setWindowTitle("LET'S PLAY !");
    setScene(new QGraphicsScene(this)); //création de la scène

    QImage mur(":/tileset/bloc/marron.png");
    QImage sol(":/tileset/sol/gris.png");
    QImage boite(":/tileset/caisse/bleue.png");
    QImage emplacement(":/tileset/environ/empl_bleu.png");
    QImage personnage(":/tileset/perso/bas_01.png");

    //Création du bouton IA
    m_boutonIAs = new QPushButton(QIcon(":/tileset/environ/empl_bleu.png"), "AIs");
    QFont PoliceIA("Calibri", 10, QFont::Bold);
    m_boutonIAs->setFont(PoliceIA);

    //Création du bouton retour menu
    m_boutonRetourMenu = new QPushButton("MAIN MENU");
    QFont PoliceRetourMenu("Calibri", 10, QFont::Bold);
    m_boutonRetourMenu->setFont(PoliceRetourMenu);

    //Intégration du bouton IA dans la scène
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    proxy->setWidget(m_boutonIAs);
    scene()->addItem(proxy);
    proxy->setPos(0, 40);
    proxy->setZValue(3);

    //Intégration du bouton Retour Menu dans la scène
    QGraphicsProxyWidget *proxyBis = new QGraphicsProxyWidget();
    proxyBis->setWidget(m_boutonRetourMenu);
    scene()->addItem(proxyBis);
    proxyBis->setZValue(3);




   /* m_boutonRetourMenu->show();*/


    for (std::shared_ptr<moteur::Immuable> obj: *m_carte){ //boucle pour parcourir la carte
        // affichage sol et mur
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(sol));
        scene()->addItem(item);
        item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
        item->setZValue(1);

        if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)){ //vérifie s'il s'agit d'un obstacle et pas d'un emplacement ou autre
             item = new QGraphicsPixmapItem(QPixmap::fromImage(mur)); //si c'est bien un obstacle la méthode renvoie le pointeur inchangé
             scene()->addItem(item);
             item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
             item->setZValue(2);
        }

        // affichage emplacements
        if (std::dynamic_pointer_cast<moteur::Emplacement>(obj)){
             item = new QGraphicsPixmapItem(QPixmap::fromImage(emplacement));
             scene()->addItem(item);
             item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
             item->setZValue(2);
        }

        // affichage déplaçables
        std::shared_ptr<moteur::Deplacable> dobj = obj->get();

        if (std::dynamic_pointer_cast<moteur::Poussable>(dobj)) {  //vérifie si c'est un poussable
            item = new QGraphicsPixmapItem(QPixmap::fromImage(boite));
            scene()->addItem(item);
            item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
            item->setZValue(3);

            m_poussable[std::dynamic_pointer_cast<moteur::Poussable>(dobj)] = item; //clé = valeur
        }

        //affichage personnage
        if (std::dynamic_pointer_cast<moteur::Personnage>(dobj)) { //vérifie si c'est un personnage
            item = new QGraphicsPixmapItem(QPixmap::fromImage(personnage));
            scene()->addItem(item);
            item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
            item->setZValue(3);

            //on stocke !
            m_perso = item;
            m_personnage = std::dynamic_pointer_cast<moteur::Personnage>(dobj);
        }
    }
}

void FenetreNiveau::updateCarte() {
    m_perso->setPos(m_personnage->coord().x()*LARGEUR_IMAGE,m_personnage->coord().y()*HAUTEUR_IMAGE);

    for (auto p : m_poussable) {
        p.second->setPos(p.first->coord().x()*LARGEUR_IMAGE,p.first->coord().y()*HAUTEUR_IMAGE);
    }
}

void FenetreNiveau::keyPressEvent(QKeyEvent* event) {
    Coord dir;
    QImage personGauche(":/tileset/perso/gauche_01.png");

    switch (event->key()) {
    case Qt::Key_Left:
        dir = GAUCHE;
        break;

    case Qt::Key_Up:
        dir = HAUT;
        break;

    case Qt::Key_Right:
        dir = DROITE;
        break;

    case Qt::Key_Down:
        dir = BAS;
        break;
    }

    if (dir != ORIGINE) {
        m_personnage->deplacer(dir);
        updateCarte();

        if (m_carte->test_fin()) {
            QMessageBox::information(this, "Victoire", "Bien joué !");
        }
    }
}
