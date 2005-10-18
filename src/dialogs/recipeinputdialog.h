/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
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

#include "datablocks/elementlist.h"
#include "datablocks/unit.h"

class QTabWidget;
class Recipe;
class ElementList;
class RecipeDB;
class FractionInput;
class QTimeEdit;
class QDragEvent;
class QButtonGroup;
class QWidgetStack;

class ImageDropLabel;
class KreTextEdit;
class IngredientComboBox;
class HeaderComboBox;
class PrepMethodComboBox;
class KWidgetListbox;
class Rating;
class RatingDisplayWidget;

/**
@author Unai Garro
*/
class RecipeInputDialog: public QVBox
{
	Q_OBJECT

public:

	RecipeInputDialog( QWidget* parent, RecipeDB* db );
	void loadRecipe( int recipeID );
	~RecipeInputDialog();
	void newRecipe( void );
	void reloadCombos( void );
	bool everythingSaved();
	void reload( void );
	int loadedRecipeID() const;

private:

	// Internal Data
	Recipe *loadedRecipe; //Loaded Recipe
	UnitList *unitComboList;
	RecipeDB *database;
	bool changedSignalEnabled;
	bool unsavedChanges;

	// Widgets
	QTabWidget* tabWidget;
	QGroupBox* recipeTab;
	QGroupBox* instructionsTab;

	//Recipe Photo
	ImageDropLabel *photoLabel;
	QPixmap sourcePhoto;
	QPushButton *changePhotoButton;

	//Recipe Body
	KreTextEdit* instructionsEdit;
	QLabel* titleLabel;
	KLineEdit* titleEdit;

	//Additional recipe data
	QLabel* yieldLabel;
	FractionInput* yieldNumInput;
	KLineEdit* yieldTypeEdit;
	QTimeEdit *prepTimeEdit;
	QLabel* authorLabel;
	KLineEdit* authorShow;
	QPushButton* addAuthorButton;
	QLabel* categoryLabel;
	KLineEdit* categoryShow;
	QPushButton* addCategoryButton;

	//Ingredient inputs
	QButtonGroup *typeButtonGrp;
	QGroupBox *ingredientGBox;
	QLabel *amountLabel;
	FractionInput* amountEdit;
	QLabel *unitLabel;
	KComboBox* unitBox;
	QLabel *prepMethodLabel;
	PrepMethodComboBox* prepMethodBox;
	QLabel *ingredientLabel;
	IngredientComboBox* ingredientBox;
	HeaderComboBox* headerBox;
	KListView* ingredientList;
	QWidgetStack *header_ing_stack;

	// Buttons to move ingredients up & down...
	KPushButton* upButton;
	KPushButton* downButton;
	KPushButton* removeButton;
	KPushButton* addButton;
	KPushButton* ingParserButton;

	//Function buttons
	QGroupBox* functionsBox;
	QToolButton* saveButton;
	QToolButton* closeButton;
	QToolButton* showButton;
	QToolButton* resizeButton;

	QToolButton* spellCheckButton;

	KWidgetListbox *ratingListDisplayWidget;

	// Internal functions
	bool checkAmountEdit();
	bool checkBounds();
	void createNewIngredientIfNecessary();
	int createNewUnitIfNecessary( const QString &unit, bool plural, const QString &ingredient, Unit &new_unit );
	QValueList<int> createNewPrepIfNecessary( const QString &prep );
	int createNewGroupIfNecessary( const QString &group );
	int createNewYieldIfNecessary( const QString &yield );
	void checkIfNewUnits();
	void saveRecipe( void );
	void showCategories( void );
	void showAuthors( void );
	int ingItemIndex( QListView *listview, const QListViewItem *item ) const;
	void addRating( const Rating &rating, RatingDisplayWidget *item );

	// Signals & Slots

private slots:
	void loadUnitListCombo( void );
	void changePhoto( void );
	void clearPhoto( void );
	void moveIngredientUp( void );
	void moveIngredientDown( void );
	void removeIngredient( void );
	void addIngredient( void );
	void syncListView( QListViewItem* it, const QString &new_text, int col );
	void recipeChanged( void );
	void recipeChanged( const QString &t );
	void enableChangedSignal( bool en = true );
	void addCategory( void );
	void addAuthor( void );
	void slotIngredientBoxLostFocus( void );
	void slotUnitBoxLostFocus( void );
	void slotPrepMethodBoxLostFocus( void );
	void enableSaveButton( bool enabled );
	void closeOptions( void );
	void showRecipe( void );
	void prepTitleChanged( const QString &title );
	void typeButtonClicked( int );
	void recipeRemoved( int id );
	void slotIngredientParser();
	void slotAddRating();
	void slotEditRating();
	void slotRemoveRating();

public slots:
	bool save ( void ); // Activated when krecipes.cpp sends signal save()
	void spellCheck( void );
	void resizeRecipe( void );

signals:
	void changed( void );
	void closeRecipe( void );
	void createButton( QWidget* w, const QString &title );
	void enableSaveOption( bool en = true );
	void showRecipe( int recipeID ); //Indicates krecipesview to show it
	void titleChanged( const QString &title );


};


class ImageDropLabel : public QLabel
{
	Q_OBJECT

public:
	ImageDropLabel( QWidget *parent, QPixmap &_sourcePhoto );

signals:
	void changed();

protected:
	void dragEnterEvent( QDragEnterEvent* event );
	void dropEvent( QDropEvent* event );

private:
	QPixmap &sourcePhoto;
};

#endif
