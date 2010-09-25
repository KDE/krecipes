/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>           *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "unitlistview.h"

#include <KComboBox>
#include <q3header.h>
//Added by qt3to4:
#include <QList>

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmenu.h>
#include <kdebug.h>
#include <QPointer>

#include "backends/recipedb.h"
#include "dialogs/createunitdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "datablocks/unit.h"


UnitListView::UnitListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->unitCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
	connect( database, SIGNAL( unitCreated( const Unit & ) ), SLOT( checkCreateUnit( const Unit & ) ) );
	connect( database, SIGNAL( unitRemoved( int ) ), SLOT( removeUnit( int ) ) );
}

void UnitListView::load( int limit, int offset )
{
	UnitList unitList;
	database->loadUnits( &unitList, Unit::All, limit, offset );

	for ( UnitList::const_iterator it = unitList.constBegin(); it != unitList.constEnd(); ++it ) {
		if ( !( *it ).name().isEmpty() || !( *it ).plural().isEmpty() )
			createUnit( *it );
	}
}

void UnitListView::checkCreateUnit( const Unit &el )
{
	if ( handleElement(el.name()) ) { //only create this unit if the base class okays it
		createUnit(el);
	}
}


StdUnitListView::StdUnitListView( QWidget *parent, RecipeDB *db, bool editable ) : UnitListView( parent, db )
{
	addColumn( i18nc( "@title:column Unit name", "Unit" ) );
	addColumn( i18nc( "@title:column Unit abbreviation", "Abbreviation" ) );
	addColumn( i18nc( "@title:column Unit plural", "Plural" ) );
	addColumn( i18nc( "@title:column unit plural abbreviation", "Abbreviation" ) );
	addColumn( i18nc( "@title:column Unit type", "Type" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( "Id", show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
		setRenameable( 1, true );
		setRenameable( 2, true );
		setRenameable( 3, true );
		setRenameable( 4, true );

		typeComboBox = new KComboBox( viewport() );
		typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type other", "Other"));
		typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type mass", "Mass"));
		typeComboBox->insertItem( typeComboBox->count(), i18nc("@item:inlistbox Unit type volume", "Volume"));
		addChild( typeComboBox );
		typeComboBox->hide();

		connect( typeComboBox, SIGNAL( activated(int) ), SLOT( updateType(int) ) );
		connect( this, SIGNAL( selectionChanged() ), SLOT( hideTypeCombo() ) );
	}
}

void StdUnitListView::insertTypeComboBox( Q3ListViewItem* it )
{
	QRect r;

	// Constraints Box1
	r = header() ->sectionRect( 4 ); //start at the section 2 header
	r.translate( 0, itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height
	r.setWidth( header() ->sectionRect( 4 ).width() ); // and width
	typeComboBox->setGeometry( r );

	UnitListViewItem *unit_it = (UnitListViewItem*)it;
	typeComboBox->setCurrentIndex( unit_it->unit().type() );

	typeComboBox->show();
}

void StdUnitListView::updateType( int type )
{
	UnitListViewItem *unit_it = (UnitListViewItem*)currentItem();
	unit_it->setType((Unit::Type)type);

	database->modUnit( unit_it->unit() );
}

void StdUnitListView::hideTypeCombo()
{
	typeComboBox->hide();
}

void StdUnitListView::createUnit( const Unit &unit )
{
	createElement(new UnitListViewItem( this, unit ));
}

void StdUnitListView::removeUnit( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 5 );
	removeElement(item);
}

#include "unitlistview.moc"
