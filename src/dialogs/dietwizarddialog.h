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

#ifndef DIETWIZARDDIALOG_H
#define DIETWIZARDDIALOG_H

#include <klistview.h>

#include <qpushbutton.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qspinbox.h>
#include <qwidgetstack.h>

#include "datablocks/constraintlist.h"
#include "elementlist.h"
#include "ingredientpropertylist.h"
#include "recipe.h"

class RecipeDB;
class EditBox;
class MealInput;
class DishInput;
class DishTitle;

/**
@author Unai Garro
*/



class DietWizardDialog:public QVBox{

Q_OBJECT

public:

    DietWizardDialog(QWidget *parent, RecipeDB* db);
    ~DietWizardDialog();

private:
	//Private variables
	RecipeDB *database;
	int mealNumber;
	ElementList categoriesList;
	IngredientPropertyList propertyList;
	//Widgets
	QHBox *optionsBox;
	QVGroupBox *sliderBox;
	QLabel *mealNumberLabel;
	QSlider *mealNumberSelector;
	QTabWidget *mealTabs;
	MealInput *mealTab; // points to the current tab
	QHBox *buttonBox;
	QPushButton *okButton;

	//Methods
	bool checkCategories(Recipe &rec,int meal,int dish);
	bool checkConstraints(Recipe &rec,int meal,int dish);
	bool checkLimits(IngredientPropertyList &properties,ConstraintList &constraints);
	void loadConstraints(int meal,int dish,ConstraintList *constraints);
	void loadEnabledCategories(int meal,int dish,ElementList *categories);
	void newTab(const QString &name);

public:
	//Methods
	void reload(void);
private slots:
	void changeMealNumber(int mn);
	void createDiet(void);
};

class MealInput:public QWidget{
Q_OBJECT

public:
	// Methods

	 MealInput(QWidget *parent);
	 ~MealInput();
	 void reload(ElementList &categoriesList,IngredientPropertyList &propertyList);
	 int dishNo(void){return dishNumber;};

	 // Public widgets and variables
	 QValueList <DishInput*> dishInputList; // The list of dishes

private:
	// Widgets
		// Private Variables
		int dishNumber;
		ElementList categoriesListLocalCache;
		IngredientPropertyList propertyListLocalCache;

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
	// Methods
		void reload(); // Reloads again from the cached data
public slots:
	void nextDish(void);
	void prevDish(void);

private slots:
		void changeDishNumber(int dn);

};

class DishInput:public QWidget{
Q_OBJECT

public:
	DishInput(QWidget *parent,const QString &title);
	~DishInput();
	// Methods
	void loadConstraints(ConstraintList *constraints);
	void loadEnabledCategories(ElementList* categories);
	void reload(ElementList *categoryList, IngredientPropertyList *propertyList);
	void setDishTitle(const QString & text);


private:
		QHGroupBox *listBox;
		DishTitle *dishTitle;
		KListView *categoriesView;
		KListView *constraintsView;
		EditBox *constraintsEditBox1;
		EditBox *constraintsEditBox2;
private slots:
		void insertConstraintsEditBoxes(QListViewItem* it);
		void setMinValue(double minValue);
		void setMaxValue(double maxValue);
};

class DishTitle:public QWidget{

Q_OBJECT

public:
	DishTitle(QWidget *parent,const QString &title);
	~DishTitle();
	virtual QSize sizeHint () const;
	virtual QSize minimumSizeHint() const;
protected:
	//Variables
	QString titleText;
	//Methods
	virtual void paintEvent(QPaintEvent *p );
};

#endif
