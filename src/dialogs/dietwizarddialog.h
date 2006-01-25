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
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qwidgetstack.h>

#include <klistview.h>

#include "datablocks/constraintlist.h"
#include "datablocks/recipelist.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/recipe.h"
#include "datablocks/unitratiolist.h"

class EditBox;
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


class DietWizardDialog: public QVBox
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

	// Cache data blocks
	UnitRatioList cachedUnitRatios;
	IngredientPropertyList cachedIngredientProperties;

	//Widgets
	QHBox *optionsBox;
	QVGroupBox *mealsSliderBox;
	QLabel *mealNumberLabel;
	QSlider *mealNumberSelector;
	QVGroupBox *daysSliderBox;
	QLabel *dayNumberLabel;
	QSlider *dayNumberSelector;
	QTabWidget *mealTabs;
	MealInput *mealTab; // points to the current tab
	QPushButton *okButton;

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
	void reload( void );
	RecipeList& dietList( void );

private slots:
	void changeDayNumber( int dn );
	void changeMealNumber( int mn );
	void createDiet( void );
	void clear();
	void createShoppingList( void );
	void populateIteratorList( RecipeList &rl, QValueList <RecipeList::Iterator> *il );
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
	void reload();
	int dishNo( void )
	{
		return dishNumber;
	};
	void setDishNo( int dn );
	void showDish( int dn );

	// Public widgets and variables
	QValueList <DishInput*> dishInputList; // The list of dishes

private:
	// Widgets
	// Private Variables
	int dishNumber;
	RecipeDB *database;

	// Settings section for the meal
	QHBox *mealOptions;

	// Dish number setting
	QHBox *dishNumberBox;
	QLabel *dishNumberLabel;
	QSpinBox *dishNumberInput;

	// Move <-> buttons
	QHGroupBox *toolBar;
	QToolButton *buttonNext;
	QToolButton *buttonPrev;

	// Settings for the dish
	QWidgetStack *dishStack;

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
	void reload();
	void setDishTitle( const QString & text );
	void clear();


private:
	// Variables
	bool categoryFiltering;
	// Widgets
	QHGroupBox *listBox;
	DishTitle *dishTitle;
	QVBox *categoriesBox;
	QCheckBox *categoriesEnabledBox;
	CategoryCheckListView *categoriesView;
	PropertyConstraintListView *constraintsView;
	EditBox *constraintsEditBox1;
	EditBox *constraintsEditBox2;
	RecipeDB *database;

private slots:
	void enableCategories( bool enable );
	void insertConstraintsEditBoxes( QListViewItem* it );
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
