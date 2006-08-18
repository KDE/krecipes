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

#include "selectunitdialog.h"

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

SelectUnitDialog::SelectUnitDialog( QWidget* parent, const UnitList &unitList, OptionFlag showEmpty )
		: KDialogBase( parent, "SelectUnitDialog", true, i18n( "Choose Unit" ),
		    KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok ), m_showEmpty(showEmpty)
{
	QVBox *page = makeVBoxMainWidget();

	box = new QGroupBox( page );
	box->setTitle( i18n( "Choose Unit" ) );
	box->setColumnLayout( 0, Qt::Vertical );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );

	unitChooseView = new KListView( box );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	unitChooseView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	unitChooseView->addColumn( i18n( "Unit" ) );
	unitChooseView->setSorting(1);
	unitChooseView->setGeometry( QRect( 5, 30, 180, 250 ) );
	unitChooseView->setAllColumnsShowFocus( true );
	boxLayout->addWidget( unitChooseView );

	resize( QSize( 200, 350 ) );

	loadUnits( unitList );
}


SelectUnitDialog::~SelectUnitDialog()
{}

int SelectUnitDialog::unitID( void )
{

	QListViewItem * it;
	if ( ( it = unitChooseView->selectedItem() ) ) {
		return ( it->text( 0 ).toInt() );
	}
	else
		return ( -1 );
}

void SelectUnitDialog::loadUnits( const UnitList &unitList )
{
	for ( UnitList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it ) {
		QString unitName = ( *unit_it ).name;
		if ( unitName.isEmpty() ) {
			if ( m_showEmpty == ShowEmptyUnit )
				unitName = " "+i18n("-No unit-");
			else
				continue;
		}

		( void ) new QListViewItem( unitChooseView, QString::number( ( *unit_it ).id ), unitName );
	}
}

