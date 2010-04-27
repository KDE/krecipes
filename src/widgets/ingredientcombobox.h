/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTCOMBOBOX_H
#define INGREDIENTCOMBOBOX_H

#include <kcombobox.h>

#include <qmap.h>

#include "datablocks/element.h"

class QTimer;

class RecipeDB;

class IngredientComboBox : public KComboBox
{
	Q_OBJECT

public:
	IngredientComboBox( bool, QWidget *parent, RecipeDB *db, const QString &specialItem = QString() );

	void reload();
	int id( int row );
	int id( const QString &ing );

	void startLoad();
	void endLoad();

private slots:
	void createIngredient( const Element &element );
	void removeIngredient( int id );

	int findInsertionPoint( const QString &name );
	void loadMore();

private:
	RecipeDB *database;
	QMap<int, int> ingredientComboRows; // Contains the category id for every given row in the category combobox

	int loading_at;
	int ing_count;
	int load_limit;
	QTimer *load_timer;
	QString m_specialItem;
};

#endif //INGREDIENTCOMBOBOX_H

