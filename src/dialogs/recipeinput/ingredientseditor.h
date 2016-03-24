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

#include "backends/recipedb.h"
#include "datablocks/ingredientlist.h"

#include <QHash>

class RecipeDB;
class IngredientList;
class NutrientInfoDetailsDialog;

class QStandardItemModel;
class QModelIndex;


namespace Ui{
	class IngredientsEditor;
}

class IngredientsEditor : public QWidget
{
Q_OBJECT

public:
	IngredientsEditor( QWidget * parent = 0 );
	~IngredientsEditor() {};

	enum UserRoles {
		IdRole = Qt::UserRole,
		IsHeaderRole = Qt::UserRole+1,
		IsPluralRole = Qt::UserRole+2
	};

	void setDatabase( RecipeDB * database );

	void loadIngredientList( IngredientList * ingredientList );

	void updateIngredientList();

signals:
	void changed();

private slots:
	void addIngredientSlot();
	void addAltIngredientSlot();
	void addHeaderSlot();

	void ingParserSlot();

	void moveIngredientUpSlot();
	void moveIngredientDownSlot();

	void removeIngredientSlot();

	void nutrientInfoDetailsSlot();

private:
	void setRowData( int row, const Ingredient & ingredient );
	void setRowData( int row, const Element & header );
	void setRowDataAlternative( int row, const IngredientData & ingredient,
		const QModelIndex & parent );

	void resizeColumnsToContents();
	void moveRow( int offset );

	Ingredient readIngredientFromRow( int row );

	Ui::IngredientsEditor * ui;

	IngredientList * m_ingredientList;
	RecipeDB * m_database;

	QStandardItemModel * m_sourceModel;

	NutrientInfoDetailsDialog * m_nutrientInfoDetailsDialog;

	int m_nutrientIncompleteCount;
	int m_nutrientIntermediateCount;
};

#endif
