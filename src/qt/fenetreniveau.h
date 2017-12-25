#ifndef FENETRENIVEAU_H
#define FENETRENIVEAU_H

#include <QGraphicsView>
#include "moteur/carte.hpp"
#include <memory>
#include <map>
#include <QGraphicsPixmapItem>
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"
#include <QPushButton>


class FenetreNiveau : public QGraphicsView{

public:
    FenetreNiveau(std::shared_ptr<moteur::Carte> _carte);
    virtual void keyPressEvent(QKeyEvent* event);
    void updateCarte();

public slots:
    void fenMenu_open();

private:
    std::shared_ptr<moteur::Carte> m_carte;

    std::map<std::shared_ptr<moteur::Poussable>,QGraphicsPixmapItem*> m_poussable;

    QGraphicsPixmapItem* m_perso; // l'image doit pouvoir suivre le personnage
    std::shared_ptr<moteur::Personnage> m_personnage; //pour le déplacement du personnage

    QPushButton* m_boutonIAs;
    QPushButton* m_boutonRetourMenu;
    QPushButton* m_NouvellePartie;

};

#endif // FENETRENIVEAU_H
