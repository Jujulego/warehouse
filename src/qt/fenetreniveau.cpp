// Importations
#include <chrono>
#include <stack>

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPalette>
#include <QMediaPlayer>

#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/personnage.hpp"
#include "ia/solveur3.hpp"

#include "fenetreniveau.h"
#include "fenetremenu.h"
#include "qia.hpp"

// Namespaces
using namespace std::literals::chrono_literals;

// Macros
#ifdef __gnu_linux__
# define ECHELLE 0.5
#else
# define ECHELLE 1
#endif

#define HAUTEUR_IMAGE (64 * ECHELLE)
#define LARGEUR_IMAGE (64 * ECHELLE)

#define SOL_ATTENTE 250ms

// Constructeur
FenetreNiveau::FenetreNiveau(std::shared_ptr<moteur::Carte> _carte)
              : m_carte(_carte), m_carte_sauv(std::make_shared<moteur::Carte>(*_carte)) {

    /*QMediaPlayer* player = new QMediaPlayer(this);
    // player->setMedia(QUrl::fromLocalFile("C:/Users/Nawel Lalioui/Documents/felices.mp3"));
     player->setMedia(QUrl("C:/Users/Nawel Lalioui/Documents/Warehouse/felices.mp3"));
     player->setVolume(50);
     player->play();*/

    // Paramétrage de la fenêtre
    setStyleSheet("background-color: black;");
    setWindowTitle("LET'S PLAY !");
    grabKeyboard();

	// Initialisation graphics
    m_view = new QGraphicsView();
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->setRenderHint(QPainter::Antialiasing, true);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_view->setScene(new QGraphicsScene(this)); //création de la scène

    // Préparation font
    QFont police("Calibri", 10, QFont::Bold);

    // Création du bouton IA
    m_boutonIAs = new QPushButton(QIcon(":/tileset/environ/empl_bleu.png"), "AIs");
    m_boutonIAs->setFixedSize(200, 40);

    m_boutonIAs->setFont(police);
    m_boutonIAs->setStyleSheet("background-color: red;");

    // Gestion de l'IA
    connect(m_boutonIAs, SIGNAL(clicked()), this, SLOT(demarer_ia()));

    // Création du bouton retour menu
    m_boutonRetourMenu = new QPushButton("MAIN MENU");
    m_boutonRetourMenu->setFixedSize(200, 40);

    m_boutonRetourMenu->setFont(police);
    m_boutonRetourMenu->setStyleSheet("background-color: silver;");

    // Fermeture de la fenêtre niveau et réouverture du menu quand on clicke sur le bouton Retour menu
    connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(fenMenu_open()));
    connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(close()));

    // Création du bouton nouvelle partie
    m_boutonNouvellePartie = new QPushButton("NEW GAME");
    m_boutonNouvellePartie->setFixedSize(200, 40);

    m_boutonNouvellePartie->setFont(police);
    m_boutonNouvellePartie->setStyleSheet("background-color: green;");

    // Recommencer la partie
    connect(m_boutonNouvellePartie, SIGNAL(clicked()), this, SLOT(recommencer()));

    //Création du bouton quitter
    m_boutonQuitter = new QPushButton("QUIT");
    m_boutonQuitter->setFixedSize(200, 40);

    m_boutonQuitter->setFont(police);
    m_boutonQuitter->setStyleSheet("background-color: silver;");

    // Arrêt de l'application
    connect(m_boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));

    // Création des layout
    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    vlayout->setSpacing(60);

    vlayout->addWidget(m_boutonRetourMenu);
    vlayout->addWidget(m_boutonNouvellePartie);
    vlayout->addWidget(m_boutonIAs);
    vlayout->addWidget(m_boutonQuitter);

    QHBoxLayout* hlayout = new QHBoxLayout;
    hlayout->addWidget(m_view);
    hlayout->addLayout(vlayout);

    setLayout(hlayout);

    // Initialisation du timer
    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(appliquer_mvt()));

	// Affichage de la carte
    QImage mur(":/tileset/bloc/marron.png");
    QImage sol(":/tileset/sol/gris.png");
    QImage boite(":/tileset/caisse/bleue.png");
    QImage emplacement(":/tileset/environ/empl_bleu.png");
    QImage personnage(":/tileset/perso/bas_01.png");

    for (std::shared_ptr<moteur::Immuable> obj: *m_carte){ //boucle pour parcourir la carte
        // affichage sol et mur
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(sol));
        m_view->scene()->addItem(item);
        item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
        item->setScale(ECHELLE);
        item->setZValue(1);

        if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)){ //vérifie s'il s'agit d'un obstacle et pas d'un emplacement ou autre
            item = new QGraphicsPixmapItem(QPixmap::fromImage(mur)); //si c'est bien un obstacle la méthode renvoie le pointeur inchangé
            m_view->scene()->addItem(item);
            item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
            item->setScale(ECHELLE);
            item->setZValue(2);
        }

        // affichage emplacements
        if (std::dynamic_pointer_cast<moteur::Emplacement>(obj)){
            item = new QGraphicsPixmapItem(QPixmap::fromImage(emplacement));
            m_view->scene()->addItem(item);
            item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
            item->setScale(ECHELLE);
            item->setZValue(2);
        }

        // affichage déplaçables
        std::shared_ptr<moteur::Deplacable> dobj = obj->get();

        if (std::dynamic_pointer_cast<moteur::Poussable>(dobj)) {  //vérifie si c'est un poussable
            item = new QGraphicsPixmapItem(QPixmap::fromImage(boite));
            m_view->scene()->addItem(item);
            item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
            item->setScale(ECHELLE);
            item->setZValue(3);

            m_poussable[std::dynamic_pointer_cast<moteur::Poussable>(dobj)] = item; //clé = valeur
        }

        //affichage personnage
        if (std::dynamic_pointer_cast<moteur::Personnage>(dobj)) { //vérifie si c'est un personnage
            item = new QGraphicsPixmapItem(QPixmap::fromImage(personnage));
            m_view->scene()->addItem(item);
            item->setPos(obj->coord().x()*LARGEUR_IMAGE,obj->coord().y()*HAUTEUR_IMAGE);
            item->setScale(ECHELLE);
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

void FenetreNiveau::updatePointers() {
    m_personnage = m_carte->personnage();

    std::list<std::shared_ptr<moteur::Poussable>> poussables = m_carte->liste<moteur::Poussable>();
    std::list<std::shared_ptr<moteur::Poussable>> old;

    for (auto p : m_poussable) {
        old.push_back(p.first);
    }

    auto pit = poussables.begin();
    for (auto p : old) {
        auto it = m_poussable.find(p);

        m_poussable[*pit] = it->second;
        m_poussable.erase(it);

        ++pit;
    }

    updateCarte();
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

    case Qt::Key_Z:
       annulerCoup();
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
    m_view->scene()->removeItem(m_perso);

    m_perso = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    m_view->scene()->addItem(m_perso);
    m_perso->setScale(ECHELLE);
    m_perso->setZValue(3);

    // Application du mouvement
    auto copie = std::make_shared<moteur::Carte>(*m_carte);
    int nb_push = 0;

    m_personnage->deplacer(dir, nb_push);
    if (nb_push) m_historique.push(copie);

    updateCarte();

    if (m_carte->test_fin()) {
        QMessageBox::information( 0, "Victory !" , "<font size = 5 color = red > Well done ! </font>" );
    }
}

// Slots
void FenetreNiveau::fenMenu_open() {
    if (m_ia && m_ia->isRunning()) m_ia->interrompre();

    FenetreMenu* Menu = new FenetreMenu();
    Menu->show();
}

void FenetreNiveau::recommencer() {
    if (m_ia && m_ia->isRunning()) m_ia->interrompre();

    // Retour au début !
    m_carte = std::make_shared<moteur::Carte>(*m_carte_sauv);
    updatePointers();
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
    if (m_historique.size() == 0) return;

    m_carte = m_historique.top();
    updatePointers();
    m_historique.pop();
}


