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
class FenetreNiveau : public QWidget {
    Q_OBJECT

public:
    FenetreNiveau(std::shared_ptr<moteur::Carte> _carte);
    virtual void keyPressEvent(QKeyEvent* event);
    void appliquer_mvt(Coord const& dir);
    void updateCarte();
    void updatePointers();

public slots:
    void fenMenu_open();
    void recommencer();

    void demarer_ia();
    void recv_chemin(ia::Chemin const& ch);
    void appliquer_mvt();
    void annulerCoup();

private:
    // Attributs
    std::shared_ptr<moteur::Carte> m_carte;
    std::shared_ptr<moteur::Carte> m_carte_sauv;

    std::map<std::shared_ptr<moteur::Poussable>,QGraphicsPixmapItem*> m_poussable;

    QGraphicsPixmapItem* m_perso; // l'image doit pouvoir suivre le personnage
    std::shared_ptr<moteur::Personnage> m_personnage; //pour le déplacement du personnage

    QGraphicsView* m_view;
    QPushButton* m_boutonIAs;
    QPushButton* m_boutonRetourMenu;
    QPushButton* m_boutonNouvellePartie;
    QPushButton* m_boutonQuitter;

    QTimer* m_timer;
    QIA* m_ia = nullptr;
    ia::Chemin m_chemin;

    // Pile pour annuler un coup
    std::stack<std::shared_ptr<moteur::Carte>> m_historique;

};

#endif // FENETRENIVEAU_H
