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

#include "dietwizarddialog.h"

#include "mealinput.h"
#include "dishinput.h"
#include "dishtitle.h"

#include "backends/recipedb.h"
#include "dietviewdialog.h"

#include <q3header.h>
#include <QPainter>
#include <QPointer>
//Added by qt3to4:
#include <Q3ValueList>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QPaintEvent>

#include <kapplication.h>
#include <kcursor.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kdebug.h>
#include <krandom.h>
#include <kvbox.h>
#include <KPushButton>
#include <QGroupBox>
#include <KTabWidget>

#include "propertycalculator.h"
#include "widgets/propertylistview.h"
#include "widgets/categorylistview.h"

#include "profiling.h"

DietWizardDialog::DietWizardDialog( QWidget *parent, RecipeDB *db ) : KVBox( parent )
{
	// Initialize internal variables
	database = db;
	mealNumber = 1;
	dayNumber = 1;
	dietRList = new RecipeList();

	//Design the dialog
	setSpacing( 5 );
	// Options Box
	optionsBox = new KHBox( this );

	daysSliderBox = new QGroupBox( optionsBox );
	daysSliderBox->setTitle( i18nc( "@label:slider", "Number of Days" ) );
	QHBoxLayout *daysSliderBoxLayout = new QHBoxLayout;
	daysSliderBox->setLayout( daysSliderBoxLayout );
	dayNumberLabel = new QLabel;
	dayNumberLabel->setText( "- 1 -" );
	dayNumberLabel->setAlignment( Qt::AlignCenter );
	dayNumberSelector = new QSlider;

	daysSliderBoxLayout->addWidget( dayNumberSelector );
	daysSliderBoxLayout->addWidget( dayNumberLabel );

	dayNumberSelector->setOrientation( Qt::Horizontal );
	dayNumberSelector->setRange( 1, 10 );
	dayNumberSelector->setSingleStep( 1 );
	dayNumberSelector->setPageStep( 1 );
	dayNumberSelector->setTickPosition( QSlider::TicksBelow );
	dayNumberSelector->setFixedWidth( 100 );

	mealsSliderBox = new QGroupBox( optionsBox );
	mealsSliderBox->setTitle( i18nc( "@label:slider", "Meals per Day" ) );
	QHBoxLayout *mealsSliderBoxLayout = new QHBoxLayout;
	mealsSliderBox->setLayout( mealsSliderBoxLayout );
	mealNumberLabel = new QLabel;
	mealNumberLabel->setText( "- 1 -" );
	mealNumberLabel->setAlignment( Qt::AlignCenter );
	mealNumberSelector = new QSlider;

	mealsSliderBoxLayout->addWidget( mealNumberSelector );
	mealsSliderBoxLayout->addWidget( mealNumberLabel );

	mealNumberSelector->setOrientation( Qt::Horizontal );
	mealNumberSelector->setRange( 1, 10 );
	mealNumberSelector->setSingleStep( 1 );
	mealNumberSelector->setPageStep( 1 );
	mealNumberSelector->setTickPosition( QSlider::TicksBelow );
	mealNumberSelector->setFixedWidth( 100 );

	// Tabs
	mealTabs = new KTabWidget( this );
	mealTabs->setContentsMargins( 5 , 5, 5, 5 );

	// Button bar
	KHBox *bottom_layout = new KHBox( this );
	//bottom_layout->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	okButton = new KPushButton( bottom_layout );
	okButton->setIcon( KIcon( "dialog-ok" ) );
	okButton->setText( i18nc( "@action:button", "Create the diet" ) );

	KPushButton *clearButton = new KPushButton( bottom_layout );
	clearButton->setIcon( KIcon( "edit-clear" ) );
	clearButton->setText( i18nc( "@action:button Clear Diet","Clear" ) );

	// Create Tabs
	//don't use newTab, it'll load data and we don't want it to do that at startup
	mealTab = new MealInput( mealTabs, database );
	// keep this string the same as when creating the other tabs to ease translation
	mealTabs->addTab( mealTab, i18ncp( "@title:tab" , "Meal 1" , "Meal %1" , 1 ) );
	mealTabs->setCurrentIndex( mealTabs->indexOf( mealTab ) );

	// Signals & Slots
	connect( mealNumberSelector, SIGNAL(valueChanged(int)), this, SLOT(changeMealNumber(int)) );
	connect( dayNumberSelector, SIGNAL(valueChanged(int)), this, SLOT(changeDayNumber(int)) );
	connect( okButton, SIGNAL(clicked()), this, SLOT(createDiet()) );
	connect( clearButton, SIGNAL(clicked()), this, SLOT(clear()) );
}


DietWizardDialog::~DietWizardDialog()
{
	delete dietRList;
}

void DietWizardDialog::clear()
{
	mealNumberSelector->setValue( 1 );
	dayNumberSelector->setValue( 1 );

	MealInput* mealTab = ( MealInput* ) ( mealTabs->widget( 0 ) ); // Get the meal
	mealTab->setDishNo( 3 );
	mealTab->showDish( 0 );

	for ( int i = 0; i < mealTab->dishInputList.count(); ++i ) {
		DishInput* dishInput = mealTab->dishInputList[ i ]; // Get the dish input
		dishInput->clear();
	}
}

void DietWizardDialog::reload( ReloadFlags flag )
{
	for ( int i = 0; i < mealTabs->count(); ++i ) {
		MealInput *mealTab = (MealInput*)mealTabs->widget(i);
		mealTab->reload(flag);
	}
}

void DietWizardDialog::newTab( const QString &name )
{
	mealTab = new MealInput( mealTabs, database );
	mealTab->reload();
	mealTabs->addTab( mealTab, name );
	mealTabs->setCurrentIndex( mealTabs->indexOf( mealTab ) );
}

void DietWizardDialog::changeMealNumber( int mn )
{
	mealNumberLabel->setText( i18n( "- %1 -" , mn ) );
	if ( mn > mealNumber ) {

		while ( mealNumber != mn ) {
			mealNumber++;
			newTab( i18ncp( "@title:tab" , "Meal 1" , "Meal %1" , mealNumber ) );
		}
	}
	else if ( mn < mealNumber ) {

		while ( mealNumber != mn ) {
			mealNumber--;
			delete mealTabs->widget( mealTabs->count() - 1 );
		}
	}
}

void DietWizardDialog::changeDayNumber( int dn )
{

	if ( dn < 7 ) {
		dayNumber = dn;
		dayNumberLabel->setText( i18n( "- %1 -" , dn ) );
	}
	else {
		dayNumber = ( dn - 6 ) * 7;
		dayNumberLabel->setText( i18np( "- 1 week -", "- %1 weeks -" , dn - 6 ));
	}
}

void DietWizardDialog::createDiet( void )
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	START_TIMER("Creating the diet");

	RecipeList rlist;
	dietRList->clear();

	// Get the whole list of recipes, detailed
	int flags = RecipeDB::Title | getNecessaryFlags();
	database->loadRecipes( &rlist, flags );

	// temporal iterator list so elements can be removed without reloading them again from the DB
	// this list prevents the same meal from showing up in the same day twice
	Q3ValueList <RecipeList::Iterator> tempRList;

	bool alert = false;

	for ( int day = 0;day < dayNumber;day++ )  // Create the diet for the number of days defined by the user
	{
		populateIteratorList( rlist, &tempRList ); // temporal iterator list so elements can be removed without reloading them again from the DB
		for ( int meal = 0;meal < mealNumber;meal++ )
		{
			int dishNo = ( ( MealInput* ) ( mealTabs->widget( meal ) ) ) ->dishNo();

			for ( int dish = 0;dish < dishNo;dish++ ) {
				bool found = false;
				Q3ValueList <RecipeList::Iterator> tempDishRList = tempRList;
				while ( ( !found ) && !tempDishRList.empty() ) {
					int random_index = ( int ) ( ( float ) ( KRandom::random() ) / ( float ) RAND_MAX * tempDishRList.count() );
					Q3ValueList<RecipeList::Iterator>::Iterator iit = tempDishRList.at( random_index ); // note that at() retrieves an iterator to the iterator list, so we need to use * in order to get the RecipeList::Iterator

					RecipeList::Iterator rit = *iit;
					if ( found = ( ( ( !categoryFiltering( meal, dish ) ) || checkCategories( *rit, meal, dish ) ) && checkConstraints( *rit, meal, dish ) ) )  // Check that the recipe is inside the constraint limits and in the categories specified
					{
						dietRList->append( *rit ); // Add recipe to the diet list
						tempRList.remove( tempRList.find(*iit) ); //can't just remove()... the iterator isn't from this list (its an iterator from tempDishRList)
					}
					else {
						tempDishRList.remove( iit ); // Remove this analized recipe from teh list
					}
				}
				if ( !found )
					alert = true;
			}
		}
	}

	if ( alert ) {
		KApplication::restoreOverrideCursor();
		KMessageBox::sorry( this, i18nc( "@info", "Given the constraints, a full diet list could not be constructed. Either the recipe list is too short or the constraints are too demanding. " ) );
	}

	else // show the resulting diet
	{

		// make a list of dishnumbers
		QList<int> dishNumbers;

		for ( int meal = 0;meal < mealNumber;meal++ ) {
			int dishNo = ( ( MealInput* ) ( mealTabs->widget( meal ) ) ) ->dishNo();
			dishNumbers << dishNo;
		}

		KApplication::restoreOverrideCursor();

		// display the list
		QPointer<DietViewDialog> dietDisplay = new DietViewDialog( this, *dietRList, dayNumber, mealNumber, dishNumbers );
		connect( dietDisplay, SIGNAL(signalOk()), this, SLOT(createShoppingList()) );
		dietDisplay->exec();
		delete dietDisplay;
	}

	END_TIMER();
}


void DietWizardDialog::populateIteratorList( RecipeList &rl, Q3ValueList <RecipeList::Iterator> *il )
{
	il->clear();
	RecipeList::Iterator it;
	for ( it = rl.begin();it != rl.end(); ++it )
		il->append( it );

}

int DietWizardDialog::getNecessaryFlags() const
{
	bool need_ingredients = false;
	bool need_categories = false;
	for ( int meal = 0;meal < mealNumber;meal++ ) {
		int dishNo = ( ( MealInput* ) ( mealTabs->widget( meal ) ) ) ->dishNo();
		for ( int dish = 0;dish < dishNo;dish++ ) {
			if ( !need_categories ) {
				if ( categoryFiltering( meal, dish ) ) {
					need_categories = true;
				}
			}

			if ( !need_ingredients ) {
				ConstraintList constraints;
				loadConstraints( meal, dish, &constraints );
				for ( ConstraintList::const_iterator ct_it = constraints.constBegin(); ct_it != constraints.constEnd(); ++ct_it ) {
					if ( (*ct_it).enabled ) {
						need_ingredients = true;
						break;
					}
				}
			}

			if ( need_ingredients && need_categories )
				break;
		}

		if ( need_ingredients && need_categories )
			break;
	}

	kDebug()<<"Do we need to load ingredients: "<<need_ingredients;
	kDebug()<<"Do we need to load categories: "<<need_categories;

	int flags = 0;
	if ( need_ingredients ) flags |= RecipeDB::Ingredients;
	if ( need_categories ) flags |= RecipeDB::Categories;

	return flags;
}

bool DietWizardDialog::checkCategories( Recipe &rec, int meal, int dish )
{

	// Check if the recipe is among the categories chosen
	ElementList categoryList;
	loadEnabledCategories( meal, dish, &categoryList );


	for ( ElementList::const_iterator cat_it = rec.categoryList.constBegin(); cat_it != rec.categoryList.constEnd(); ++cat_it ) {
		if ( categoryList.containsId( ( *cat_it ).id ) )
			return true;
	}

	return false;
}

/*
** Calculate the recipe Properties and check if they're within the constraints
*/

bool DietWizardDialog::checkConstraints( Recipe &rec, int meal, int dish )
{
	// Check if the properties are within the constraints
	ConstraintList constraints;
	loadConstraints( meal, dish, &constraints ); //load the constraints

	bool any_enabled = false;
	for ( ConstraintList::const_iterator ct_it = constraints.constBegin(); ct_it != constraints.constEnd(); ++ct_it ) {
		if ( (*ct_it).enabled ) {
			any_enabled = true;
			break;
		}
	}
	if ( !any_enabled )
		return true;

	// Calculate properties of the recipe
	calculateProperties( rec, database );

	bool withinLimits = checkLimits( rec.properties, constraints );

	return ( withinLimits );
}

void DietWizardDialog::loadConstraints( int meal, int dish, ConstraintList *constraints ) const
{
	MealInput * mealTab = ( MealInput* ) ( mealTabs->widget( meal ) ); // Get the meal
	DishInput* dishInput = mealTab->dishInputList[ dish ]; // Get the dish input
	dishInput->loadConstraints( constraints ); //Load the constraints form the K3ListView
}

void DietWizardDialog::loadEnabledCategories( int meal, int dish, ElementList *categories )
{
	MealInput * mealTab = ( MealInput* ) ( mealTabs->widget( meal ) ); // Get the meal
	DishInput* dishInput = mealTab->dishInputList[ dish ]; // Get the dish input
	dishInput->loadEnabledCategories( categories ); //Load the categories that have been checked in the K3ListView
}

bool DietWizardDialog::categoryFiltering( int meal, int dish ) const
{
	MealInput * mealTab = ( MealInput* ) ( mealTabs->widget( meal ) ); // Get the meal
	DishInput* dishInput = mealTab->dishInputList[ dish ]; // Get the dish input
	return ( dishInput->isCategoryFilteringEnabled() ); //Load the categories that have been checked in the K3ListView
}

bool DietWizardDialog::checkLimits( IngredientPropertyList &properties, ConstraintList &constraints )
{
	for ( ConstraintList::const_iterator ct_it = constraints.constBegin(); ct_it != constraints.constEnd(); ++ct_it ) {
		if ( (*ct_it).enabled ) {
			IngredientPropertyList::iterator ip_it = properties.find( (*ct_it).id );
			if ( ip_it != properties.end() ) {
				if ( ( (*ip_it).amount > (*ct_it).max ) || ( (*ip_it).amount < (*ct_it).min ) )
					return false;
			}
			else
				return false;
		}
	}
	return true;
}

void DietWizardDialog::createShoppingList( void )
{
	emit dietReady();
}

RecipeList& DietWizardDialog::dietList( void )
{
	return *dietRList;
}

#include "dietwizarddialog.moc"
