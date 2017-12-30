// Importations
#include <iostream>
#include <memory>

#include <QThread>

#include "outils/posstream.hpp"
#include "ia/chemin.hpp"
#include "ia/ia.hpp"

#include "qia.hpp"

// Constructeur
QIA::QIA(std::shared_ptr<ia::IA> ia, QObject* parent) : QThread(parent), m_ia(ia) {}

// Méthodes
void QIA::run() {
	posstream<std::ostream> stream(&std::cout, 0, 0);
	ia::Chemin chemin = m_ia->resoudre(stream);

	emit resultat(chemin);
}
