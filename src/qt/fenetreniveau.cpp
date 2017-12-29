#include "fenetreniveau.h"
#include "fenetremenu.h"
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMessageBox>
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/personnage.hpp"
#include "QPalette"


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

    //Fond de la fenêtre
    this->setStyleSheet("background-color: black;");

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
    QFont PoliceRetourMenu("Calibri", 9, QFont::Bold);
    m_boutonRetourMenu->setStyleSheet("background-color: silver;");
    m_boutonRetourMenu->setFont(PoliceRetourMenu);

    //Création du bouton nouvelle partie
    m_NouvellePartie = new QPushButton("NEW GAME");
    QFont PoliceNouvellePartie("Calibri", 10, QFont::Bold);
    m_NouvellePartie->setStyleSheet("background-color: green;");
    m_NouvellePartie->setFont(PoliceNouvellePartie);

    //Création du bouton quitter
    m_boutonQuitter = new QPushButton("QUIT");
    QFont PoliceBoutonQuitter("Calibri", 10, QFont::Bold);
    m_boutonQuitter->setStyleSheet("background-color: silver;");
    m_boutonQuitter->setFont(PoliceBoutonQuitter);

    //Intégration du bouton IA dans la scène
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    proxy->setWidget(m_boutonIAs);
    scene()->addItem(proxy);
    m_boutonIAs->setStyleSheet("background-color: red;");
    proxy->setPos(512, 200);
    proxy->setZValue(3);

    //Intégration du bouton Retour Menu dans la scène
    QGraphicsProxyWidget *proxy2 = new QGraphicsProxyWidget();
    proxy2->setWidget(m_boutonRetourMenu);
    scene()->addItem(proxy2);
    proxy2->setPos(512, 100);
    proxy2->setZValue(3);

    //Intégration du bouton nouvelle partie dans la scène
    QGraphicsProxyWidget *proxy3 = new QGraphicsProxyWidget();
    proxy3->setWidget(m_NouvellePartie);
    scene()->addItem(proxy3);
    proxy3->setPos(512, 300);
    proxy3->setZValue(3);

    //Intégration du bouton quitter dans la scène
    QGraphicsProxyWidget *proxy4 = new QGraphicsProxyWidget();
    proxy4->setWidget(m_boutonQuitter);
    scene()->addItem(proxy4);
    proxy4->setPos(512, 400);
    proxy4->setZValue(3);



    //Signaux et slots pour fermer la fenêtre niveau et réouvrir le menu quand on clicke sur le bouton Retour menu
    QObject::connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(fenMenu_open()));
    QObject::connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(close()));
    //Pour recommencer la partie
    QObject::connect(m_NouvellePartie, SIGNAL(clicked()), this, SLOT(nouvellePartie_open()));
    //Pour que lorsqu'on commence une nouvelle partie la fenêtre précédente se ferme
    QObject::connect(m_NouvellePartie, SIGNAL(clicked()), this, SLOT(close()));

    QObject::connect(m_boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));





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
    QImage personDroite(":/tileset/perso/droite_01.png");
    QImage personHaut(":/tileset/perso/haut_01.png");
    QImage personBas(":/tileset/perso/bas_01.png");

    switch (event->key()) {

    case Qt::Key_Left:
        scene()->removeItem(m_perso);
        dir = GAUCHE;
        m_perso = new QGraphicsPixmapItem(QPixmap::fromImage(personGauche));
        scene()->addItem(m_perso);
        m_perso->setZValue(3);
        break;

    case Qt::Key_Up:
        scene()->removeItem(m_perso);
        dir = HAUT;
        m_perso = new QGraphicsPixmapItem(QPixmap::fromImage(personHaut));
        scene()->addItem(m_perso);
        m_perso->setZValue(3);
        break;

    case Qt::Key_Right:
        scene()->removeItem(m_perso);
        dir = DROITE;
        m_perso = new QGraphicsPixmapItem(QPixmap::fromImage(personDroite));
        scene()->addItem(m_perso);
        m_perso->setZValue(3);
        break;

    case Qt::Key_Down:
        scene()->removeItem(m_perso);
        dir = BAS;
        m_perso = new QGraphicsPixmapItem(QPixmap::fromImage(personBas));
        scene()->addItem(m_perso);
        m_perso->setZValue(3);
        break;
    }

    if (dir != ORIGINE) {

        m_personnage->deplacer(dir);
        updateCarte();

        if (m_carte->test_fin()) {

            //QMessageBox::information(this,"Victoire", "Bien joué !");
            QMessageBox::information( 0, "Victory !" , "<font size = 5 color = red > Well done ! </font> " );

        }
    }
}


//Slots
void FenetreNiveau::fenMenu_open(){

    FenetreMenu* Menu = new FenetreMenu();
    Menu->show();
}



void FenetreNiveau::nouvellePartie_open(){

    FenetreNiveau* carte  = new FenetreNiveau(moteur::Carte::charger("../warehouse/carte.txt"));
    carte->show();

}


