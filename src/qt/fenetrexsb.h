#ifndef FENETREXSB_H
#define FENETREXSB_H

#include <qstring.h>
#include <QPushButton>

class FenetreXSB : public QWidget
{
public:
    FenetreXSB(QString _fichier);

private:

    QString m_fichier;
    QPushButton* m_boutonxsb;
};

#endif // FENETREXSB_H
