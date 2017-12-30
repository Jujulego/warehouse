#include "fenetreregles.h"
#include "fenetremenu.h"
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QHBoxLayout>
#include <QLabel>


FenetreRegles::FenetreRegles()
{
    //Taille de la fenêtre
    setMinimumSize(1000, 600);
    setWindowTitle("RULES");
    //Fond de la fenêtre
    this->setStyleSheet("background-color: black;");

    QLabel *label = new QLabel(this);
    QLabel *label2 = new QLabel(this);


    QFont f( "Arial", 10, QFont::Bold);


    label->setAlignment(Qt::AlignCenter);
    label->setText("THE WAREHOUSE RULES\n\n\n As a Warehouse Guardian, you must put boxes on target spots. \n You can move in all four directions,"
                   " and push (but not pull) a single box at a time.\n Once all the boxes are stowed, the level is successful.\n The ideal is to succeed"
                   " with the fewest moves.");
    label->setFont(f);
    label->setStyleSheet("background-color:yellow;");
    label2->setAlignment(Qt::AlignLeft);
    //label2->setPixmap(QPixmap("C:/Users/Nawel Lalioui/Documents/warehouse/playerFace.png"));
    label2->setPixmap(QPixmap(":/tileset/perso/tete_sombre.png"));

    // create horizontal layout
    QHBoxLayout* layout=new QHBoxLayout;
    // and add label to it
    layout->addWidget(label);
    layout->addWidget(label2);


    // set layout to widget
    setLayout(layout);

    //Création du bouton retour menu
    m_boutonRetourMenu = new QPushButton("MAIN MENU");
    QFont PoliceRetourMenu("Arial", 9, QFont::Bold);
    m_boutonRetourMenu->setStyleSheet("background-color: yellow;");
    m_boutonRetourMenu->setFont(PoliceRetourMenu);

    //Intégration du bouton Retour Menu dans la scène
    layout->addWidget(m_boutonRetourMenu);

    QObject::connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(Menu_open()));
    QObject::connect(m_boutonRetourMenu, SIGNAL(clicked()), this, SLOT(close()));


}


void FenetreRegles::Menu_open(){

    FenetreMenu* Menu = new FenetreMenu();
    Menu->show();
}

