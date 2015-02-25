/***************************************************************************
*   Copyright © 2007 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dialogs/editpropertiesdialog.h"

#include <QPushButton>
#include <QLabel>
#include <q3header.h>
#include <QPointer>
//Added by qt3to4:
#include <QHBoxLayout>
#include <Q3ValueList>
#include <QVBoxLayout>
#include <QTextStream>
#include <KMessageBox>
#include <QFile>
#include <QSplitter>
#include <QGroupBox>

#include <k3listview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kvbox.h>
#include <KDoubleValidator>
#include <KLineEdit>

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

class WeightListItem : public Q3ListViewItem
{
public:
	WeightListItem( Q3ListView *listview, Q3ListViewItem *item, const Weight &w ) : Q3ListViewItem(listview,item), m_weight(w){}

	void setWeight( const Weight &w ) { m_weight = w; }
	Weight weight() const { return m_weight; }

	void setAmountUnit( double amount, const Unit &unit, const Element &prepMethod )
	{
		m_weight.setPerAmount(amount);
		m_weight.setPerAmountUnitId(unit.id());
		m_weight.setPerAmountUnit(unit.determineName(m_weight.perAmount(), /*useAbbrev=*/false));
		m_weight.setPrepMethodId(prepMethod.id);
		m_weight.setPrepMethod(prepMethod.name);
	}

	void setWeightUnit( double weight, const Unit &unit )
	{
		m_weight.setWeight(weight);
		m_weight.setWeightUnitId(unit.id());
		m_weight.setWeightUnit(unit.determineName(m_weight.weight(), /*useAbbrev=*/false));
	}

	virtual QString text( int c ) const
	{
		if ( c == 0 )
			return QString::number(m_weight.weight())+' '+m_weight.weightUnit();
		else if ( c == 1 )
			return QString::number(m_weight.perAmount())+' '
			  +m_weight.perAmountUnit()
			  +((m_weight.prepMethodId()!=-1)?", "+m_weight.prepMethod():QString());
		else
			return QString();
	}

private:
	Weight m_weight;
};



EditPropertiesDialog::EditPropertiesDialog( int ingID, const QString &ingName, RecipeDB *database, QWidget* parent )
	: KDialog( parent ), ingredientID(ingID), ingredientName(ingName)
{
	this->setObjectName( "EditPropertiesDialog" );
	this->setModal( true );
	this->setButtons( KDialog::Ok );
	this->setDefaultButton( KDialog::Ok );

	// Initialize internal variables
	perUnitListBack = new ElementList;
	db = database;

	KVBox *wholePage = new KVBox( this );
	setMainWidget( wholePage );
	QWidget *page = new QWidget(wholePage);

	EditPropertiesDialogLayout = new QVBoxLayout( page );
	EditPropertiesDialogLayout->setObjectName( "EditPropertiesDialogLayout" );
	EditPropertiesDialogLayout->setMargin( 11 );
	EditPropertiesDialogLayout->setSpacing( 6 );

	infoLabel = new QLabel( page );
	infoLabel->setObjectName( "infoLabel" );
	EditPropertiesDialogLayout->addWidget( infoLabel );

	splitter = new QSplitter( Qt::Vertical, page );
	EditPropertiesDialogLayout->addWidget( splitter );

	KVBox* higherBox = new KVBox;

	usdaListView = new KreListView( higherBox, QString(), higherBox, 0 );
	usdaListView->listView()->addColumn( i18nc( "@title:column", "USDA Ingredient" ) );
	usdaListView->listView()->addColumn( "Id" );
	usdaListView->listView()->setAllColumnsShowFocus( true );
	usdaListView->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
	loadDataFromFile();

	loadButton = new QPushButton( higherBox );
	loadButton->setObjectName( "loadButton" );
	loadButton->setIcon( KIcon( "arrow-down" ) );

	splitter->addWidget( higherBox );

	KHBox* lowerBox = new KHBox;

	propertiesBox = new QGroupBox( lowerBox );

	layout7 = new QVBoxLayout();
	layout7->setObjectName( "layout7" );
	layout7->setMargin( 0 );
	layout7->setSpacing( 6 );

	layout3 = new QHBoxLayout();
	layout3->setObjectName( "layout3" );
	layout3->setMargin( 0 );
	layout3->setSpacing( 6 );

	propertyAddButton = new QPushButton( page );
	propertyAddButton->setObjectName( "propertyAddButton" );
	propertyAddButton->setIcon( KIcon( "list-add" ) );
	layout3->addWidget( propertyAddButton );

	propertyRemoveButton = new QPushButton( page );
	loadButton->setObjectName( "propertyRemoveButton" );
	propertyRemoveButton->setIcon( KIcon( "list-remove" ) );
	layout3->addWidget( propertyRemoveButton );
	layout7->addLayout( layout3 );

	propertyListView = new K3ListView( page );
	propertyListView->setObjectName( "propertyListView" );
	propertyListView->setAllColumnsShowFocus( true );
	propertyListView->addColumn( i18nc( "@title:column", "Property" ) );
	propertyListView->addColumn( i18nc( "@title:column", "Amount" ) );
	propertyListView->addColumn( i18nc( "@title:column", "Unit" ) );
	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	propertyListView->addColumn( "Id" , show_id ? -1 : 0 );
	layout7->addWidget( propertyListView );

	propertiesBox->setLayout( layout7 );

	weightsBox = new QGroupBox( lowerBox );

	layout8 = new QVBoxLayout;
	layout8->setObjectName( "layout8" );
	layout8->setMargin( 0 );
	layout8->setSpacing( 6 );

	layout3_2 = new QHBoxLayout();
	layout3_2->setObjectName( "layout3_2" );
	layout3_2->setMargin( 0 );
	layout3_2->setSpacing( 6 );

	weightAddButton = new QPushButton( page );
	weightAddButton->setObjectName( "weightAddButton" );
	weightAddButton->setIcon( KIcon( "list-add" ) );
	layout3_2->addWidget( weightAddButton );

	weightRemoveButton = new QPushButton( page );
	weightRemoveButton->setObjectName( "weightRemoveButton" );
	weightRemoveButton->setIcon( KIcon( "list-remove" ) );
	layout3_2->addWidget( weightRemoveButton );
	layout8->addLayout( layout3_2 );

	weightListView = new K3ListView( page );
	weightListView->setObjectName( "weightListView" );
	weightListView->addColumn( i18nc( "@title:column", "Weight" ) );
	weightListView->addColumn( i18nc( "@title:column", "Per Amount" ) );
	layout8->addWidget( weightListView );

	weightsBox->setLayout( layout8 );
	
	splitter->addWidget( lowerBox );
	splitter->setCollapsible( 1, false );

	languageChange();
	adjustSize();
	resize( 630, size().height() );
	//KDE4 port
	//clearWState( WState_Polished );

	KDoubleValidator * doubleValidator = new KDoubleValidator(this);
	inputBox = new KLineEdit( propertyListView ->viewport() );
	propertyListView ->addChild( inputBox );
	inputBox->setValidator( doubleValidator );
	inputBox->hide();

	connect( weightAddButton, SIGNAL( clicked() ), this, SLOT( addWeight() ) );
	connect( weightRemoveButton, SIGNAL( clicked() ), this, SLOT( removeWeight() ) );
	connect( propertyAddButton, SIGNAL( clicked() ), this, SLOT( addPropertyToIngredient() ) );
	connect( propertyRemoveButton, SIGNAL( clicked() ), this, SLOT( removePropertyFromIngredient() ) );
	connect( propertyListView, SIGNAL( executed( Q3ListViewItem* ) ), this, SLOT( insertPropertyEditBox( Q3ListViewItem* ) ) );
	connect( propertyListView, SIGNAL( selectionChanged() ), inputBox, SLOT( hide() ) );

	removeEventFilter( inputBox );
	connect( inputBox, SIGNAL( returnPressed( const QString & ) ),
		this, SLOT( setPropertyAmount( const QString &) ) );
	connect( inputBox, SIGNAL( returnPressed( const QString & ) ),
		inputBox, SLOT( hide() ) );

	connect( weightListView, SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ), SLOT( itemRenamed( Q3ListViewItem*, const QPoint &, int ) ) );

	connect( loadButton, SIGNAL( clicked() ), this, SLOT( loadUSDAData() ) );

	updateLists();
}

EditPropertiesDialog::~EditPropertiesDialog()
{
	delete perUnitListBack;
}

void EditPropertiesDialog::languageChange()
{
	setCaption( i18nc( "@title:window", "Property Information" ) );
	infoLabel->setText( i18nc( "@label", "Property Information for <b>%1</b>" , ingredientName) );
	usdaListView->listView()->header()->setLabel( 0, i18nc( "@title:column", "USDA Ingredient" ) );
	usdaListView->listView()->header()->setLabel( 1,  "Id" );
	loadButton->setText( i18nc( "@action:button Load USDA data", "Load" ) );
	propertiesBox->setTitle( i18nc( "@title:group", "Properties" ) );
	propertyAddButton->setText( i18nc( "@action:button", "Add" ) );
	propertyRemoveButton->setText( i18nc( "@action:button", "Remove" ) );
	propertyListView->header()->setLabel( 0, i18nc( "@title:column", "Property" ) );
	propertyListView->header()->setLabel( 1, i18nc( "@title:column", "Amount" ) );
	propertyListView->header()->setLabel( 2, i18nc( "@title:column", "Unit" ) );
	weightsBox->setTitle( i18nc( "@title:group", "Weights" ) );
	weightAddButton->setText( i18nc( "@action:button", "Add" ) );
	weightRemoveButton->setText( i18nc( "@action:button", "Remove" ) );
	weightListView->header()->setLabel( 0, i18nc( "@title:column", "Weight" ) );
	weightListView->header()->setLabel( 1, i18nc( "@title:column", "Per Amount" ) );
}

void EditPropertiesDialog::addWeight()
{
	QPointer<CreateIngredientWeightDialog> weightDialog = new CreateIngredientWeightDialog( this, db );
	if ( weightDialog->exec() == QDialog::Accepted ) {
		Weight w = weightDialog->weight();
		w.setIngredientId(ingredientID);
		db->addIngredientWeight( w );

		Q3ListViewItem * lastElement = weightListView->lastItem();

		WeightListItem *weight_it = new WeightListItem( weightListView, lastElement, w );
		weight_it->setAmountUnit( w.perAmount(), db->unitName(w.perAmountUnitId()),
			Element(w.prepMethod(),w.prepMethodId())
		);
		weight_it->setWeightUnit( w.weight(), db->unitName(w.weightUnitId()) );
	}
	delete weightDialog;
}

void EditPropertiesDialog::removeWeight()
{
	Q3ListViewItem *it = weightListView->selectedItem();
	if ( it ) {
		     switch ( KMessageBox::warningContinueCancel(this, i18nc("@info", "Recipes may require this information for nutrient analysis. Are you sure you want to delete this entry?"), QString(), KStandardGuiItem::cont(), KStandardGuiItem::cancel(), "DeleteIngredientWeight") ) {
		case KMessageBox::Continue:
			db->removeIngredientWeight( ((WeightListItem*)it)->weight().id() );
			delete it;
			break;
		default: break;
		}
	}
}

void EditPropertiesDialog::itemRenamed( Q3ListViewItem* item, const QPoint &, int col )
{
	WeightListItem *weight_it = (WeightListItem*)item;
	Weight w = weight_it->weight();

	if ( col == 0 ) {
		QPointer<KDialog> amountEditDialog = new KDialog( this );
		amountEditDialog->setObjectName( "WeightAmountEdit" );
		amountEditDialog->setModal( false );
		amountEditDialog->setCaption( i18nc("@title:window", "Enter amount") );
		amountEditDialog->setButtons( KDialog::Cancel | KDialog::Ok );
		amountEditDialog->setDefaultButton( KDialog::Ok );

		Q3GroupBox *box = new Q3GroupBox( 1, Qt::Horizontal, i18nc("@title:group", "Amount"), amountEditDialog );
		AmountUnitInput *amountEdit = new AmountUnitInput( box, db, Unit::Mass, MixedNumber::DecimalFormat );

		WeightListItem *it = (WeightListItem*)item;
		Weight w = it->weight();

		amountEdit->setAmount( MixedNumber(w.weight()) );
		amountEdit->setUnit( Unit(w.weightUnit(),w.weightUnit(),w.weightUnitId()) );

		amountEditDialog->setMainWidget(box);
		amountEditDialog->adjustSize();
		amountEditDialog->resize( 400, amountEditDialog->size().height() );
		amountEditDialog->setFixedHeight( amountEditDialog->size().height() );

		if ( amountEditDialog->exec() == QDialog::Accepted
		&& amountEdit->isInputValid() ) {
			MixedNumber amount = amountEdit->amount();
			Unit unit = amountEdit->unit();

			it->setWeightUnit( amount.toDouble(), unit );
			db->addIngredientWeight( it->weight() );
		}
		delete amountEditDialog;
	}
	else if ( col == 1 ) {
		QPointer<KDialog> amountEditDialog = new KDialog( this );
		amountEditDialog->setObjectName( "PerAmountEdit" );
		amountEditDialog->setModal( false );
		amountEditDialog->setCaption( i18nc("@title:window", "Enter amount") );
		amountEditDialog->setButtons( KDialog::Cancel | KDialog::Ok );
		amountEditDialog->setDefaultButton( KDialog::Ok );

		Q3GroupBox *box = new Q3GroupBox( 1, Qt::Horizontal, i18nc("@title:group", "Amount"), amountEditDialog );
		WeightInput *amountEdit = new WeightInput( box, db, Unit::All, MixedNumber::DecimalFormat );

		WeightListItem *it = (WeightListItem*)item;
		Weight w = it->weight();

		amountEdit->setAmount( MixedNumber(w.perAmount()) );
		amountEdit->setUnit( Unit(w.perAmountUnit(),w.perAmountUnit(),w.perAmountUnitId()) );
		amountEdit->setPrepMethod( Element(w.prepMethod(),w.prepMethodId()) );

		amountEditDialog->setMainWidget(box);
		amountEditDialog->adjustSize();
		amountEditDialog->resize( 400, amountEditDialog->size().height() );
		amountEditDialog->setFixedHeight( amountEditDialog->size().height() );

		if ( amountEditDialog->exec() == QDialog::Accepted
		&& amountEdit->isInputValid() ) {
			MixedNumber amount = amountEdit->amount();
			Unit unit = amountEdit->unit();

			it->setAmountUnit( amount.toDouble(), unit, amountEdit->prepMethod() );
			db->addIngredientWeight( it->weight() );
		}
		delete amountEditDialog;
	}
}

void EditPropertiesDialog:: reloadPropertyList( void )
{
	propertyListView ->clear();
	perUnitListBack->clear();

	inputBox->hide();

	IngredientPropertyList propertiesList;
	db->loadProperties( &propertiesList, ingredientID ); // load the list for this ingredient
	for ( IngredientPropertyList::const_iterator prop_it = propertiesList.constBegin(); prop_it != propertiesList.constEnd(); ++prop_it ) {
		Q3ListViewItem * lastElement = propertyListView ->lastItem();
		//Insert property after the last one (it's important to keep the order in the case of the properties to be able to identify the per_units ID later on).
		( void ) new Q3ListViewItem( propertyListView, lastElement, (*prop_it).name, QString::number( (*prop_it).amount ), (*prop_it).units + QString( "/" ) + (*prop_it).perUnit.name(), QString::number( (*prop_it).id ) );
		// Store the perUnits with the ID for using later
		Element perUnitEl;
		perUnitEl.id = (*prop_it).perUnit.id();
		perUnitEl.name = (*prop_it).perUnit.name();
		perUnitListBack->append( perUnitEl );

	}
}

void EditPropertiesDialog::reloadWeightList( void )
{
	weightListView ->clear();

	WeightList list = db->ingredientWeightUnits( ingredientID ); // load the list for this ingredient
	for ( WeightList::const_iterator weight_it = list.constBegin(); weight_it != list.constEnd(); ++weight_it ) {
		Q3ListViewItem * lastElement = weightListView ->lastItem();

		Weight w = *weight_it;
		WeightListItem *weight = new WeightListItem( weightListView, lastElement, w );
		weight->setAmountUnit( w.perAmount(),
			db->unitName(w.perAmountUnitId()),
			Element((w.prepMethodId()==-1)?QString():db->prepMethodName(w.prepMethodId()),w.prepMethodId())
		);
		weight->setWeightUnit( w.weight(), db->unitName(w.weightUnitId()) );
	}
}

void EditPropertiesDialog:: updateLists( void )
{
	reloadPropertyList();
	reloadWeightList();
}

void EditPropertiesDialog::addPropertyToIngredient( void )
{
	QPointer<SelectPropertyDialog> propertyDialog = 
		new SelectPropertyDialog( this, ingredientID, db, SelectPropertyDialog::HideEmptyUnit );

	if ( propertyDialog->exec() == QDialog::Accepted )
	{
		int propertyID = propertyDialog->propertyID();
		int perUnitsID = -1;
		if ( propertyID != -1 ) // check if the property is not -1 ... (not selected)
			perUnitsID = propertyDialog->perUnitsID();
		if ( !( db->ingredientContainsProperty( ingredientID, propertyID, perUnitsID ) ) ) {
			if ( ( propertyID >= 0 ) && ( perUnitsID >= 0 ) )  
				db->addPropertyToIngredient( ingredientID, propertyID, 0, perUnitsID ); // Add result chosen property to ingredient in database, with amount 0 by default
		}
		else {
			KMessageBox::information( this, i18nc( "@title:window" , "Property Exists" ), i18nc( "@info", "The property you tried to add already exists in the ingredient with the same per units." ) );
		}
		reloadPropertyList(); // Reload the list from database
	}

	delete propertyDialog;
}

void EditPropertiesDialog::removePropertyFromIngredient( void )
{

	// Find selected ingredient/property item combination
	Q3ListViewItem * it;
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

void EditPropertiesDialog::insertPropertyEditBox( Q3ListViewItem* it )
{
	QRect r = propertyListView ->header() ->sectionRect( 1 );

	r.translate( 0, propertyListView ->itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height

	inputBox->setGeometry( r );
	inputBox->setText( it->text( 1 ) );
	inputBox->show();
}

void EditPropertiesDialog::setPropertyAmount( const QString & amount )
{
	setPropertyAmount( amount.toDouble() );
}

void EditPropertiesDialog::setPropertyAmount( double amount )
{
	Q3ListViewItem *prop_it = propertyListView ->selectedItem();

	if ( prop_it ) // Appart from property, Check if an ingredient is selected first, just in case
	{
		prop_it->setText( 1, QString::number( amount ) );
		int propertyID = prop_it->text( 3 ).toInt();
		int per_units = perUnitListBack->getElement( findPropertyNo( prop_it ) ).id ;
		db->changePropertyAmountToIngredient( ingredientID, propertyID, amount, per_units );
	}
}

int EditPropertiesDialog::findPropertyNo( Q3ListViewItem * /*it*/ )
{
	bool found = false;
	int i = 0;
	Q3ListViewItem* item = propertyListView ->firstChild();
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
	QString abbrev_file = KStandardDirs::locate( "appdata", "data/abbrev.txt" );
	if ( abbrev_file.isEmpty() ) {
		kDebug() << "Unable to find abbrev.txt data file." ;
		return ;
	}

	QFile file( abbrev_file );
	if ( !file.open( QIODevice::ReadOnly ) ) {
		kDebug() << "Unable to open data file: " << abbrev_file ;
		return ;
	}

	int index = 0;
	QTextStream stream( &file );
	while ( !stream.atEnd() ) {
		QString line = stream.readLine();
		if ( line.isEmpty() ) {
			continue;
		}

		QStringList fields = line.split( '^', QString::KeepEmptyParts );
		loaded_data << fields;

		QString ing_id = fields[ 0 ].mid( 1, fields[ 1 ].length() - 2 );
		QString ing_name = fields[ 1 ].mid( 1, fields[ 1 ].length() - 2 );
		( void ) new Q3ListViewItem( usdaListView->listView(), ing_name, QString::number( index ) ); //using an index instead of the actual id will help find the data later

		index++;
	}
}

void EditPropertiesDialog::loadUSDAData()
{
	Q3ListViewItem * item = usdaListView->listView()->selectedItem();
	if ( item ) {
		int index = item->text( 1 ).toInt();
		const QStringList data = loaded_data[ index ];

		int grams_id = db->findExistingUnitByName( "g" ); //get this id because all data is given per gram
		if ( grams_id == -1 ) {
			//FIXME: take advantage of abbreviations
			Unit unit("g","g");
			unit.setType(Unit::Mass);
			grams_id = db->createNewUnit( unit );
		}
		else {
			Unit unit = db->unitName(grams_id);
			if ( unit.type() != Unit::Mass ) {
				unit.setType(Unit::Mass);
				db->modUnit( unit );
			}
		}

		IngredientPropertyList property_list;
		db->loadProperties( &property_list );
		IngredientPropertyList existing_ing_props;
		db->loadProperties( &existing_ing_props, ingredientID );

		Q3ValueList<USDA::PropertyData> property_data_list = USDA::loadProperties();
		USDA::PrepDataList prep_data_list =  USDA::loadPrepMethods();
		USDA::UnitDataList unit_data_list =  USDA::loadUnits();
		 
		Q3ValueList<USDA::PropertyData>::const_iterator propertyIt = property_data_list.constBegin();
		for ( QStringList::const_iterator it = data.constBegin()+2; propertyIt != property_data_list.constEnd(); ++it, ++propertyIt ) {
			int property_id = property_list.findByName( (*propertyIt).name );
			if ( property_id == -1 ) {
				property_id = db->addProperty( (*propertyIt).name, (*propertyIt).unit );
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
			w.setWeight(data[i].toDouble());

			i++;

			QString amountAndWeight = data[i].mid( 1, data[i].length() - 2 );
			if ( !amountAndWeight.isEmpty() ) {
				int spaceIndex = amountAndWeight.indexOf(" ");
				w.setPerAmount(amountAndWeight.left(spaceIndex).toDouble());

				QString perAmountUnit = amountAndWeight.right(amountAndWeight.length()-spaceIndex-1);

				QString w_perAmountUnit(w.perAmountUnit()), w_prepMethod(w.prepMethod());
				if ( !USDA::parseUnitAndPrep( perAmountUnit, w_perAmountUnit, w_prepMethod, unit_data_list, prep_data_list ) ) 
					continue;
				w.setPerAmountUnit(w_perAmountUnit);
				w.setPrepMethod(w_prepMethod);

				int unitID = db->findExistingUnitByName( w.perAmountUnit() );
				if ( unitID == -1 ) {
					for ( USDA::UnitDataList::const_iterator it = unit_data_list.constBegin(); it != unit_data_list.constEnd(); ++it ) {
						if ( w.perAmountUnit() == (*it).translation || w.perAmountUnit() == (*it).translationPlural ) {
							unitID = db->createNewUnit( Unit((*it).translation,(*it).translationPlural) );
						}
					}
				}
				w.setPerAmountUnitId(unitID);

				if ( !w.prepMethod().isEmpty() ) {
					int prepID = db->findExistingPrepByName( w.prepMethod() );
					if ( prepID == -1 ) {
						prepID = db->createNewPrepMethod( w.prepMethod() );
					}
					w.setPrepMethodId(prepID);
				}

				bool exists = false;
				for ( WeightList::const_iterator it = weights.constBegin(); it != weights.constEnd(); ++it ) {
					if ( (*it).perAmountUnitId() == w.perAmountUnitId() && (*it).prepMethodId() == w.prepMethodId() ) {
						exists = true;
						break;
					}
				}
				if ( exists )
					continue;

				w.setWeightUnitId(grams_id);
				w.setIngredientId(ingredientID);
				db->addIngredientWeight( w );
			}
		}
	}
	else {
		KMessageBox::sorry( this, i18nc( "@info",
		"Please select an ingredient in the USDA ingredient list" ) );
	}

	updateLists();
}
