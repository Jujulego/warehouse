#ifndef FENETRENIVEAU_H
#define FENETRENIVEAU_H

// Importations
#include <memory>
#include <map>
#include <stack>

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QWidget>
#include <QApplication>
#include <QTimer>

#include "moteur/carte.hpp"
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"
#include "ia/chemin.hpp"

#include "qia.hpp"

// Classe
class FenetreNiveau : public QGraphicsView {
    Q_OBJECT

public:
    FenetreNiveau(std::shared_ptr<moteur::Carte> _carte);
    virtual void keyPressEvent(QKeyEvent* event);
    void appliquer_mvt(Coord const& dir);
    void updateCarte();

public slots:
    void fenMenu_open();
    void nouvellePartie_open();


    void demarer_ia();
    void recv_chemin(ia::Chemin const& ch);
    void appliquer_mvt();
    void annulerCoup();


private:
    std::shared_ptr<moteur::Carte> m_carte;

    std::map<std::shared_ptr<moteur::Poussable>,QGraphicsPixmapItem*> m_poussable;

    QGraphicsPixmapItem* m_perso; // l'image doit pouvoir suivre le personnage
    std::shared_ptr<moteur::Personnage> m_personnage; //pour le déplacement du personnage

    QPushButton* m_boutonIAs;
    QPushButton* m_boutonRetourMenu;
    QPushButton* m_NouvellePartie;
    QPushButton* m_boutonQuitter;
    //QPushButton* m_boutonAnnulerCoup;

    QIA* m_ia = nullptr;

    QTimer* m_timer;
    ia::Chemin m_chemin;

    //Pile pour annuler un coup
    std::stack<std::shared_ptr<moteur::Carte>> m_pile;

};

#endif // FENETRENIVEAU_H
