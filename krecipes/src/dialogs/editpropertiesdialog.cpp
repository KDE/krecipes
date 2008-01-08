/***************************************************************************
*   Copyright (C) 2007 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dialogs/editpropertiesdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qheader.h>
#include <klistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qfile.h>

#include <klistview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include "widgets/weightinput.h"
#include "widgets/krelistview.h"
#include "dialogs/createingredientweightdialog.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/mixednumber.h"
#include "datablocks/weight.h"
#include "datablocks/unit.h"
#include "unitsdialog.h"
#include "selectpropertydialog.h"
#include "selectunitdialog.h"
#include "backends/recipedb.h"
#include "backends/usda_property_data.h"
#include "backends/usda_unit_data.h"

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



EditPropertiesDialog::EditPropertiesDialog( int ingID, const QString &ingName, RecipeDB *database, QWidget* parent )
		: KDialogBase( parent, "EditPropertiesDialog", true, QString::null,
		    KDialogBase::Ok, KDialogBase::Ok ),
  ingredientID(ingID), ingredientName(ingName)
{
	// Initialize internal variables
	perUnitListBack = new ElementList;
	db = database;

	QVBox *wholePage = makeVBoxMainWidget();
	QWidget *page = new QWidget(wholePage);

	EditPropertiesDialogLayout = new QVBoxLayout( page, 11, 6, "EditPropertiesDialogLayout"); 

	infoLabel = new QLabel( page, "infoLabel" );
	EditPropertiesDialogLayout->addWidget( infoLabel );

	layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

	layout8 = new QVBoxLayout( 0, 0, 6, "layout8"); 

	usdaListView = new KreListView( page, QString::null, page, 0 );
	usdaListView->listView()->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
	usdaListView->listView()->addColumn( i18n( "USDA Ingredient" ) );
	usdaListView->listView()->addColumn( i18n( "Id" ) );
	usdaListView->listView()->setAllColumnsShowFocus( true );
	loadDataFromFile();
	
	layout8->addWidget( usdaListView );
	layout9->addLayout( layout8 );

	loadButton = new QPushButton( page, "loadButton" );
	layout9->addWidget( loadButton );

	layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 

	layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

	propertyLabel = new QLabel( page, "propertyLabel" );
	layout3->addWidget( propertyLabel );

	propertyAddButton = new QPushButton( page, "propertyAddButton" );
	layout3->addWidget( propertyAddButton );

	propertyRemoveButton = new QPushButton( page, "propertyRemoveButton" );
	layout3->addWidget( propertyRemoveButton );
	layout7->addLayout( layout3 );

	propertyListView = new KListView( page, "propertyListView" );
	propertyListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	propertyListView->setAllColumnsShowFocus( true );
	propertyListView->addColumn( i18n( "Property" ) );
	propertyListView->addColumn( i18n( "Amount" ) );
	propertyListView->addColumn( i18n( "Unit" ) );
	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	propertyListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
	layout7->addWidget( propertyListView );

	layout3_2 = new QHBoxLayout( 0, 0, 6, "layout3_2"); 

	weightLabel = new QLabel( page, "weightLabel" );
	layout3_2->addWidget( weightLabel );

	weightAddButton = new QPushButton( page, "weightAddButton" );
	layout3_2->addWidget( weightAddButton );

	weightRemoveButton = new QPushButton( page, "weightRemoveButton" );
	layout3_2->addWidget( weightRemoveButton );
	layout7->addLayout( layout3_2 );

	weightListView = new KListView( page, "weightListView" );
	weightListView->addColumn( i18n( "Weight" ) );
	weightListView->addColumn( i18n( "Per Amount" ) );
	layout7->addWidget( weightListView );
	layout9->addLayout( layout7 );
	EditPropertiesDialogLayout->addLayout( layout9 );
	languageChange();
	//adjustSize();
	clearWState( WState_Polished );

	inputBox = new KDoubleNumInput( propertyListView ->viewport() );
	propertyListView ->addChild( inputBox );
	inputBox->hide();

	connect( weightAddButton, SIGNAL( clicked() ), this, SLOT( addWeight() ) );
	connect( weightRemoveButton, SIGNAL( clicked() ), this, SLOT( removeWeight() ) );
	connect( propertyAddButton, SIGNAL( clicked() ), this, SLOT( addPropertyToIngredient() ) );
	connect( propertyRemoveButton, SIGNAL( clicked() ), this, SLOT( removePropertyFromIngredient() ) );
	connect( propertyListView, SIGNAL( executed( QListViewItem* ) ), this, SLOT( insertPropertyEditBox( QListViewItem* ) ) );
	connect( propertyListView, SIGNAL( selectionChanged() ), inputBox, SLOT( hide() ) );
	connect( inputBox, SIGNAL( valueChanged( double ) ), this, SLOT( setPropertyAmount( double ) ) );

	connect( weightListView, SIGNAL( doubleClicked( QListViewItem*, const QPoint &, int ) ), SLOT( itemRenamed( QListViewItem*, const QPoint &, int ) ) );

	connect( loadButton, SIGNAL( clicked() ), this, SLOT( loadUSDAData() ) );

	updateLists();
}

EditPropertiesDialog::~EditPropertiesDialog()
{
	delete perUnitListBack;
}

void EditPropertiesDialog::languageChange()
{
	setCaption( i18n( "Property Information" ) );
	infoLabel->setText( i18n( "Property Information for <b>%1</b>" ).arg(ingredientName) );
	usdaListView->listView()->header()->setLabel( 0, i18n( "USDA Ingredient" ) );
	usdaListView->listView()->header()->setLabel( 1, i18n( "Id" ) );
	loadButton->setText( i18n( "Load ->" ) );
	propertyLabel->setText( i18n( "Properties" ) );
	propertyAddButton->setText( i18n( "Add" ) );
	propertyRemoveButton->setText( i18n( "Remove" ) );
	propertyListView->header()->setLabel( 0, i18n( "Property" ) );
	propertyListView->header()->setLabel( 1, i18n( "Amount" ) );
	propertyListView->header()->setLabel( 2, i18n( "Unit" ) );
	weightLabel->setText( i18n( "Weights" ) );
	weightAddButton->setText( i18n( "Add" ) );
	weightRemoveButton->setText( i18n( "Remove" ) );
	weightListView->header()->setLabel( 0, i18n( "Weight" ) );
	weightListView->header()->setLabel( 1, i18n( "Per Amount" ) );
}

void EditPropertiesDialog::addWeight()
{
	CreateIngredientWeightDialog weightDialog( this, db );
	if ( weightDialog.exec() == QDialog::Accepted ) {
		Weight w = weightDialog.weight();
		w.ingredientID = ingredientID;
		db->addIngredientWeight( w );

		QListViewItem * lastElement = weightListView->lastItem();

		WeightListItem *weight_it = new WeightListItem( weightListView, lastElement, w );
		weight_it->setAmountUnit( w.perAmount, db->unitName(w.perAmountUnitID),
			Element(w.prepMethod,w.prepMethodID)
		);
		weight_it->setWeightUnit( w.weight, db->unitName(w.weightUnitID) );
	}
}

void EditPropertiesDialog::removeWeight()
{
	QListViewItem *it = weightListView ->selectedItem();
	if ( it ) {
			switch ( KMessageBox::warningContinueCancel(this, i18n("Recipes may require this information for nutrient analysis. Are you sure you want to delete this entry?"), QString::null, KStdGuiItem::cont(), "DeleteIngredientWeight") ) {
		case KMessageBox::Continue:
			db->removeIngredientWeight( ((WeightListItem*)it)->weight().id );
			delete it;
			break;
		default: break;
		}
	}
}

void EditPropertiesDialog::itemRenamed( QListViewItem* item, const QPoint &, int col )
{
	WeightListItem *weight_it = (WeightListItem*)item;
	Weight w = weight_it->weight();

	if ( col == 0 ) {
		KDialogBase amountEditDialog(this,"WeightAmountEdit",
		  false, i18n("Enter amount"), KDialogBase::Cancel | KDialogBase::Ok, KDialogBase::Ok);

		QGroupBox *box = new QGroupBox( 1, Horizontal, i18n("Amount"), &amountEditDialog );
		AmountUnitInput *amountEdit = new AmountUnitInput( box, db, Unit::Mass, MixedNumber::DecimalFormat );

		WeightListItem *it = (WeightListItem*)item;
		Weight w = it->weight();

		amountEdit->setAmount( w.weight );
		amountEdit->setUnit( Unit(w.weightUnit,w.weightUnit,w.weightUnitID) );

		amountEditDialog.setMainWidget(box);

		if ( amountEditDialog.exec() == QDialog::Accepted ) {
			MixedNumber amount = amountEdit->amount();
			Unit unit = amountEdit->unit();

			it->setWeightUnit( amount.toDouble(), unit );
			db->addIngredientWeight( it->weight() );
		}
	}
	else if ( col == 1 ) {
		KDialogBase amountEditDialog(this,"PerAmountEdit",
		  false, i18n("Enter amount"), KDialogBase::Cancel | KDialogBase::Ok, KDialogBase::Ok);

		QGroupBox *box = new QGroupBox( 1, Horizontal, i18n("Amount"), &amountEditDialog );
		WeightInput *amountEdit = new WeightInput( box, db, Unit::All, MixedNumber::DecimalFormat );

		WeightListItem *it = (WeightListItem*)item;
		Weight w = it->weight();

		amountEdit->setAmount( w.perAmount );
		amountEdit->setUnit( Unit(w.perAmountUnit,w.perAmountUnit,w.perAmountUnitID) );
		amountEdit->setPrepMethod( Element(w.prepMethod,w.prepMethodID) );

		amountEditDialog.setMainWidget(box);

		if ( amountEditDialog.exec() == QDialog::Accepted ) {
			MixedNumber amount = amountEdit->amount();
			Unit unit = amountEdit->unit();

			it->setAmountUnit( amount.toDouble(), unit, amountEdit->prepMethod() );
			db->addIngredientWeight( it->weight() );
		}
	}
}

void EditPropertiesDialog:: reloadPropertyList( void )
{
	propertyListView ->clear();
	perUnitListBack->clear();

	inputBox->hide();

	IngredientPropertyList propertiesList;
	db->loadProperties( &propertiesList, ingredientID ); // load the list for this ingredient
	for ( IngredientPropertyList::const_iterator prop_it = propertiesList.begin(); prop_it != propertiesList.end(); ++prop_it ) {
		QListViewItem * lastElement = propertyListView ->lastItem();
		//Insert property after the last one (it's important to keep the order in the case of the properties to be able to identify the per_units ID later on).
		( void ) new QListViewItem( propertyListView, lastElement, (*prop_it).name, QString::number( (*prop_it).amount ), (*prop_it).units + QString( "/" ) + (*prop_it).perUnit.name, QString::number( (*prop_it).id ) );
		// Store the perUnits with the ID for using later
		Element perUnitEl;
		perUnitEl.id = (*prop_it).perUnit.id;
		perUnitEl.name = (*prop_it).perUnit.name;
		perUnitListBack->append( perUnitEl );

	}
}

void EditPropertiesDialog::reloadWeightList( void )
{
	weightListView ->clear();

	WeightList list = db->ingredientWeightUnits( ingredientID ); // load the list for this ingredient
	for ( WeightList::const_iterator weight_it = list.begin(); weight_it != list.end(); ++weight_it ) {
		QListViewItem * lastElement = weightListView ->lastItem();

		Weight w = *weight_it;
		WeightListItem *weight_it = new WeightListItem( weightListView, lastElement, w );
		weight_it->setAmountUnit( w.perAmount,
			db->unitName(w.perAmountUnitID),
			Element((w.prepMethodID==-1)?QString::null:db->prepMethodName(w.prepMethodID),w.prepMethodID)
		);
		weight_it->setWeightUnit( w.weight, db->unitName(w.weightUnitID) );
	}
}

void EditPropertiesDialog:: updateLists( void )
{
	reloadPropertyList();
	reloadWeightList();
}

void EditPropertiesDialog::addPropertyToIngredient( void )
{
	SelectPropertyDialog propertyDialog( this, ingredientID, db, SelectPropertyDialog::HideEmptyUnit );

	if ( propertyDialog.exec() == QDialog::Accepted )
	{

		int propertyID = propertyDialog.propertyID();
		int perUnitsID = propertyDialog.perUnitsID();
		if ( !( db->ingredientContainsProperty( ingredientID, propertyID, perUnitsID ) ) ) {
			if ( ( propertyID >= 0 ) && ( perUnitsID >= 0 ) )  // check if the property is not -1 ... (not selected)
				db->addPropertyToIngredient( ingredientID, propertyID, 0, perUnitsID ); // Add result chosen property to ingredient in database, with amount 0 by default
		}
		else {
			QMessageBox::information( this, i18n( "Property Exists" ), i18n( "The property you tried to add already exists in the ingredient with the same per units." ) );
		}
		reloadPropertyList(); // Reload the list from database
	}
}

void EditPropertiesDialog::removePropertyFromIngredient( void )
{

	// Find selected ingredient/property item combination
	QListViewItem * it;
	int propertyID = -1;
	int perUnitsID = -1;
	if ( ( it = propertyListView ->selectedItem() ) )
		propertyID = it->text( 3 ).toInt();
	if ( propertyID >= 0 )
		perUnitsID = perUnitListBack->getElement( findPropertyNo( it ) ).id ;

	if ( ( propertyID >= 0 ) && ( perUnitsID >= 0 ) )  // an ingredient/property combination was selected previously
	{
		ElementList results;
		db->removePropertyFromIngredient( ingredientID, propertyID, perUnitsID );

		reloadPropertyList(); // Reload the list from database

	}
}

void EditPropertiesDialog::insertPropertyEditBox( QListViewItem* it )
{
	QRect r = propertyListView ->header() ->sectionRect( 1 );

	r.moveBy( 0, propertyListView ->itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height

	inputBox->setGeometry( r );
	inputBox->setValue( it->text( 1 ).toDouble() );
	inputBox->show();
}

void EditPropertiesDialog::setPropertyAmount( double amount )
{
	QListViewItem *prop_it = propertyListView ->selectedItem();

	if ( prop_it ) // Appart from property, Check if an ingredient is selected first, just in case
	{
		prop_it->setText( 1, QString::number( amount ) );
		int propertyID = prop_it->text( 3 ).toInt();
		int per_units = perUnitListBack->getElement( findPropertyNo( prop_it ) ).id ;
		db->changePropertyAmountToIngredient( ingredientID, propertyID, amount, per_units );
	}
}

int EditPropertiesDialog::findPropertyNo( QListViewItem * /*it*/ )
{
	bool found = false;
	int i = 0;
	QListViewItem* item = propertyListView ->firstChild();
	while ( i < propertyListView ->childCount() && !found ) {
		if ( item == propertyListView ->currentItem() )
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

void EditPropertiesDialog::loadDataFromFile()
{
	QString abbrev_file = locate( "appdata", "data/abbrev.txt" );
	if ( abbrev_file.isEmpty() ) {
		kdDebug() << "Unable to find abbrev.txt data file." << endl;
		return ;
	}

	QFile file( abbrev_file );
	if ( !file.open( IO_ReadOnly ) ) {
		kdDebug() << "Unable to open data file: " << abbrev_file << endl;
		return ;
	}

	int index = 0;
	QTextStream stream( &file );
	while ( !stream.atEnd() ) {
		QString line = stream.readLine();
		if ( line.isEmpty() ) {
			continue;
		}

		QStringList fields = QStringList::split( "^", line, true );
		loaded_data << fields;

		QString ing_id = fields[ 0 ].mid( 1, fields[ 1 ].length() - 2 );
		QString ing_name = fields[ 1 ].mid( 1, fields[ 1 ].length() - 2 );
		( void ) new QListViewItem( usdaListView->listView(), ing_name, QString::number( index ) ); //using an index instead of the actual id will help find the data later

		index++;
	}
}

void EditPropertiesDialog::loadUSDAData()
{
	QListViewItem * item = usdaListView->listView()->selectedItem();
	if ( item ) {
		int index = item->text( 1 ).toInt();
		QStringList data = loaded_data[ index ];

		int grams_id = db->findExistingUnitByName( "g" ); //get this id because all data is given per gram
		if ( grams_id == -1 ) {
			//FIXME: take advantage of abbreviations
			Unit unit("g","g");
			unit.type = Unit::Mass;
			db->createNewUnit( unit );
			grams_id = db->lastInsertID();
		}
		else {
			Unit unit = db->unitName(grams_id);
			if ( unit.type != Unit::Mass ) {
				unit.type = Unit::Mass;
				db->modUnit( unit );
			}
		}

		IngredientPropertyList property_list;
		db->loadProperties( &property_list );
		IngredientPropertyList existing_ing_props;
		db->loadProperties( &existing_ing_props, ingredientID );

		QValueList<USDA::PropertyData> property_data_list = USDA::loadProperties();
		USDA::PrepDataList prep_data_list =  USDA::loadPrepMethods();
		USDA::UnitDataList unit_data_list =  USDA::loadUnits();


		QValueList<USDA::PropertyData>::const_iterator propertyIt = property_data_list.begin();
		for ( QStringList::const_iterator it = data.at( 2 ); propertyIt != property_data_list.end(); ++it, ++propertyIt ) {
			int property_id = property_list.findByName( (*propertyIt).name );
			if ( property_id == -1 ) {
				db->addProperty( (*propertyIt).name, (*propertyIt).unit );
				property_id = db->lastInsertID();
			}

			double amount = ( *it ).toDouble() / 100.0; //data givin per 100g so divide by 100 to get the amount in 1 gram

			if ( existing_ing_props.find( property_id ) != existing_ing_props.end() )  //property already added to ingredient, so just update
				db->changePropertyAmountToIngredient( ingredientID, property_id, amount, grams_id );
			else
				db->addPropertyToIngredient( ingredientID, property_id, amount, grams_id );
		}

		int i = 2 + property_data_list.count();

		int i_initial = i;
		WeightList weights = db->ingredientWeightUnits( ingredientID );
		for ( ; i < i_initial+3; ++i ) {
			Weight w;
			w.weight = data[i].toDouble();

			i++;

			QString amountAndWeight = data[i].mid( 1, data[i].length() - 2 );
			if ( !amountAndWeight.isEmpty() ) {
				int spaceIndex = amountAndWeight.find(" ");
				w.perAmount = amountAndWeight.left(spaceIndex).toDouble();

				QString perAmountUnit = amountAndWeight.right(amountAndWeight.length()-spaceIndex-1);
				if ( !USDA::parseUnitAndPrep( perAmountUnit, w.perAmountUnit, w.prepMethod, unit_data_list, prep_data_list ) )
					continue;

				int unitID = db->findExistingUnitByName( w.perAmountUnit );
				if ( unitID == -1 ) {
					for ( USDA::UnitDataList::const_iterator it = unit_data_list.begin(); it != unit_data_list.end(); ++it ) {
						if ( w.perAmountUnit == (*it).translation || w.perAmountUnit == (*it).translationPlural ) {
							db->createNewUnit( Unit((*it).translation,(*it).translationPlural) );
						}
					}

					unitID = db->lastInsertID();
				}
				w.perAmountUnitID = unitID;

				if ( !w.prepMethod.isEmpty() ) {
					int prepID = db->findExistingPrepByName( w.prepMethod );
					if ( prepID == -1 ) {
						db->createNewPrepMethod( w.prepMethod );
						prepID = db->lastInsertID();
					}
					w.prepMethodID = prepID;
				}

				bool exists = false;
				for ( WeightList::const_iterator it = weights.begin(); it != weights.end(); ++it ) {
					if ( (*it).perAmountUnitID == w.perAmountUnitID && (*it).prepMethodID == w.prepMethodID ) {
						exists = true;
						break;
					}
				}
				if ( exists )
					continue;

				w.weightUnitID = grams_id;
				w.ingredientID = ingredientID;
				db->addIngredientWeight( w );
			}
		}
	}
	updateLists();
}
