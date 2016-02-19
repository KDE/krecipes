/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef MEALINPUT_H
#define MEALINPUT_H


#include "widgets/dblistviewbase.h"

#include <q3valuelist.h>

class DishInput;
class DishTitle;
class MealInput;
class RecipeDB;

class KDoubleNumInput;
class KPushButton;
class KTabWidget;
class KHBox;

class QSpinBox;
class QLabel;
class QStackedWidget;


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
	KPushButton *buttonNext;
	KPushButton *buttonPrev;

	// Settings for the dish
	QStackedWidget *dishStack;

public slots:
	void nextDish( void );
	void prevDish( void );

private slots:
	void changeDishNumber( int dn );

};

#endif
