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
     void fenNiveau_open();
     void fenRegles_open();
     void charger();


 private:

    QPushButton* m_boutonJouer;
    QPushButton* m_boutonQuitter;
    QPushButton* m_boutonRegles;
    QPushButton* m_boutonCharger;
    /*QPushButton* m_boutonIA;*/


};

#endif // FENETREMENU_H
