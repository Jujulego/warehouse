#ifndef FENETREMENU_H
#define FENETREMENU_H

#include <QWidget>
#include <QPushButton>


class FenetreMenu : public QWidget {
public:
    FenetreMenu();

private:

    QPushButton* m_boutonJouer;
    QPushButton* m_boutonQuitter;


};

#endif // FENETREMENU_H
