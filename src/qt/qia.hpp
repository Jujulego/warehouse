#pragma once

// Importations
#include <memory>

#include <QThread>

#include "ia/chemin.hpp"
#include "ia/ia.hpp"

// Classe
class QIA : public QThread {
	Q_OBJECT

	private:
		// Attributs
		std::shared_ptr<ia::IA> m_ia;

	public:
		// Constructeur
		QIA(std::shared_ptr<ia::IA> ia, QObject* parent = nullptr);

	signals:
		// Signaux
		void resultat(ia::Chemin const&);

	protected:
		// Méthodes
		virtual void run() override;
};
