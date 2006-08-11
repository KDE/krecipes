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

#include "ingredientsdialog.h"
#include "backends/recipedb.h"
#include "createelementdialog.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/mixednumber.h"
#include "datablocks/weight.h"
#include "unitsdialog.h"
#include "usdadatadialog.h"
#include "selectpropertydialog.h"
#include "selectunitdialog.h"
#include "dependanciesdialog.h"
#include "widgets/ingredientlistview.h"
#include "widgets/weightinput.h"
#include "dialogs/ingredientgroupsdialog.h"
#include "dialogs/createingredientweightdialog.h"

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include <qheader.h>
#include <qmessagebox.h>
#include <qtabwidget.h>

class WeightListItem : public QListViewItem
{
public:
	WeightListItem( QListView *listview, QListViewItem *item, const Weight &w ) : QListViewItem(listview,item), m_weight(w){}

	void setWeight( const Weight &w ) { m_weight = w; }
	Weight weight() const { return m_weight; }

	void setAmountUnit( double amount, const Unit &unit, const Element &prepMethod )
	{
		m_weight.perAmount = amount;
		m_weight.perAmountUnitID = unit.id;
		m_weight.perAmountUnit = (m_weight.perAmount>1)?unit.plural:unit.name;
		m_weight.prepMethodID = prepMethod.id;
		m_weight.prepMethod = prepMethod.name;
	}

	void setWeightUnit( double weight, const Unit &unit )
	{
		m_weight.weight = weight;
		m_weight.weightUnitID = unit.id;
		m_weight.weightUnit = (m_weight.weight>1)?unit.plural:unit.name;
	}

	virtual QString text( int c ) const
	{
		if ( c == 0 )
			return QString::number(m_weight.weight)+" "+m_weight.weightUnit;
		else if ( c == 1 )
			return QString::number(m_weight.perAmount)+" "
			  +m_weight.perAmountUnit
			  +((m_weight.prepMethodID!=-1)?", "+m_weight.prepMethod:QString::null);
		else
			return QString::null;
	}

private:
	Weight m_weight;
};

IngredientsDialog::IngredientsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Initialize internal variables
	propertiesList = new IngredientPropertyList;
	perUnitListBack = new ElementList;

	// Design dialog

	QHBoxLayout* page_layout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	QTabWidget *tabWidget = new QTabWidget( this );

	QWidget *ingredientTab = new QWidget( tabWidget );

	layout = new QGridLayout( ingredientTab, 1, 1, 0, 0 );
	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1 );

	ingredientListView = new KreListView ( ingredientTab, i18n( "Ingredient list" ), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView( ingredientListView, database, true );
	ingredientListView->setListView( list_view );
	layout->addMultiCellWidget ( ingredientListView, 1, 5, 1, 1 );
	ingredientListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QSpacerItem* spacer_rightIngredients = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_rightIngredients, 1, 2 );


	addIngredientButton = new QPushButton( ingredientTab );
	addIngredientButton->setText( "+" );
	layout->addWidget( addIngredientButton, 1, 3 );
	addIngredientButton->setMinimumSize( QSize( 30, 30 ) );
	addIngredientButton->setMaximumSize( QSize( 30, 30 ) );
	addIngredientButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addIngredientButton->setFlat( true );

	removeIngredientButton = new QPushButton( ingredientTab );
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

	QScrollView *scrollView1 = new QScrollView( ingredientTab, "scrollView1" );
	scrollView1->enableClipper(true);
	QWidget *rightWidget = new QWidget(scrollView1);
	QGridLayout *rightLayout = new QGridLayout( rightWidget, 1, 1, 0, 0 );

	unitsListView = new KreListView ( rightWidget, i18n( "Unit list" ) );
	unitsListView->listView() ->addColumn( i18n( "Units" ) );
	unitsListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
	unitsListView->listView() ->setSorting( 0 );
	unitsListView->listView() ->setAllColumnsShowFocus( true );
	rightLayout->addMultiCellWidget ( unitsListView, 1, 4, 0, 0 );
	unitsListView->listView() ->setMinimumWidth( 150 );
	unitsListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QSpacerItem* spacer_rightUnits = new QSpacerItem( 5, 5, QSizePolicy::Fixed, QSizePolicy::Minimum );
	rightLayout->addItem( spacer_rightUnits, 1, 1 );

	addUnitButton = new QPushButton( rightWidget );
	addUnitButton->setText( "+" );
	rightLayout->addWidget( addUnitButton, 1, 2 );
	addUnitButton->resize( QSize( 30, 30 ) );
	addUnitButton->setMinimumSize( QSize( 30, 30 ) );
	addUnitButton->setMaximumSize( QSize( 30, 30 ) );
	addUnitButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addUnitButton->setFlat( true );

	removeUnitButton = new QPushButton( rightWidget );
	removeUnitButton->setText( "-" );
	rightLayout->addWidget( removeUnitButton, 3, 2 );
	removeUnitButton->resize( QSize( 30, 30 ) );
	removeUnitButton->setMinimumSize( QSize( 30, 30 ) );
	removeUnitButton->setMaximumSize( QSize( 30, 30 ) );
	removeUnitButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeUnitButton->setFlat( true );
	QSpacerItem* spacer_Units_Properties = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
 	rightLayout->addItem( spacer_Units_Properties, 2, 2 );


	propertiesListView = new KreListView ( rightWidget, i18n( "Ingredient Properties" ) );
	rightLayout->addMultiCellWidget ( propertiesListView, 6, 9, 0, 0 );

	propertiesListView->listView() ->addColumn( i18n( "Property" ) );
	propertiesListView->listView() ->addColumn( i18n( "Amount" ) );
	propertiesListView->listView() ->addColumn( i18n( "Units" ) );
	propertiesListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
	propertiesListView->listView() ->setAllColumnsShowFocus( true );
	propertiesListView->listView() ->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	propertiesListView->listView() ->setSorting( -1 ); // Disable sorting. For the moment, the order is important to identify the per_units ID corresponding to this row. So the user shouldn't change this order.

	addPropertyButton = new QPushButton( rightWidget );
	addPropertyButton->setText( "+" );
	rightLayout->addWidget( addPropertyButton, 6, 2 );
	addPropertyButton->resize( QSize( 30, 30 ) );
	addPropertyButton->setMinimumSize( QSize( 30, 30 ) );
	addPropertyButton->setMaximumSize( QSize( 30, 30 ) );
	addPropertyButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addPropertyButton->setFlat( true );

	removePropertyButton = new QPushButton( rightWidget );
	removePropertyButton->setText( "-" );
	rightLayout->addWidget( removePropertyButton, 8, 2 );
	removePropertyButton->resize( QSize( 30, 30 ) );
	removePropertyButton->setMinimumSize( QSize( 30, 30 ) );
	removePropertyButton->setMaximumSize( QSize( 30, 30 ) );
	removePropertyButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removePropertyButton->setFlat( true );

	QSpacerItem* spacer_Prop_Buttons = new QSpacerItem( 9, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	rightLayout->addItem( spacer_Prop_Buttons, 7, 2 );

	weightsListView = new KreListView ( rightWidget, i18n( "Ingredient Weights" ) );
	weightsListView->listView() ->addColumn( i18n( "Weight" ) );
	weightsListView->listView() ->addColumn( i18n( "Per Amount" ) );
	weightsListView->listView() ->setAllColumnsShowFocus( true );
	rightLayout->addMultiCellWidget ( weightsListView, 10, 14, 0, 0 );
	weightsListView->listView() ->setMinimumWidth( 150 );
	weightsListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	//QSpacerItem* spacer_rightWeights = new QSpacerItem( 5, 5, QSizePolicy::Fixed, QSizePolicy::Minimum );
	//layout->addItem( spacer_rightWeights, 1, 6 );

	addWeightButton = new QPushButton( rightWidget );
	addWeightButton->setText( "+" );
	rightLayout->addWidget( addWeightButton, 10, 2 );
	addWeightButton->resize( QSize( 30, 30 ) );
	addWeightButton->setMinimumSize( QSize( 30, 30 ) );
	addWeightButton->setMaximumSize( QSize( 30, 30 ) );
	addWeightButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	addWeightButton->setFlat( true );

	removeWeightButton = new QPushButton( rightWidget );
	removeWeightButton->setText( "-" );
	rightLayout->addWidget( removeWeightButton, 12, 2 );
	removeWeightButton->resize( QSize( 30, 30 ) );
	removeWeightButton->setMinimumSize( QSize( 30, 30 ) );
	removeWeightButton->setMaximumSize( QSize( 30, 30 ) );
	removeWeightButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	removeWeightButton->setFlat( true );

	QSpacerItem* spacer_Weight_Properties = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	rightLayout->addItem( spacer_Weight_Properties, 11, 2 );

	QSpacerItem* spacerBottom = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding );
	rightLayout->addItem( spacerBottom, 13, 2 );

	scrollView1->setResizePolicy( QScrollView::AutoOneFit );
	layout->addMultiCellWidget(scrollView1,1,4,5,5);

	QPushButton *loadUsdaButton = new QPushButton( ingredientTab );
	loadUsdaButton->setText( i18n( "Load USDA data" ) );
	loadUsdaButton->setFlat( true );
	layout->addWidget(loadUsdaButton,5,5);

	scrollView1->addChild(rightWidget);
	scrollView1->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Ignored );


	inputBox = new KDoubleNumInput( propertiesListView->listView() ->viewport() );
	propertiesListView->listView() ->addChild( inputBox );
	inputBox->hide();

	weightInputBox = new AmountUnitInput( weightsListView->listView()->viewport(), database, Unit::Mass, MixedNumber::DecimalFormat );
	weightsListView->listView()->addChild( weightInputBox );
	weightInputBox->hide();

	perAmountInputBox = new WeightInput( weightsListView->listView()->viewport(), database, Unit::All, MixedNumber::DecimalFormat );
	weightsListView->listView()->addChild( perAmountInputBox );
	perAmountInputBox->hide();

	weightsListView->listView()->setColumnWidth( 0, weightInputBox->width() );
	weightsListView->listView()->setColumnWidth( 1, perAmountInputBox->width() );

	tabWidget->insertTab( ingredientTab, i18n( "Ingredients" ) );

	groupsDialog = new IngredientGroupsDialog(database,tabWidget,"groupsDialog");
	tabWidget->insertTab( groupsDialog, i18n( "Headers" ) );

	page_layout->addWidget( tabWidget );

	// Initialize
	unitList = new UnitList;

	// Signals & Slots
	connect( ingredientListView->listView(), SIGNAL( selectionChanged() ), this, SLOT( updateLists() ) );
	connect( addIngredientButton, SIGNAL( clicked() ), list_view, SLOT( createNew() ) );
	connect( addUnitButton, SIGNAL( clicked() ), this, SLOT( addUnitToIngredient() ) );
	connect( removeUnitButton, SIGNAL( clicked() ), this, SLOT( removeUnitFromIngredient() ) );
	connect( addWeightButton, SIGNAL( clicked() ), this, SLOT( addWeight() ) );
	connect( removeWeightButton, SIGNAL( clicked() ), this, SLOT( removeWeight() ) );
	connect( removeIngredientButton, SIGNAL( clicked() ), list_view, SLOT( remove
		         () ) );
	connect( addPropertyButton, SIGNAL( clicked() ), this, SLOT( addPropertyToIngredient() ) );
	connect( removePropertyButton, SIGNAL( clicked() ), this, SLOT( removePropertyFromIngredient() ) );
	connect( propertiesListView->listView(), SIGNAL( executed( QListViewItem* ) ), this, SLOT( insertPropertyEditBox( QListViewItem* ) ) );
	connect( propertiesListView->listView(), SIGNAL( selectionChanged() ), inputBox, SLOT( hide() ) );
	connect( inputBox, SIGNAL( valueChanged( double ) ), this, SLOT( setPropertyAmount( double ) ) );

	connect( weightsListView->listView(), SIGNAL( selectionChanged() ), this, SLOT( setWeights() ) );
	connect( weightsListView->listView(), SIGNAL( doubleClicked( QListViewItem*, const QPoint &, int ) ), SLOT( itemRenamed( QListViewItem*, const QPoint &, int ) ) );

	connect( loadUsdaButton, SIGNAL( clicked() ), this, SLOT( openUSDADialog() ) );
}


IngredientsDialog::~IngredientsDialog()
{
	delete unitList;
	delete perUnitListBack;
	delete propertiesList;
}

void IngredientsDialog::reloadIngredientList( ReloadFlags flag )
{
	( ( StdIngredientListView* ) ingredientListView->listView() ) ->reload(flag);

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

void IngredientsDialog::addWeight()
{
	QListViewItem *it = ingredientListView->listView()->selectedItem();
	if ( it ) {
		CreateIngredientWeightDialog weightDialog( this, database );
		if ( weightDialog.exec() == QDialog::Accepted ) {
			Weight w = weightDialog.weight();
			w.ingredientID = it->text( 1 ).toInt();
			database->addIngredientWeight( w );
	
			QListViewItem * lastElement = weightsListView->listView()->lastItem();
	
			WeightListItem *weight_it = new WeightListItem( weightsListView->listView(), lastElement, w );
			weight_it->setAmountUnit( w.perAmount, database->unitName(w.perAmountUnitID),
			  Element(w.prepMethod,w.prepMethodID)
			);
			weight_it->setWeightUnit( w.weight, database->unitName(w.weightUnitID) );
		}
	}
}

void IngredientsDialog::removeWeight()
{
	QListViewItem *it = weightsListView->listView() ->selectedItem();
	if ( it ) {
		database->removeIngredientWeight( ((WeightListItem*)it)->weight().id );
		delete it;
	}
}

void IngredientsDialog::setWeights()
{
	Weight w;

	if ( weightInputBox->isShown() ) {
		WeightListItem *it = (WeightListItem*)weightInputBox->item();
		it->setWeightUnit( weightInputBox->amount().toDouble(), weightInputBox->unit() );
		w = it->weight();

		weightInputBox->setShown(false);
	}

	if ( perAmountInputBox->isShown() ) {
		WeightListItem *it = (WeightListItem*)perAmountInputBox->item();
		it->setAmountUnit( perAmountInputBox->amount().toDouble(), perAmountInputBox->unit(), perAmountInputBox->prepMethod() );
		w = it->weight();

		perAmountInputBox->setShown(false);
	}

	if ( w.id != -1 )
		database->addIngredientWeight( w );
}

void IngredientsDialog::itemRenamed( QListViewItem* item, const QPoint &, int col )
{
	WeightListItem *weight_it = (WeightListItem*)item;
	Weight w = weight_it->weight();

	if ( col == 0 ) {
		insertIntoListView(item,0,weightInputBox);
		weightInputBox->setAmount( w.weight );
		Unit u;
		u.id = w.weightUnitID;
		weightInputBox->setUnit( u );
	}
	else if ( col == 1 ) {
		insertIntoListView(item,1,perAmountInputBox);

		perAmountInputBox->setAmount( w.perAmount );

		Unit u;
		u.id = w.perAmountUnitID;
		perAmountInputBox->setUnit( u );

		perAmountInputBox->setPrepMethod( Element(w.prepMethod,w.prepMethodID) );
	}
}

void IngredientsDialog::insertIntoListView( QListViewItem *it, int col, AmountUnitInput *amountEdit )
{
	amountEdit->setItem( it );

	if ( !it ) {
		amountEdit->setShown(false);
		return;
	}

	QRect r;

	r = it->listView()->header() ->sectionRect( col ); //start at the section 2 header
	r.moveBy( 0, it->listView()->itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height
	r.setWidth( it->listView()->header() ->sectionRect( col ).width() ); // and width
	amountEdit->setGeometry( r );

	amountEdit->setShown(true);
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
			DependanciesDialog *warnDialog = new DependanciesDialog( 0, &dependingRecipes, &dependingPropertiesInfo );

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
		for ( IngredientPropertyList::const_iterator prop_it = propertiesList->begin(); prop_it != propertiesList->end(); ++prop_it ) {
			QListViewItem * lastElement = propertiesListView->listView() ->lastItem();
			//Insert property after the last one (it's important to keep the order in the case of the properties to be able to identify the per_units ID later on).
			( void ) new QListViewItem( propertiesListView->listView(), lastElement, (*prop_it).name, QString::number( (*prop_it).amount ), (*prop_it).units + QString( "/" ) + (*prop_it).perUnit.name, QString::number( (*prop_it).id ) );
			// Store the perUnits with the ID for using later
			Element perUnitEl;
			perUnitEl.id = (*prop_it).perUnit.id;
			perUnitEl.name = (*prop_it).perUnit.name;
			perUnitListBack->append( perUnitEl );

		}
	}
}

void IngredientsDialog::reloadWeightList( void )
{
	weightsListView->listView() ->clear();

	//If none is selected, select first item
	QListViewItem *it = ingredientListView->listView() ->selectedItem();

	//Populate this data into the KListView
	if ( it ) { // make sure that the ingredient list is not empty
		WeightList list = database->ingredientWeightUnits( it->text( 1 ).toInt() ); // load the list for this ingredient
		for ( WeightList::const_iterator weight_it = list.begin(); weight_it != list.end(); ++weight_it ) {
			QListViewItem * lastElement = weightsListView->listView() ->lastItem();

			Weight w = *weight_it;
			WeightListItem *weight_it = new WeightListItem( weightsListView->listView(), lastElement, w );
			weight_it->setAmountUnit( w.perAmount,
			  database->unitName(w.perAmountUnitID),
			  Element((w.prepMethodID==-1)?QString::null:database->prepMethodName(w.prepMethodID),w.prepMethodID)
			);
			weight_it->setWeightUnit( w.weight, database->unitName(w.weightUnitID) );
		}
	}
}

void IngredientsDialog:: updateLists( void )
{
	reloadUnitList();
	reloadPropertyList();
	reloadWeightList();
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

void IngredientsDialog::reload( ReloadFlags flag )
{
	reloadIngredientList( flag );
	groupsDialog->reload( flag );
}

void IngredientsDialog::openUSDADialog( void )
{
	QListViewItem * ing_it = ingredientListView->listView() ->selectedItem(); // Find selected ingredient
	if ( ing_it ) {
		KApplication::setOverrideCursor( KCursor::waitCursor() );
		USDADataDialog usda_dialog( Element( ing_it->text( 0 ), ing_it->text( 1 ).toInt() ), database, this );
		KApplication::restoreOverrideCursor();

		if ( usda_dialog.exec() == QDialog::Accepted ) {
			reloadPropertyList(); //update property list upon success
			reloadWeightList();
		}
	}
	else
		QMessageBox::information( this, QString::null, i18n( "No ingredient selected." ) );
}

#include "ingredientsdialog.moc"
