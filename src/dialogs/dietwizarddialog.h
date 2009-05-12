/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DIETWIZARDDIALOG_H
#define DIETWIZARDDIALOG_H

#include <stdlib.h> // For RAND_MAX

#include <qcheckbox.h>

#include <qlabel.h>
#include <q3ptrlist.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <q3valuelist.h>

#include <QStackedWidget>
//Added by qt3to4:
#include <QPaintEvent>

#include <k3listview.h>
#include <kvbox.h>

#include "datablocks/constraintlist.h"
#include "datablocks/recipelist.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/recipe.h"
#include "datablocks/unitratiolist.h"

#include "widgets/dblistviewbase.h"

class KDoubleNumInput;
class QFrame;
class KPushButton;
class QGroupBox;
class KTabWidget;

class DishInput;
class DishTitle;
class MealInput;
class RecipeDB;
class RecipeList;
class CategoryCheckListView;
class PropertyConstraintListView;

/**
@author Unai Garro
*/


class DietWizardDialog: public KVBox
{

	Q_OBJECT

public:

	DietWizardDialog( QWidget *parent, RecipeDB* db );
	~DietWizardDialog();

private:
	//Private variables
	RecipeDB *database;

	int dayNumber;
	int mealNumber;

	RecipeList *dietRList;

	//Widgets
	KHBox *optionsBox;
	QGroupBox *mealsSliderBox;
	QLabel *mealNumberLabel;
	QSlider *mealNumberSelector;
	QGroupBox *daysSliderBox;
	QLabel *dayNumberLabel;
	QSlider *dayNumberSelector;
	KTabWidget *mealTabs;
	MealInput *mealTab; // points to the current tab
	KPushButton *okButton;

	//Methods
	bool checkCategories( Recipe &rec, int meal, int dish );
	bool checkConstraints( Recipe &rec, int meal, int dish );
	bool checkLimits( IngredientPropertyList &properties, ConstraintList &constraints );
	void loadConstraints( int meal, int dish, ConstraintList *constraints ) const;
	void loadEnabledCategories( int meal, int dish, ElementList *categories );
	void newTab( const QString &name );
	bool categoryFiltering( int meal, int dish ) const;
	int getNecessaryFlags() const;

public:
	//Methods
	void reload( ReloadFlags flags = Load );
	RecipeList& dietList( void );

private slots:
	void changeDayNumber( int dn );
	void changeMealNumber( int mn );
	void createDiet( void );
	void clear();
	void createShoppingList( void );
	void populateIteratorList( RecipeList &rl, Q3ValueList <RecipeList::Iterator> *il );
signals:
	void dietReady( void );
};

class MealInput: public QWidget
{
	Q_OBJECT

public:
	// Methods

	MealInput( QWidget *parent, RecipeDB *database );
	~MealInput();
	void reload( ReloadFlags flag = Load );
	int dishNo( void )
	{
		return dishNumber;
	};
	void setDishNo( int dn );
	void showDish( int dn );

	// Public widgets and variables
	Q3ValueList <DishInput*> dishInputList; // The list of dishes

private:
	// Widgets
	// Private Variables
	int dishNumber;
	RecipeDB *database;

	// Settings section for the meal
	KHBox *mealOptions;

	// Dish number setting
	KHBox *dishNumberBox;
	QLabel *dishNumberLabel;
	QSpinBox *dishNumberInput;

	// Move <-> buttons
	QFrame *toolBar;
	QToolButton *buttonNext;
	QToolButton *buttonPrev;

	// Settings for the dish
        QStackedWidget *dishStack;

public slots:
	void nextDish( void );
	void prevDish( void );

private slots:
	void changeDishNumber( int dn );

};

class DishInput: public QWidget
{
	Q_OBJECT

public:
	DishInput( QWidget *parent, RecipeDB *database, const QString &title );
	~DishInput();
	// Methods
	bool isCategoryFilteringEnabled( void ) const;
	void loadConstraints( ConstraintList *constraints ) const;
	void loadEnabledCategories( ElementList* categories );
	void reload( ReloadFlags flag = Load );
	void setDishTitle( const QString & text );
	void clear();


private:
	// Variables
	bool categoryFiltering;
	// Widgets
	QGroupBox *listBox;
	DishTitle *dishTitle;
	KVBox *categoriesBox;
	QCheckBox *categoriesEnabledBox;
	CategoryCheckListView *categoriesView;
	PropertyConstraintListView *constraintsView;
	KDoubleNumInput *constraintsEditBox1;
	KDoubleNumInput *constraintsEditBox2;
	RecipeDB *database;

private slots:
	void enableCategories( bool enable );
	void insertConstraintsEditBoxes( Q3ListViewItem* it );
	void hideConstraintInputs();
	void setMinValue( double minValue );
	void setMaxValue( double maxValue );
};

class DishTitle: public QWidget
{

	Q_OBJECT

public:
	DishTitle( QWidget *parent, const QString &title );
	~DishTitle();
	virtual QSize sizeHint () const;
	virtual QSize minimumSizeHint() const;
protected:
	//Variables
	QString titleText;
	//Methods
	virtual void paintEvent( QPaintEvent *p );
};

#endif
