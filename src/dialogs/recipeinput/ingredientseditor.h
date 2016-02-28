/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef INGREDIENTSEDITOR_H
#define INGREDIENTSEDITOR_H


#include <QWidget>
#include <QHash>

#include "datablocks/ingredientlist.h"

class IngredientList;

class QStandardItemModel;
class QPersistentModelIndex;


namespace Ui{
	class IngredientsEditor;
}

class IngredientsEditor : public QWidget
{
Q_OBJECT

public:
	IngredientsEditor( QWidget * parent = 0 );
	~IngredientsEditor() {};

	void loadIngredients( IngredientList * ingredientList );

private slots:
	void removeIngredientSlot();
	void moveIngredientUpSlot();
	void moveIngredientDownSlot();

private:
	void resizeColumnsToContents();
	void moveColumn( int offset );

	Ui::IngredientsEditor * ui;

	IngredientList * m_ingredientList;

	QStandardItemModel * m_sourceModel;

	//FIXME: Use this to update the list on the fly or remove
	QHash<QPersistentModelIndex,IngredientList::iterator> m_indexToListIteratorMap;
	QHash<QPersistentModelIndex,Ingredient::SubstitutesList::iterator> m_indexToSubsListIteratorMap;
};

#endif
