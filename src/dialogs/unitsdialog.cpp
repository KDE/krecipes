/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <qlayout.h>
#include <qtabwidget.h>

#include "unitsdialog.h"
#include "createelementdialog.h"
#include "dependanciesdialog.h"
#include "DBBackend/recipedb.h"
#include "editbox.h"
#include "conversiontable.h"
#include "widgets/unitlistview.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogress.h>

UnitsDialog::UnitsDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog
	QHBoxLayout* page_layout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

	QTabWidget *tabWidget = new QTabWidget( this );

	QWidget *unitTab = new QWidget( tabWidget );
	QHBoxLayout* layout = new QHBoxLayout( unitTab, KDialog::marginHint(), KDialog::spacingHint() );

	unitListView = new StdUnitListView( unitTab, database, true );
	unitListView->reload(); //load the initial data
	layout->addWidget( unitListView );

	QVBoxLayout* vboxl = new QVBoxLayout( KDialog::spacingHint() );
	newUnitButton = new QPushButton( unitTab );
	newUnitButton->setText( i18n( "Create ..." ) );
	newUnitButton->setFlat( true );
	newUnitButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	vboxl->addWidget( newUnitButton );

	removeUnitButton = new QPushButton( unitTab );
	removeUnitButton->setText( i18n( "Delete" ) );
	removeUnitButton->setFlat( true );
	removeUnitButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	vboxl->addWidget( removeUnitButton );
	vboxl->addStretch();
	layout->addLayout( vboxl );

	tabWidget->insertTab( unitTab, i18n( "Units" ) );

	conversionTable = new ConversionTable( tabWidget, 1, 1 );
	tabWidget->insertTab( conversionTable, i18n( "Unit Conversions" ) );

	page_layout->addWidget( tabWidget );

	// Connect signals & slots
	connect( newUnitButton, SIGNAL( clicked() ), unitListView, SLOT( createNew() ) );
	connect( removeUnitButton, SIGNAL( clicked() ), unitListView, SLOT( remove
		         () ) );
	connect( conversionTable, SIGNAL( ratioChanged( int, int, double ) ), this, SLOT( saveRatio( int, int, double ) ) );

	//TODO: I'm too lazy right now, so do a complete reload to keep in sync with db
	connect( database, SIGNAL( unitCreated( const Unit& ) ), this, SLOT( loadConversionTable() ) );
	connect( database, SIGNAL( unitRemoved( int ) ), this, SLOT( loadConversionTable() ) );

	//this is for the above TODO, but it still has some bugs to be worked out
	//connect(database,SIGNAL(unitCreated(const Element&)),conversionTable,SLOT(unitCreated(const Element&)));
	//connect(database,SIGNAL(unitRemoved(int)),conversionTable,SLOT(unitRemoved(int)));

	//Populate data into the table
	loadConversionTable();

	//FIXME: We've got some sort of build issue... we get undefined references to CreateElementDialog without this dummy code here
	CreateElementDialog d( this, "" );
}

UnitsDialog::~UnitsDialog()
{}


void UnitsDialog::loadUnitsList( void )
{
	unitListView->reload();
}

void UnitsDialog::reloadData( void )
{
	loadUnitsList();
	loadConversionTable();
}

void UnitsDialog::loadConversionTable( void )
{
	UnitList unitList;
	database->loadUnits( &unitList );
	QStringList unitNames;
	IDList unitIDs; // We need to store these in the table, so rows and cols are identified by unitID, not name.
	conversionTable->clear();
	for ( UnitList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it ) {
		unitNames.append( ( *unit_it ).name );
		int *newId = new int( ( *unit_it ).id ); // Create the new int element
		unitIDs.append( newId ); // append the element
	}

	// Resize the table
	conversionTable->resize( unitNames.count(), unitNames.count() );

	// Set the table labels, and id's
	conversionTable->setRowLabels( unitNames );
	conversionTable->setColumnLabels( unitNames );
	conversionTable->setUnitIDs( unitIDs );


	// Load and Populate the data into the table
	UnitRatioList ratioList;
	database->loadUnitRatios( &ratioList );
	for ( UnitRatioList::const_iterator ratio_it = ratioList.begin(); ratio_it != ratioList.end(); ratio_it++ ) {
		conversionTable->setRatio( ( *ratio_it ).uID1, ( *ratio_it ).uID2, ( *ratio_it ).ratio );
	}
}

void UnitsDialog::saveRatio( int r, int c, double value )
{
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
	database->loadUnitRatios( &ratioList );

	saveAllRatios( ratioList );
#endif
}

void UnitsDialog::saveAllRatios( UnitRatioList &ratioList )
{
#if 0
	KProgressDialog progress_dialog( this, "progress_dialog", i18n( "Finding Unit Ratios" ), QString::null, true );
	progress_dialog.progressBar() ->setTotalSteps( ratioList.count() * ratioList.count() );

	for ( UnitRatioList::const_iterator current_it = ratioList.begin(); current_it != ratioList.end(); current_it++ ) {
		UnitRatio current_ratio = *current_it;
		for ( UnitRatioList::const_iterator ratio_it = ratioList.begin(); ratio_it != ratioList.end(); ratio_it++ ) {
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

void UnitsDialog::reload( void )
{
	this->reloadData();
}

#include "unitsdialog.moc"
