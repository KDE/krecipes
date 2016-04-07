/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREALLINGREDIENTSMODELS_H
#define KREALLINGREDIENTSMODELS_H

#include <QObject>

class RecipeDB;
class QStandardItemModel;

class KreAllIngredientsModels: public QObject {
Q_OBJECT

public:
	KreAllIngredientsModels( RecipeDB * database );

	QStandardItemModel * sourceModel();

private:
	RecipeDB * m_database;
	QStandardItemModel * m_sourceModel;
	
};

#endif
