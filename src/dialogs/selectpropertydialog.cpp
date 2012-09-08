/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "selectpropertydialog.h"

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <QPointer>

#include "datablocks/ingredientpropertylist.h"
#include "dialogs/createunitdialog.h"
#include "backends/recipedb.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <KVBox>

SelectPropertyDialog::SelectPropertyDialog( QWidget* parent, int ingID, RecipeDB *database, OptionFlag showEmpty )
		: KDialog( parent), m_showEmpty(showEmpty),
		ingredientID(ingID), db(database)
{
	setModal( true );
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton( KDialog::Ok);
	setCaption(i18nc( "@title:window", "Choose Property" ));

	// Initialize internal variables
	unitListBack = new UnitList;

	// Initialize Widgets
	KVBox *page = new KVBox( this );
	setMainWidget( page );
	box = new Q3GroupBox( page );
	box->setTitle( i18nc( "@title:group", "Choose Property" ) );
	box->setColumnLayout( 0, Qt::Vertical );
	box->layout() ->setSpacing( 6 );
	box->layout() ->setMargin( 11 );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
	boxLayout->setAlignment( Qt::AlignTop );

	propertyChooseView = new K3ListView( box );

	KConfigGroup config( KGlobal::config(),  "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	propertyChooseView->addColumn( i18nc( "@title:column", "Id" ), show_id ? -1 : 0 );

	propertyChooseView->addColumn( i18nc( "@title:column", "Property" ) );
	propertyChooseView->setSorting(1);
	propertyChooseView->setAllColumnsShowFocus( true );
	boxLayout->addWidget( propertyChooseView );

	QHBoxLayout *layout2 = new QHBoxLayout;

	perUnitsLabel = new QLabel( box );
	perUnitsLabel->setGeometry( QRect( 5, 285, 100, 30 ) );
	perUnitsLabel->setText( i18nc( "@label:textbox", "Per units:" ) );
	layout2->addWidget( perUnitsLabel );

	perUnitsBox = new KComboBox( true, box );
	perUnitsBox->setInsertPolicy( QComboBox::NoInsert );
	layout2->addWidget( perUnitsBox );
	boxLayout->addLayout( layout2 );

	resize( QSize( 400, 400 ).expandedTo( minimumSizeHint() ) );
	//TODO port kde4
	//clearWState( WState_Polished );

	IngredientPropertyList propertyList;
	db->loadProperties( &propertyList );
	UnitList unitList;
	db->loadPossibleUnits( ingredientID, &unitList );

	// Load data
	loadProperties( &propertyList );
	loadUnits( &unitList );
}


SelectPropertyDialog::~SelectPropertyDialog()
{
	delete unitListBack;
}


int SelectPropertyDialog::propertyID( void )
{

	Q3ListViewItem * it;
	if ( ( it = propertyChooseView->selectedItem() ) ) {
		return ( it->text( 0 ).toInt() );
	}
	else
		return ( -1 );
}

int SelectPropertyDialog::perUnitsID()
{

	int comboCount = perUnitsBox->count();
	for (int i = 0; i < comboCount; ++i) {
		if (perUnitsBox->currentText() == perUnitsBox->itemText(i))
			return unitListBack->at( i ).id();
	}

	//new unit, add it to the database
	QString unit = perUnitsBox->currentText();
	int id = db->findExistingUnitByName( unit );
	if ( -1 == id )
	{
		QPointer<CreateUnitDialog> getUnit = new CreateUnitDialog( this, unit, QString() );
		if ( getUnit->exec() == QDialog::Accepted ) {
			Unit new_unit = getUnit->newUnit();
			id = db->createNewUnit( new_unit );
		}
		delete getUnit;
	}
	db->addUnitToIngredient( ingredientID, id ); // Add chosen unit to ingredient in database

	return id;
}

void SelectPropertyDialog::loadProperties( IngredientPropertyList *propertyList )
{
	for ( IngredientPropertyList::const_iterator prop_it = propertyList->constBegin(); prop_it != propertyList->constEnd(); ++prop_it ) {
		( void ) new Q3ListViewItem( propertyChooseView, QString::number( (*prop_it).id ), (*prop_it).name );
	}
}
void SelectPropertyDialog::loadUnits( UnitList *unitList )
{
	for ( UnitList::const_iterator unit_it = unitList->constBegin(); unit_it != unitList->constEnd(); ++unit_it ) {
		QString unitName = ( *unit_it ).name();
		if ( unitName.isEmpty() ) {
			if ( m_showEmpty == ShowEmptyUnit )
				unitName = ' '+i18nc("@item", "-No unit-");
			else
				continue;
		}

		// Insert in the combobox
		perUnitsBox->insertItem( perUnitsBox->count(), unitName );

		// Store with index for using later
		Unit newUnit( *unit_it );
		newUnit.setName(unitName);
		unitListBack->append( newUnit );
	}
}
