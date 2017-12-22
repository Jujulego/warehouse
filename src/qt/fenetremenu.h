#ifndef FENETREMENU_H
#define FENETREMENU_H

#include <QWidget>
#include <QPushButton>
#include <QApplication>


class FenetreMenu : public QWidget {
    Q_OBJECT

public:
    FenetreMenu();

public slots:
     void fen_open();

private:

    QPushButton* m_boutonJouer;
    QPushButton* m_boutonQuitter;
    QPushButton* m_boutonRegles;


};

#endif // FENETREMENU_H
