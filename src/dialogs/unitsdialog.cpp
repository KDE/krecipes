/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <qlayout.h>
#include <qtabwidget.h>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "unitsdialog.h"
#include "createelementdialog.h"
#include "dependanciesdialog.h"
#include "backends/recipedb.h"
#include "widgets/conversiontable.h"
#include "widgets/unitlistview.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogressdialog.h>

UnitsDialog::UnitsDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog
	QHBoxLayout* page_layout = new QHBoxLayout( this );
   page_layout->setMargin( KDialog::marginHint() );
   page_layout->setSpacing( KDialog::spacingHint() );

	QTabWidget *tabWidget = new QTabWidget( this );

	QWidget *unitTab = new QWidget( tabWidget );
	QHBoxLayout* layout = new QHBoxLayout( unitTab );
   layout->setMargin( KDialog::marginHint() );
   layout->setSpacing( KDialog::spacingHint() );

	unitListView = new StdUnitListView( unitTab, database, true );
	layout->addWidget( unitListView );

	QVBoxLayout* vboxl = new QVBoxLayout( KDialog::spacingHint() );
	newUnitButton = new QPushButton( unitTab );
	newUnitButton->setText( i18n( "Create ..." ) );
	newUnitButton->setFlat( true );
	vboxl->addWidget( newUnitButton );

	removeUnitButton = new QPushButton( unitTab );
	removeUnitButton->setText( i18n( "Delete" ) );
	removeUnitButton->setFlat( true );
	vboxl->addWidget( removeUnitButton );
	vboxl->addStretch();
	layout->addLayout( vboxl );

	tabWidget->insertTab( unitTab, i18n( "Units" ) );

	massConversionTable = new ConversionTable( tabWidget, 1, 1 );
	tabWidget->insertTab( massConversionTable, i18n( "Mass Conversions" ) );

	volumeConversionTable = new ConversionTable( tabWidget, 1, 1 );
	tabWidget->insertTab( volumeConversionTable, i18n( "Volume Conversions" ) );

	page_layout->addWidget( tabWidget );

	// Connect signals & slots
	connect( newUnitButton, SIGNAL( clicked() ), unitListView, SLOT( createNew() ) );
	connect( removeUnitButton, SIGNAL( clicked() ), unitListView, SLOT( remove() ) );
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

	//FIXME: We've got some sort of build issue... we get undefined references to CreateElementDialog without this dummy code here
	CreateElementDialog d( this, "" );
}

UnitsDialog::~UnitsDialog()
{}

void UnitsDialog::reload( ReloadFlags flag )
{
	unitListView->reload( flag );
	loadConversionTables();
}

void UnitsDialog::loadConversionTables( void )
{
	loadConversionTable( massConversionTable, Unit::Mass );
	loadConversionTable( volumeConversionTable, Unit::Volume );
}

void UnitsDialog::loadConversionTable( ConversionTable *table, Unit::Type type )
{
	UnitList unitList;
	database->loadUnits( &unitList, type );

	QStringList unitNames;
	IDList unitIDs; // We need to store these in the table, so rows and cols are identified by unitID, not name.
	table->clear();
	for ( UnitList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it ) {
		unitNames.append( ( *unit_it ).name );
		unitIDs.append( ( *unit_it ).id ); // append the element
	}

	// Resize the table
	table->resize( unitNames.count(), unitNames.count() );

	// Set the table labels, and id's
	table->setRowLabels( unitNames );
	table->setColumnLabels( unitNames );
	table->setUnitIDs( unitIDs );


	// Load and Populate the data into the table
	UnitRatioList ratioList;
	database->loadUnitRatios( &ratioList, type );
	for ( UnitRatioList::const_iterator ratio_it = ratioList.begin(); ratio_it != ratioList.end(); ++ratio_it ) {
		table->setRatio( ( *ratio_it ).uID1, ( *ratio_it ).uID2, ( *ratio_it ).ratio );
	}
}

void UnitsDialog::saveRatio( int r, int c, double value )
{
	ConversionTable *conversionTable = (ConversionTable*)sender();
	UnitRatio ratio;

	ratio.uID1 = conversionTable->getUnitID( r );
	ratio.uID2 = conversionTable->getUnitID( c );
	ratio.ratio = value;
	database->saveUnitRatio( &ratio );

	UnitRatio reverse_ratio;
	reverse_ratio.uID1 = ratio.uID2;
	reverse_ratio.uID2 = ratio.uID1;
	reverse_ratio.ratio = 1.0 / ratio.ratio;
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
	ConversionTable *conversionTable = (ConversionTable*)sender();
	database->removeUnitRatio( conversionTable->getUnitID( r ), conversionTable->getUnitID( c ) );
}

void UnitsDialog::saveAllRatios( UnitRatioList &/*ratioList*/ )
{
#if 0
	ConversionTable *conversionTable = massConversionTable;

	KProgressDialog progress_dialog( this, "progress_dialog", i18n( "Finding Unit Ratios" ), QString::null, true );
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
