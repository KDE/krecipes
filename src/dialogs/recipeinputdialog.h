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
#include <qmap.h>
#include <qobject.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qvbox.h>

#include "elementlist.h"

class QTabWidget;
class Recipe;
class ElementList;
class RecipeDB;
class FractionInput;
class QTimeEdit;

/**
@author Unai Garro
*/
class RecipeInputDialog: public QVBox{
Q_OBJECT

public:

    RecipeInputDialog( QWidget* parent, RecipeDB* db);
    void loadRecipe(int recipeID);
    ~RecipeInputDialog();
    void newRecipe(void);
    void reloadCombos(void);
    bool everythingSaved();
    void reload(void);
    int loadedRecipeID() const;
   
private:

	// Internal Data
	Recipe *loadedRecipe; //Loaded Recipe
	ElementList *ingredientComboList;
	ElementList *unitComboList;
	ElementList *prepMethodComboList;
	RecipeDB *database;
	bool changedSignalEnabled;
	bool unsavedChanges;

	// Widgets
	QTabWidget* tabWidget;
	QGroupBox* recipeTab;
	QGroupBox* instructionsTab;

	//Recipe Photo
	QLabel *photoLabel;
	QPixmap sourcePhoto;
	QPushButton *changePhotoButton;

	//Recipe Body
	KTextEdit* instructionsEdit;
	QLabel* titleLabel;
	KLineEdit* titleEdit;

	//Additional recipe data
	QLabel* servingsLabel;
	KIntNumInput* servingsNumInput;
	QTimeEdit *prepTimeEdit;
	QLabel* authorLabel;
	KLineEdit* authorShow;
	QPushButton* addAuthorButton;
	QLabel* categoryLabel;
	KLineEdit* categoryShow;
	QPushButton* addCategoryButton;

	//Ingredient inputs
	QGroupBox *ingredientGBox;
	QLabel *amountLabel;
	FractionInput* amountEdit;
	QLabel *unitLabel;
	KComboBox* unitBox;
	QLabel *prepMethodLabel;
	KComboBox* prepMethodBox;
	QLabel *ingredientLabel;
	KComboBox* ingredientBox;
	KListView* ingredientList;
	KIconLoader *il;

	// Buttons to move ingredients up & down...
	KPushButton* upButton;
	KPushButton* downButton;
	KPushButton* removeButton;
	KPushButton* addButton;

	//Function buttons
	QGroupBox* functionsBox;
	QToolButton* saveButton;
	QToolButton* closeButton;
	QToolButton* showButton;
	QToolButton* resizeButton;

	QToolButton* spellCheckButton;

	// Internal functions
	bool checkAmountEdit();
	void createNewIngredientIfNecessary();
	int createNewUnitIfNecessary( const QString &unit, const QString &ingredient );
	int createNewPrepIfNecessary( const QString &prep );
	void checkIfNewUnits();
	void findCategoriesInRecipe(const ElementList &categoryList, QMap<Element,bool> &selected);
	void loadIngredientListCombo(void);
	void loadUnitListCombo(void);
	void loadPrepMethodListCombo(void);
	void saveRecipe(void);
	void showCategories(void);
	void showAuthors(void);

	// Signals & Slots

	private slots:
	void reloadUnitsCombo(int);
	void changePhoto(void);
	void clearPhoto(void);
	void moveIngredientUp(void);
	void moveIngredientDown(void);
	void removeIngredient(void);
	void addIngredient(void);
	void syncListView( QListViewItem* it, const QString &new_text, int col );
	void recipeChanged(void);
	void recipeChanged(const QString &t);
	void enableChangedSignal(bool en=true);
	void addCategory(void);
	void addAuthor(void);
	void slotIngredientBoxLostFocus(void);
	void slotUnitBoxLostFocus(void);
	void slotPrepMethodBoxLostFocus(void);
	void enableSaveButton(bool enabled);
	void closeOptions(void);
	void showRecipe(void);

	public slots:
	void save (void); // Activated when krecipes.cpp sends signal save()
	void spellCheck(void);
	void resizeRecipe(void);

	signals:
	void changed(void);
	void closeRecipe(void);
	void createButton(QWidget* w,QString title);
	void enableSaveOption(bool en=true);
	void showRecipe(int recipeID); //Indicates krecipesview to show it
	void titleChanged(const QString &title);


};

//#include "recipeinputdialog.moc"

#endif
