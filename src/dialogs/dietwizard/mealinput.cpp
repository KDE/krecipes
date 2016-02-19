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

#include "mealinput.h"

#include "dishinput.h"
#include "dishtitle.h"

#include "backends/recipedb.h"

#include <KLocale>
#include <KNumInput>
#include <KHBox>
#include <KPushButton>
#include <KTabWidget>

#include <QStackedWidget>
#include <Q3ValueList>
#include <QLabel>
#include <QVBoxLayout>


MealInput::MealInput( QWidget *parent, RecipeDB *db ) : QWidget( parent ),
		database( db )
{
	// Design the dialog
	QVBoxLayout *layout = new QVBoxLayout( this );

	// Options box

	mealOptions = new KHBox( this );
	mealOptions->setSpacing( 10 );
	layout->addWidget( mealOptions );

	// Number of dishes input
	dishNumberBox = new KHBox( mealOptions );
	dishNumberBox->setSpacing( 10 );
	dishNumberLabel = new QLabel( i18nc( "@label:spinbox", "No. of dishes: " ), dishNumberBox );
	dishNumberInput = new QSpinBox( dishNumberBox );
	dishNumberInput->setMinimum( 1 );
	dishNumberInput->setMaximum( 10 );
	dishNumberBox->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );

	// Next dish/ Previous dish buttons
	buttonPrev = new KPushButton( mealOptions );
	buttonPrev->setText( i18nc( "@action:button", "Previous Dish" ) );
	buttonPrev->setIcon( KIcon( "go-previous" ) );
	buttonNext = new KPushButton( mealOptions );
	buttonNext->setText( i18nc( "@action:button", "Next Dish" ) );
	buttonNext->setIcon( KIcon( "go-next" ) );

	// Dish widgets
	dishStack = new  QStackedWidget( this );
	layout->addWidget( dishStack );
	dishStack->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	// Add default dishes
	DishInput *newDish = new DishInput( this, database, i18n( "1st Course" ) );
	dishStack->addWidget( newDish );
	dishInputList.append( newDish );
	newDish = new DishInput( this, database, i18n( "2nd Course" ) );
	dishStack->addWidget( newDish );
	dishInputList.append( newDish );
	newDish = new DishInput( this, database, i18n( "Dessert" ) );
	dishStack->addWidget( newDish );
	dishInputList.append( newDish );
	dishNumber = 3;
	dishNumberInput->setValue( dishNumber );

	// Signals & Slots
	connect( dishNumberInput, SIGNAL( valueChanged( int ) ), this, SLOT( changeDishNumber( int ) ) );
	connect( buttonPrev, SIGNAL( clicked() ), this, SLOT( prevDish() ) );
	connect( buttonNext, SIGNAL( clicked() ), this, SLOT( nextDish() ) );

}

MealInput::~MealInput()
{}

void MealInput::reload( ReloadFlags flag )
{
	Q3ValueList<DishInput*>::iterator it;
	for ( it = dishInputList.begin(); it != dishInputList.end(); ++it ) {
		DishInput *di = ( *it );
		di->reload(flag);
	}
}

void MealInput::setDishNo( int dn )
{
	dishNumberInput->setValue( dn );
}

void MealInput::changeDishNumber( int dn )
{
	if ( dn > dishNumber ) {
		while ( dishNumber != dn ) {
			DishInput * newDish = new DishInput( this, database,
				i18nc("Dish number %1", "Dish %1", dishNumber + 1 ) );
			newDish->reload();
			dishStack->addWidget( newDish );
			dishInputList.append( newDish );
			dishStack->setCurrentWidget( newDish );
			dishNumber++;
		}
	}
	else if ( dn < dishNumber ) {
		Q3ValueList <DishInput*>::Iterator it;
		while ( dishNumber != dn ) {
			it = dishInputList.fromLast();
			DishInput *lastDish = ( *it );
			dishInputList.remove( it );

			if ( ( *it ) == ( DishInput* ) dishStack->currentWidget() ) {
				it--;
				dishStack->setCurrentWidget( *it );
			}
			delete lastDish;
			dishNumber--;
		}
	}
}


void MealInput::nextDish( void )
{
	// First get the place of the current dish input in the list
	Q3ValueList <DishInput*>::iterator it = dishInputList.find( ( DishInput* ) ( dishStack->currentWidget() ) );

	//Show the next dish if it exists
	it++;
	if ( it != dishInputList.end() ) {
		dishStack->setCurrentWidget( *it );
	}

}

void MealInput::prevDish( void )
{
	// First get the place of the current dish input in the list
	Q3ValueList <DishInput*>::iterator it = dishInputList.find( ( DishInput* ) ( dishStack->currentWidget() ) );

	//Show the previous dish if it exists
	it--;
	if ( it != dishInputList.end() ) {
		dishStack->setCurrentWidget( *it );
	}
}

void MealInput::showDish( int dn )
{
	Q3ValueList <DishInput*>::iterator it = dishInputList.at( dn );
	if ( it != dishInputList.end() )
		dishStack->setCurrentWidget( *it );
}

