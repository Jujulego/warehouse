#ifndef FENETREREGLES_H
#define FENETREREGLES_H

#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QGraphicsView>
#include <QLabel>


class FenetreRegles : public QLabel {
Q_OBJECT
public:
    FenetreRegles();

public slots:
    void Menu_open();

private:
     QPushButton* m_boutonRetourMenu;

};

#endif // FENETREREGLES_H

