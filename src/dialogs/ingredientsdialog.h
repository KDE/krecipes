/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTSDIALOG_H
#define INGREDIENTSDIALOG_H

#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qwidget.h>
#include <knuminput.h>

#include "widgets/krelistview.h"
#include "datablocks/unit.h"

class RecipeDB;
class ElementList;
class SelectUnitDialog;
class CreateElementDialog;
class IngredientPropertyList;
class SelectPropertyDialog;
class EditBox;
class UnitsDialog;

/**
@author Unai Garro
*/
class IngredientsDialog: public QWidget
{

	Q_OBJECT

public:
	IngredientsDialog( QWidget* parent, RecipeDB *db );
	~IngredientsDialog();
	void reload( void );

private:
	// Widgets
	QGridLayout* layout;
	QPushButton* addIngredientButton;
	QPushButton* removeIngredientButton;
	QPushButton* addUnitButton;
	QPushButton* removeUnitButton;
	QPushButton* addPropertyButton;
	QPushButton* removePropertyButton;
	KreListView* ingredientListView;
	KreListView* unitsListView;
	KreListView* propertiesListView;
	QPushButton* pushButton5;
	EditBox* inputBox;

	// Internal Methods
	void reloadIngredientList( void );
	void reloadUnitList( void );
	void reloadPropertyList( void );
	int findPropertyNo( QListViewItem *it );
	// Internal Variables
	RecipeDB *database;
	ElementList *ingredientList;
	UnitList *unitList;
	IngredientPropertyList *propertiesList;
	ElementList *perUnitListBack;

private slots:
	void addUnitToIngredient( void );
	void removeUnitFromIngredient( void );
	void updateLists( void );
	void addPropertyToIngredient( void );
	void removePropertyFromIngredient( void );
	void insertPropertyEditBox( QListViewItem* it );
	void setPropertyAmount( double amount );
	void openUSDADialog( void );

};

#endif
