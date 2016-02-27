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

class IngredientList;

class QStandardItemModel;


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

private:
	void resizeColumnsToContents();

	Ui::IngredientsEditor * ui;

	IngredientList * m_ingredientList;

	QStandardItemModel * m_sourceModel;
};

#endif
