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

#include "selectunitdialog.h"

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kvbox.h>
#include <QVBoxLayout>

SelectUnitDialog::SelectUnitDialog( QWidget* parent, const UnitList &unitList, OptionFlag showEmpty )
		: KDialog( parent), m_showEmpty(showEmpty)
{
	setButtons(  KDialog::Ok | KDialog::Cancel );
	setDefaultButton( KDialog::Ok );
	setCaption(i18nc( "@title:window", "Choose Unit" ) );
	setModal( true );
	KVBox *page = new KVBox(this );
	setMainWidget( page );

	box = new Q3GroupBox( page );
	box->setTitle( i18nc( "@title:group", "Choose Unit" ) );
	box->setColumnLayout( 0, Qt::Vertical );
	QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );

	unitChooseView = new K3ListView( box );

	KConfigGroup config( KGlobal::config(), "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	unitChooseView->addColumn( "Id" , show_id ? -1 : 0 );

	unitChooseView->addColumn( i18nc( "@title:column", "Unit" ) );
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

	Q3ListViewItem * it;
	if ( ( it = unitChooseView->selectedItem() ) ) {
		return ( it->text( 0 ).toInt() );
	}
	else
		return ( -1 );
}

void SelectUnitDialog::loadUnits( const UnitList &unitList )
{
	for ( UnitList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it ) {
		QString unitName = ( *unit_it ).name();
		if ( unitName.isEmpty() ) {
			if ( m_showEmpty == ShowEmptyUnit )
				unitName = ' '+i18nc("@item", "-No unit-");
			else
				continue;
		}

		( void ) new Q3ListViewItem( unitChooseView, QString::number( ( *unit_it ).id() ), unitName );
	}
}

