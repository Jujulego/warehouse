// Importations
#include <chrono>

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <stack>

#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/personnage.hpp"
#include "ia/solveur3.hpp"
#include "QPalette"
#include <QMediaPlayer>

#include "fenetreniveau.h"
#include "fenetremenu.h"
#include "qia.hpp"

// Namespaces
using namespace std::literals::chrono_literals;

// Macros
#define HAUTEUR_IMAGE 64
#define LARGEUR_IMAGE 64

#define SOL_ATTENTE 250ms

// Constructeur
FenetreNiveau::FenetreNiveau(std::shared_ptr<moteur::Carte> _carte)
              :m_carte(_carte)
{

    /*QMediaPlayer* player = new QMediaPlayer(this);
    // player->setMedia(QUrl::fromLocalFile("C:/Users/Nawel Lalioui/Documents/felices.mp3"));
     player->setMedia(QUrl("C:/Users/Nawel Lalioui/Documents/Warehouse/felices.mp3"));
     player->setVolume(50);
     player->play();*/

	// Initialisation graphics
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setWindowTitle("LET'S PLAY !");
    setScene(new QGraphicsScene(this)); //création de la scène

    //Fond de la fenêtre
    this->setStyleSheet("background-color: black;");

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

    //Création du bouton annuler
    m_boutonAnnulerCoup = new QPushButton("ANNULER");
    QFont PoliceBoutonAnnulerCoup("Calibri", 10, QFont::Bold);
    m_boutonAnnulerCoup->setStyleSheet("background-color: silver;");
    m_boutonAnnulerCoup->setFont(PoliceBoutonAnnulerCoup);

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

    //Intégration du bouton annuler dans la scène
    QGraphicsProxyWidget *proxy5 = new QGraphicsProxyWidget();
    proxy5->setWidget(m_boutonAnnulerCoup);
    scene()->addItem(proxy5);
    proxy5->setPos(512, 500);
    proxy5->setZValue(3);

    // Initialisation du timer
    m_timer = new QTimer(this);

    //Signaux et slots pour fermer la fenêtre niveau et réouvrir le menu quand on clicke sur le bouton Retour menu
    QObject::connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(fenMenu_open()));
    QObject::connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(close()));
    //Pour recommencer la partie
    QObject::connect(m_NouvellePartie, SIGNAL(clicked()), this, SLOT(nouvellePartie_open()));
    //Pour que lorsqu'on commence une nouvelle partie la fenêtre précédente se ferme
    QObject::connect(m_NouvellePartie, SIGNAL(clicked()), this, SLOT(close()));

    QObject::connect(m_boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));
    QObject::connect(m_boutonAnnulerCoup, SIGNAL(clicked()), this, SLOT(annulerCoup()));

	// Gestion de l'IA
    connect(m_boutonIAs, SIGNAL(clicked()), this, SLOT(demarer_ia()));
    connect(m_timer,     SIGNAL(timeout()), this, SLOT(appliquer_mvt()));


	// Affichage de la carte
    QImage mur(":/tileset/bloc/marron.png");
    QImage sol(":/tileset/sol/gris.png");
    QImage boite(":/tileset/caisse/bleue.png");
    QImage emplacement(":/tileset/environ/empl_bleu.png");
    QImage personnage(":/tileset/perso/bas_01.png");

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

    appliquer_mvt(dir);
}

void FenetreNiveau::appliquer_mvt(Coord const& dir) {
    // Gardien
    if (dir == ORIGINE) return;

    // Choix de l'image
    QImage img;

    if (dir == HAUT) {
        img = QImage(":/tileset/perso/haut_01.png");
    } else if (dir == BAS) {
        img = QImage(":/tileset/perso/bas_01.png");
    } else if (dir == GAUCHE) {
        img = QImage(":/tileset/perso/gauche_01.png");
    } else if (dir == DROITE) {
        img = QImage(":/tileset/perso/droite_01.png");
    }

    // changement
    scene()->removeItem(m_perso);

    m_perso = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    scene()->addItem(m_perso);
    m_perso->setZValue(3);

    // Application du mouvement
    if(m_personnage->deplacer(dir)==false){

    m_pile.push(std::make_shared<moteur::Carte>(*m_carte));

    }

    updateCarte();

    if (m_carte->test_fin()) {
        QMessageBox::information( 0, "Victory !" , "<font size = 5 color = red > Well done ! </font> " );
    }
}

// Slots
void FenetreNiveau::fenMenu_open() {
    FenetreMenu* Menu = new FenetreMenu();
    Menu->show();
}

void FenetreNiveau::nouvellePartie_open() {
    FenetreNiveau* carte = new FenetreNiveau(moteur::Carte::charger("../warehouse/carte.txt"));
    carte->show();
}

void FenetreNiveau::demarer_ia() {
	// Gardien
	if (m_ia && m_ia->isRunning()) return;

	// Lancement de l'IA
	m_ia = new QIA(std::make_shared<ia::Solveur3>(m_carte, m_personnage), this);
	m_ia->start();

	connect(m_ia, &QIA::resultat, this, &FenetreNiveau::recv_chemin);
}

void FenetreNiveau::recv_chemin(ia::Chemin const& ch) {
	m_chemin = ch;
	m_timer->start(SOL_ATTENTE);
}

void FenetreNiveau::appliquer_mvt() {
	// Gardien
	if (m_chemin.longueur() == 0) return;

	// Application du mouvement
    appliquer_mvt(m_chemin.pop());

	// Fin ?
	if (m_chemin.longueur() == 0) m_timer->stop();
}

void FenetreNiveau::annulerCoup() {

   if(m_pile.size()==0) return;

   m_carte=m_pile.top();
   m_personnage=m_carte->personnage();

   std::list<std::shared_ptr<moteur::Poussable>> poussables = m_carte->liste<moteur::Poussable>();
   auto it = poussables.begin();

   for (auto p : m_poussable) {
       m_poussable[*it] = p.second;
       m_poussable.erase(p.first);

       ++it;
   }

   updateCarte();
   m_pile.pop();


}
