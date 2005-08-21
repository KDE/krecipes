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

#include "ingredientsdialog.h"
#include "backends/recipedb.h"
#include "createelementdialog.h"
#include "widgets/editbox.h"
#include "datablocks/ingredientpropertylist.h"
#include "unitsdialog.h"
#include "usdadatadialog.h"
#include "selectpropertydialog.h"
#include "selectunitdialog.h"
#include "dependanciesdialog.h"
#include "widgets/ingredientlistview.h"

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include <qheader.h>
#include <qmessagebox.h>

IngredientsDialog::IngredientsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Initialize internal variables
	propertiesList = new IngredientPropertyList;
	perUnitListBack = new ElementList;

	// Design dialog

	layout = new QGridLayout( this, 1, 1, 0, 0 );
	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );

	ingredientListView = new KreListView ( this, i18n( "Ingredient list" ), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView( ingredientListView, database, true );
	list_view->reload();
	ingredientListView->setListView( list_view );
	layout->addMultiCellWidget ( ingredientListView, 1, 10, 1, 1 );
	ingredientListView->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding ) );

	QSpacerItem* spacer_rightIngredients = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_rightIngredients, 1, 2 );


	addIngredientButton = new QPushButton( this );
	addIngredientButton->setText( "+" );
	layout->addWidget( addIngredientButton, 1, 3 );
	addIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	addIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	addIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addIngredientButton->setFlat( true );

	removeIngredientButton = new QPushButton( this );
	removeIngredientButton->setText( "-" );
	layout->addWidget( removeIngredientButton, 3, 3 );
	removeIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	removeIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	removeIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeIngredientButton->setFlat( true );

	QSpacerItem* spacer_Ing_Buttons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_Ing_Buttons, 2, 3 );


	QSpacerItem* spacer_Ing_Units = new QSpacerItem( 30, 5, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_Ing_Units, 1, 4 );


	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );


	unitsListView = new KreListView ( this, i18n( "Unit list" ) );
	unitsListView->listView() ->addColumn( i18n( "Units" ) );
	unitsListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
	unitsListView->listView() ->setSorting( 0 );
	unitsListView->listView() ->setAllColumnsShowFocus( true );
	layout->addMultiCellWidget ( unitsListView, 1, 4, 5, 5 );
	unitsListView->listView() ->setMinimumWidth( 150 );
	unitsListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QSpacerItem* spacer_rightUnits = new QSpacerItem( 5, 5, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_rightUnits, 1, 6 );

	addUnitButton = new QPushButton( this );
	addUnitButton->setText( "+" );
	layout->addWidget( addUnitButton, 1, 7 );
	addUnitButton->resize( QSize( 30, 30 ) );
	addUnitButton->setMinimumSize( QSize( 30, 30 ) );
	addUnitButton->setMaximumSize( QSize( 30, 30 ) );
	addUnitButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addUnitButton->setFlat( true );

	removeUnitButton = new QPushButton( this );
	removeUnitButton->setText( "-" );
	layout->addWidget( removeUnitButton, 3, 7 );
	removeUnitButton->resize( QSize( 30, 30 ) );
	removeUnitButton->setMinimumSize( QSize( 30, 30 ) );
	removeUnitButton->setMaximumSize( QSize( 30, 30 ) );
	removeUnitButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeUnitButton->setFlat( true );
	QSpacerItem* spacer_Units_Properties = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_Units_Properties, 5, 5 );


	propertiesListView = new KreListView ( this, i18n( "Ingredient Properties" ) );
	layout->addMultiCellWidget ( propertiesListView, 6, 9, 5, 5 );

	propertiesListView->listView() ->addColumn( i18n( "Property" ) );
	propertiesListView->listView() ->addColumn( i18n( "Amount" ) );
	propertiesListView->listView() ->addColumn( i18n( "Units" ) );
	propertiesListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
	propertiesListView->listView() ->setAllColumnsShowFocus( true );
	propertiesListView->listView() ->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	propertiesListView->listView() ->setSorting( -1 ); // Disable sorting. For the moment, the order is important to identify the per_units ID corresponding to this row. So the user shouldn't change this order.

	addPropertyButton = new QPushButton( this );
	addPropertyButton->setText( "+" );
	layout->addWidget( addPropertyButton, 6, 7 );
	addPropertyButton->resize( QSize( 30, 30 ) );
	addPropertyButton->setMinimumSize( QSize( 30, 30 ) );
	addPropertyButton->setMaximumSize( QSize( 30, 30 ) );
	addPropertyButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addPropertyButton->setFlat( true );

	removePropertyButton = new QPushButton( this );
	removePropertyButton->setText( "-" );
	layout->addWidget( removePropertyButton, 8, 7 );
	removePropertyButton->resize( QSize( 30, 30 ) );
	removePropertyButton->setMinimumSize( QSize( 30, 30 ) );
	removePropertyButton->setMaximumSize( QSize( 30, 30 ) );
	removePropertyButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removePropertyButton->setFlat( true );

	QPushButton *loadUsdaButton = new QPushButton( this );
	loadUsdaButton->setText( i18n( "Load USDA data" ) );
	layout->addMultiCellWidget( loadUsdaButton, 10, 10, 5, 6 );
	loadUsdaButton->setFlat( true );

	QSpacerItem* spacer_Prop_Buttons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_Prop_Buttons, 7, 7 );

	inputBox = new EditBox( propertiesListView->listView() ->viewport() );
	propertiesListView->listView() ->addChild( inputBox );
	inputBox->hide();

	// Initialize
	unitList = new UnitList;

	// Signals & Slots
	connect( ingredientListView->listView(), SIGNAL( selectionChanged() ), this, SLOT( updateLists() ) );
	connect( addIngredientButton, SIGNAL( clicked() ), list_view, SLOT( createNew() ) );
	connect( addUnitButton, SIGNAL( clicked() ), this, SLOT( addUnitToIngredient() ) );
	connect( removeUnitButton, SIGNAL( clicked() ), this, SLOT( removeUnitFromIngredient() ) );
	connect( removeIngredientButton, SIGNAL( clicked() ), list_view, SLOT( remove
		         () ) );
	connect( addPropertyButton, SIGNAL( clicked() ), this, SLOT( addPropertyToIngredient() ) );
	connect( removePropertyButton, SIGNAL( clicked() ), this, SLOT( removePropertyFromIngredient() ) );
	connect( propertiesListView->listView(), SIGNAL( executed( QListViewItem* ) ), this, SLOT( insertPropertyEditBox( QListViewItem* ) ) );
	connect( propertiesListView->listView(), SIGNAL( selectionChanged() ), inputBox, SLOT( hide() ) );
	connect( inputBox, SIGNAL( valueChanged( double ) ), this, SLOT( setPropertyAmount( double ) ) );
	connect( loadUsdaButton, SIGNAL( clicked() ), this, SLOT( openUSDADialog() ) );
}


IngredientsDialog::~IngredientsDialog()
{
	delete unitList;
	delete perUnitListBack;
	delete propertiesList;
}

void IngredientsDialog::reloadIngredientList( void )
{
	( ( StdIngredientListView* ) ingredientListView->listView() ) ->reload();

	// Reload Unit List
	updateLists();

}

void IngredientsDialog::reloadUnitList()
{

	int ingredientID = -1;
	// Find selected ingredient
	QListViewItem *it;
	it = ingredientListView->listView() ->selectedItem();

	if ( it ) {  // Check if an ingredient is selected first
		ingredientID = it->text( 1 ).toInt();
	}


	unitList->clear();
	unitsListView->listView() ->clear();

	if ( ingredientID >= 0 ) {
		database->loadPossibleUnits( ingredientID, unitList );

		//Populate this data into the KListView

		for ( UnitList::const_iterator unit_it = unitList->begin(); unit_it != unitList->end(); ++unit_it ) {
			if ( !( *unit_it ).name.isEmpty() ) {
				( void ) new QListViewItem( unitsListView->listView(), ( *unit_it ).name, QString::number( ( *unit_it ).id ) );
			}
		}

		// Select the first unit
		unitsListView->listView() ->setSelected( unitsListView->listView() ->firstChild(), true );

	}
}

void IngredientsDialog::addUnitToIngredient( void )
{

	// Find selected ingredient item
	QListViewItem * it;
	int ingredientID = -1;
	if ( ( it = ingredientListView->listView() ->selectedItem() ) ) {
		ingredientID = it->text( 1 ).toInt();
	}
	if ( ingredientID >= 0 )  // an ingredient was selected previously
	{
		UnitList allUnits;
		database->loadUnits( &allUnits );

		SelectUnitDialog* unitsDialog = new SelectUnitDialog( 0, allUnits );

		if ( unitsDialog->exec() == QDialog::Accepted )
		{
			int unitID = unitsDialog->unitID();

			if ( !( database->ingredientContainsUnit( ingredientID, unitID ) ) )
				database->addUnitToIngredient( ingredientID, unitID ); // Add chosen unit to ingredient in database
			else {
				QMessageBox::information( this, i18n( "Unit Exists" ), i18n( "The ingredient contains already the unit that you have chosen." ) );
			}
			reloadUnitList(); // Reload the list from database
		}
	}
}

void IngredientsDialog::removeUnitFromIngredient( void )
{

	// Find selected ingredient/unit item combination
	QListViewItem * it;
	int ingredientID = -1, unitID = -1;
	if ( ( it = ingredientListView->listView() ->selectedItem() ) )
		ingredientID = it->text( 1 ).toInt();
	if ( ( it = unitsListView->listView() ->selectedItem() ) )
		unitID = it->text( 1 ).toInt();

	if ( ( ingredientID >= 0 ) && ( unitID >= 0 ) )  // an ingredient/unit combination was selected previously
	{
		ElementList dependingRecipes, dependingPropertiesInfo;

		database->findIngredientUnitDependancies( ingredientID, unitID, &dependingRecipes, &dependingPropertiesInfo );
		if ( dependingRecipes.isEmpty() && dependingPropertiesInfo.isEmpty() )
			database->removeUnitFromIngredient( ingredientID, unitID );
		else
		{ // must warn!
			DependanciesDialog *warnDialog = new DependanciesDialog( 0, &dependingRecipes, 0, &dependingPropertiesInfo );

			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeUnitFromIngredient( ingredientID, unitID );
			delete warnDialog;
		}
		reloadUnitList(); // Reload the list from database
		reloadPropertyList(); // Properties could have been removed if a unit is removed, so we need to reload.
	}
}

void IngredientsDialog:: reloadPropertyList( void )
{
	propertiesList->clear();
	propertiesListView->listView() ->clear();
	perUnitListBack->clear();

	inputBox->hide();


	//If none is selected, select first item
	QListViewItem *it;
	it = ingredientListView->listView() ->selectedItem();

	//Populate this data into the KListView
	if ( it ) { // make sure that the ingredient list is not empty

		database->loadProperties( propertiesList, it->text( 1 ).toInt() ); // load the list for this ingredient
		for ( IngredientProperty * prop = propertiesList->getFirst(); prop; prop = propertiesList->getNext() ) {
			QListViewItem * lastElement = propertiesListView->listView() ->lastItem();
			//Insert property after the last one (it's important to keep the order in the case of the properties to be able to identify the per_units ID later on).
			( void ) new QListViewItem( propertiesListView->listView(), lastElement, prop->name, QString::number( prop->amount ), prop->units + QString( "/" ) + prop->perUnit.name, QString::number( prop->id ) );
			// Store the perUnits with the ID for using later
			Element perUnitEl;
			perUnitEl.id = prop->perUnit.id;
			perUnitEl.name = prop->perUnit.name;
			perUnitListBack->append( perUnitEl );

		}
	}
}

void IngredientsDialog:: updateLists( void )
{
	reloadUnitList();
	reloadPropertyList();
}

void IngredientsDialog::addPropertyToIngredient( void )
{

	// Find selected ingredient item
	QListViewItem * it;
	int ingredientID = -1;
	if ( ( it = ingredientListView->listView() ->selectedItem() ) ) {
		ingredientID = it->text( 1 ).toInt();
	}
	if ( ingredientID >= 0 )  // an ingredient was selected previously
	{
		IngredientPropertyList allProperties;
		database->loadProperties( &allProperties );
		UnitList unitList;
		database->loadPossibleUnits( ingredientID, &unitList );
		SelectPropertyDialog* propertyDialog = new SelectPropertyDialog( 0, &allProperties, &unitList );

		if ( propertyDialog->exec() == QDialog::Accepted )
		{

			int propertyID = propertyDialog->propertyID();
			int perUnitsID = propertyDialog->perUnitsID();
			if ( !( database->ingredientContainsProperty( ingredientID, propertyID, perUnitsID ) ) ) {
				if ( ( propertyID >= 0 ) && ( perUnitsID >= 0 ) )  // check if the property is not -1 ... (not selected)
					database->addPropertyToIngredient( ingredientID, propertyID, 0, perUnitsID ); // Add result chosen property to ingredient in database, with amount 0 by default
			}
			else {
				QMessageBox::information( this, i18n( "Property Exists" ), i18n( "The property you tried to add already exists in the ingredient with the same per units." ) );
			}
			reloadPropertyList(); // Reload the list from database
		}
	}
}

void IngredientsDialog::removePropertyFromIngredient( void )
{

	// Find selected ingredient/property item combination
	QListViewItem * it;
	int ingredientID = -1, propertyID = -1;
	int perUnitsID = -1;
	if ( ( it = ingredientListView->listView() ->selectedItem() ) )
		ingredientID = it->text( 1 ).toInt();
	if ( ( it = propertiesListView->listView() ->selectedItem() ) )
		propertyID = it->text( 3 ).toInt();
	if ( propertyID >= 0 )
		perUnitsID = perUnitListBack->getElement( findPropertyNo( it ) ).id ;

	if ( ( ingredientID >= 0 ) && ( propertyID >= 0 ) && ( perUnitsID >= 0 ) )  // an ingredient/property combination was selected previously
	{
		ElementList results;
		database->removePropertyFromIngredient( ingredientID, propertyID, perUnitsID );

		reloadPropertyList(); // Reload the list from database

	}
}

void IngredientsDialog::insertPropertyEditBox( QListViewItem* it )
{

	QRect r = propertiesListView->listView() ->header() ->sectionRect( 1 );

	r.moveBy( 0, propertiesListView->listView() ->itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height

	inputBox->setGeometry( r );

	inputBox->setValue( it->text( 1 ).toDouble() );
	inputBox->show();
}

void IngredientsDialog::setPropertyAmount( double amount )
{

	inputBox->hide();


	QListViewItem *ing_it = ingredientListView->listView() ->selectedItem(); // Find selected ingredient
	QListViewItem *prop_it = propertiesListView->listView() ->selectedItem();

	if ( ing_it && prop_it ) // Appart from property, Check if an ingredient is selected first, just in case
	{
		prop_it->setText( 1, QString::number( amount ) );
		int propertyID = prop_it->text( 3 ).toInt();
		int ingredientID = ing_it->text( 1 ).toInt();
		int per_units = perUnitListBack->getElement( findPropertyNo( prop_it ) ).id ;
		database->changePropertyAmountToIngredient( ingredientID, propertyID, amount, per_units );
	}

	reloadPropertyList();

}

int IngredientsDialog::findPropertyNo( QListViewItem * /*it*/ )
{
	bool found = false;
	int i = 0;
	QListViewItem* item = propertiesListView->listView() ->firstChild();
	while ( i < propertiesListView->listView() ->childCount() && !found ) {
		if ( item == propertiesListView->listView() ->currentItem() )
			found = true;
		else {
			item = item->nextSibling();
			++i;
		}
	}
	if ( found ) {
		return ( i );
	}
	else {
		return ( -1 );
	}
}

void IngredientsDialog::reload( void )
{
	reloadIngredientList();
}

void IngredientsDialog::openUSDADialog( void )
{
	QListViewItem * ing_it = ingredientListView->listView() ->selectedItem(); // Find selected ingredient
	if ( ing_it ) {
		KApplication::setOverrideCursor( KCursor::waitCursor() );
		USDADataDialog usda_dialog( Element( ing_it->text( 0 ), ing_it->text( 1 ).toInt() ), database, this );
		KApplication::restoreOverrideCursor();

		if ( usda_dialog.exec() == QDialog::Accepted )
			reloadPropertyList(); //update property list upon success
	}
	else
		QMessageBox::information( this, QString::null, i18n( "No ingredient selected." ) );
}

#include "ingredientsdialog.moc"
