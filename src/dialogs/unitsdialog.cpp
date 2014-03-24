/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "unitsdialog.h"

#include <KTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "createelementdialog.h"
#include "dependanciesdialog.h"
#include "backends/recipedb.h"
#include "widgets/conversiontable.h" //TODO: Remove, deprecated.
#include "widgets/kreconversiontable.h"
#include "widgets/unitlistview.h"
#include "actionshandlers/unitactionshandler.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogressdialog.h>
#include <KPushButton>

UnitsDialog::UnitsDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog
	QHBoxLayout* page_layout = new QHBoxLayout( this );

	tabWidget = new KTabWidget( this );

	unitTab = new QWidget( tabWidget );
	QHBoxLayout* layout = new QHBoxLayout( unitTab );

	unitListView = new StdUnitListView( unitTab, database, true );
	unitActionsHandler = new UnitActionsHandler( unitListView, database );
	layout->addWidget( unitListView );

	QVBoxLayout* vboxl = new QVBoxLayout();

	newUnitButton = new KPushButton( unitTab );
	newUnitButton->setText( i18nc( "@action:button", "Create..." ) );
	newUnitButton->setIcon( KIcon ( "list-add" ) );
	vboxl->addWidget( newUnitButton );

	removeUnitButton = new KPushButton( unitTab );
	removeUnitButton->setText( i18nc( "@action:button", "Delete" ) );
	removeUnitButton->setIcon( KIcon ( "list-remove" ) );
	vboxl->addWidget( removeUnitButton );
	vboxl->addStretch();
	layout->addLayout( vboxl );

	tabWidget->insertTab( -1, unitTab, i18nc( "@title:tab", "Units" ) );

	massConversionTable = new KreConversionTable( tabWidget, 1, 1 );
	tabWidget->insertTab( -1, massConversionTable, i18nc( "@title:tab", "Mass Conversions" ) );

	volumeConversionTable = new KreConversionTable( tabWidget, 1, 1 );
	tabWidget->insertTab( -1, volumeConversionTable, i18nc( "@title:tab", "Volume Conversions" ) );

	page_layout->addWidget( tabWidget );

	// Connect signals & slots
	connect( newUnitButton, SIGNAL( clicked() ), unitActionsHandler, SLOT( createNew() ) );
	connect( removeUnitButton, SIGNAL( clicked() ), unitActionsHandler, SLOT( remove() ) );
	connect( massConversionTable, SIGNAL( ratioChanged( int, int, double ) ), this, SLOT( saveRatio( int, int, double ) ) );
	connect( massConversionTable, SIGNAL( ratioRemoved( int, int ) ), this, SLOT( removeRatio( int, int ) ) );
	connect( volumeConversionTable, SIGNAL( ratioChanged( int, int, double ) ), this, SLOT( saveRatio( int, int, double ) ) );
	connect( volumeConversionTable, SIGNAL( ratioRemoved( int, int ) ), this, SLOT( removeRatio( int, int ) ) );

	//TODO: I'm too lazy right now, so do a complete reload to keep in sync with db
	connect( database, SIGNAL( unitCreated( const Unit& ) ), this, SLOT( loadConversionTables() ) );
	connect( database, SIGNAL( unitRemoved( int ) ), this, SLOT( loadConversionTables() ) );

	//this is for the above TODO, but it still has some bugs to be worked out
	//connect(database,SIGNAL(unitCreated(const Element&)),conversionTable,SLOT(unitCreated(const Element&)));
	//connect(database,SIGNAL(unitRemoved(int)),conversionTable,SLOT(unitRemoved(int)));

	//Populate data into the table
	loadConversionTables();

}

UnitsDialog::~UnitsDialog()
{}

void UnitsDialog::reload( ReloadFlags flag )
{
	unitListView->reload( flag );
	loadConversionTables();
}

UnitActionsHandler* UnitsDialog::getActionsHandler() const
{
	if ( tabWidget->currentWidget() == unitTab )
		return unitActionsHandler;
	else
		return 0;
}

void UnitsDialog::addAction( KAction * action )
{
	unitActionsHandler->addAction( action );
}

void UnitsDialog::loadConversionTables( void )
{
	loadConversionTable( massConversionTable, Unit::Mass );
	loadConversionTable( volumeConversionTable, Unit::Volume );
}

void UnitsDialog::loadConversionTable( KreConversionTable *table, Unit::Type type )
{
	UnitList unitList;
	database->loadUnits( &unitList, type );

	QStringList unitNames;
	IDList unitIDs; // We need to store these in the table, so rows and cols are identified by unitID, not name.
	table->clear();
	for ( UnitList::const_iterator unit_it = unitList.constBegin(); unit_it != unitList.constEnd(); ++unit_it ) {
		unitNames.append( ( *unit_it ).name() );
		unitIDs.append( ( *unit_it ).id() ); // append the element
	}

	// Resize the table
	table->resize( unitNames.count(), unitNames.count() );

	// Set the table labels, and id's
	table->setHorizontalHeaderLabels( unitNames );
	table->setVerticalHeaderLabels( unitNames );
	table->setUnitIDs( unitIDs );


	// Load and Populate the data into the table
	UnitRatioList ratioList;
	database->loadUnitRatios( &ratioList, type );
	for ( UnitRatioList::const_iterator ratio_it = ratioList.constBegin(); ratio_it != ratioList.constEnd(); ++ratio_it ) {
		table->setRatio( ( *ratio_it ).unitId1(), ( *ratio_it ).unitId2(), ( *ratio_it ).ratio() );
	}
}

void UnitsDialog::saveRatio( int r, int c, double value )
{
	KreConversionTable *conversionTable = (KreConversionTable*)sender();
	UnitRatio ratio(conversionTable->getUnitID( r ),
	                conversionTable->getUnitID( c ),
	                value);
	database->saveUnitRatio( &ratio );

	UnitRatio reverse_ratio(ratio.reverse());
	database->saveUnitRatio( &reverse_ratio );
	conversionTable->setRatio( reverse_ratio );

#if 0
	UnitRatioList ratioList;
	database->loadUnitRatios( &ratioList, Unit::Mass );

	saveAllRatios( ratioList );
#endif
}

void UnitsDialog::removeRatio( int r, int c )
{
	KreConversionTable *conversionTable = (KreConversionTable*)sender();
	// Delete ratio from database.
	database->removeUnitRatio( conversionTable->getUnitID( r ), conversionTable->getUnitID( c ) );
	// Also delete the reverse ratio both from the database and the UI.
	database->removeUnitRatio( conversionTable->getUnitID( c ), conversionTable->getUnitID( r ) );
	conversionTable->deleteRatio( c, r );
}

void UnitsDialog::saveAllRatios( UnitRatioList &/*ratioList*/ )
{
#if 0
	ConversionTable *conversionTable = massConversionTable;

	KProgressDialog progress_dialog( this, "progress_dialog", i18nc( "@title:window", "Finding Unit Ratios" ), QString(), true );
	progress_dialog.progressBar() ->setTotalSteps( ratioList.count() * ratioList.count() );

	for ( UnitRatioList::const_iterator current_it = ratioList.begin(); current_it != ratioList.end(); ++current_it ) {
		UnitRatio current_ratio = *current_it;
		for ( UnitRatioList::const_iterator ratio_it = ratioList.begin(); ratio_it != ratioList.end(); ++ratio_it ) {
			if ( progress_dialog.wasCancelled() )
				return ;

			progress_dialog.progressBar() ->advance( 1 );
			kapp->processEvents();

			UnitRatio new_ratio;
			new_ratio.uID1 = current_ratio.uID1;
			new_ratio.uID2 = ( *ratio_it ).uID2;
			new_ratio.ratio = ( *ratio_it ).ratio * current_ratio.ratio;

			if ( ratioList.contains( new_ratio ) )
				continue;

			if ( ( ( *ratio_it ).uID1 == current_ratio.uID2 ) && ( ( *ratio_it ).uID2 != current_ratio.uID1 ) ) {
				UnitRatio reverse_ratio;
				reverse_ratio.uID1 = new_ratio.uID2;
				reverse_ratio.uID2 = new_ratio.uID1;
				reverse_ratio.ratio = 1.0 / new_ratio.ratio;

				database->saveUnitRatio( &new_ratio );
				database->saveUnitRatio( &reverse_ratio );
				conversionTable->setRatio( new_ratio );
				conversionTable->setRatio( reverse_ratio );
				//ratioList.append(new_ratio); ratioList.append(reverse_ratio);
			}
		}
	}
#endif
}

#include "unitsdialog.moc"
