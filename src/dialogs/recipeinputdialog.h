/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef RECIPEINPUTDIALOG_H
#define RECIPEINPUTDIALOG_H

#include <kdialog.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <klistview.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qobject.h>

#include "recipe.h"
#include "recipedb.h"

/**
@author Unai Garro
*/
class RecipeInputDialog: public QWidget{
Q_OBJECT

public:

    RecipeInputDialog( QWidget* parent, RecipeDB* db);
    void loadRecipe(int recipeID);
    ~RecipeInputDialog();
private:

	// Internal Data
	Recipe *loadedRecipe; //Loaded Recipe
	ElementList *ingredientComboList;
	ElementList *unitComboList;
	RecipeDB *database;
	bool changedSignalEnabled;

	//Recipe Photo
	QLabel *photoLabel;
	QPushButton *changePhotoButton;

	//Recipe Body
	KTextEdit* instructionsEdit;
	KLineEdit* titleEdit;

	//Ingredient inputs
	QGroupBox *ingredientGBox;
	KDoubleNumInput* amountEdit;
	KComboBox* unitBox;
	KComboBox* ingredientBox;
	KListView* ingredientList;
	KIconLoader *il;

	// Buttons to move ingredients up & down...
	KPushButton* upButton;
	KPushButton* downButton;
	KPushButton* removeButton;
	KPushButton* addButton;

	// Internal functions
	void loadIngredientListCombo(void);
	void loadUnitListCombo(void);
	void saveRecipe(void);

	// Signals & Slots
	private slots:
	void reloadUnitsCombo(int);
	void changePhoto(void);
	void moveIngredientUp(void);
	void moveIngredientDown(void);
	void removeIngredient(void);
	void addIngredient(void);
	void recipeChanged(void);
	void recipeChanged(const QString &t);
	void enableChangedSignal(bool en=true);

	public slots:
	void save (void); // Activated when krecipes.cpp sends signal save()

	signals:
	void changed(void);
	void enableSaveOption(bool en=true);


};

//#include "recipeinputdialog.moc"

#endif
