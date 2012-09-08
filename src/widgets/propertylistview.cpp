/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "propertylistview.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kmenu.h>
#include <kdebug.h>
#include <QPointer>

#include "backends/recipedb.h"
#include "dialogs/createpropertydialog.h"

PropertyCheckListItem::PropertyCheckListItem( Q3ListView* klv, const IngredientProperty &property ) : Q3CheckListItem( klv, QString(), Q3CheckListItem::CheckBox ),
		m_property( property )
{
	//setOn( false ); // Set unchecked by default
}

PropertyCheckListItem::PropertyCheckListItem( Q3ListViewItem* it, const IngredientProperty &property ) : Q3CheckListItem( it, QString(), Q3CheckListItem::CheckBox ),
		m_property( property )
{
	//setOn( false ); // Set unchecked by default
}

QString PropertyCheckListItem::text( int column ) const
{
	switch ( column ) {
	case 0:
		return m_property.name;
		break;
	case 1:
		return m_property.units;
		break;
	case 2:
		return QString::number( m_property.id );
		break;

	}

	return QString();
}

void PropertyCheckListItem::setText ( int column, const QString & text )
{
	switch ( column ) {
	case 0:
		m_property.name = text;
		break;
	case 1:
		m_property.units = text;
		break;
	}
}

HidePropertyCheckListItem::HidePropertyCheckListItem( Q3ListView* klv, const IngredientProperty &property, bool enable ) : PropertyCheckListItem( klv, property )
{
	m_holdSettings = true;
	setOn( enable ); // Set checked by default
	m_holdSettings = false;
}

HidePropertyCheckListItem::HidePropertyCheckListItem( Q3ListViewItem* it, const IngredientProperty &property, bool enable ) : PropertyCheckListItem( it, property )
{
	m_holdSettings = true;
	setOn( enable ); // Set checked by default
	m_holdSettings = false;
}

void HidePropertyCheckListItem::stateChange( bool on )
{
	if ( !m_holdSettings ) {
		KConfigGroup config = KGlobal::config()->group("Formatting");

		config.sync();
		QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());
		if ( on )
			hiddenList.removeAll(m_property.name);
		else if ( !hiddenList.contains(m_property.name) )
			hiddenList.append(m_property.name);

		config.writeEntry("HiddenProperties",hiddenList);
	}
}

PropertyListView::PropertyListView( QWidget *parent, RecipeDB *db ) :
	DBListViewBase( parent, db, 0)
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );

	connect( db, SIGNAL( propertyCreated( const IngredientProperty & ) ), SLOT( createProperty( const IngredientProperty & ) ) );
	connect( db, SIGNAL( propertyRemoved( int ) ), SLOT( removeProperty( int ) ) );
}

void PropertyListView::reload()
{
	clear(); // Clear the view

	m_loading = true;

	IngredientPropertyList propertyList;
	database->loadProperties( &propertyList );

	//Populate this data into the K3ListView
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = propertyList.constBegin(); prop_it != propertyList.constEnd(); ++prop_it )
		createProperty( *prop_it );

	m_loading = false;
}

void PropertyListView::load(int, int)
{
	reload();
}


StdPropertyListView::StdPropertyListView( QWidget *parent, RecipeDB *db, bool editable ) : PropertyListView( parent, db )
{
	addColumn( i18nc( "@title:column", "Property" ) );
	addColumn( i18nc( "@title:column", "Units" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18nc( "@title:column", "Id" ) , show_id ? -1 : 2 );

	setSorting( 0 );

	if ( editable ) {
		setRenameable( 0, true );
		setRenameable( 1, true );
	}
}

void StdPropertyListView::removeProperty( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 2 );

	Q_ASSERT( item );

	delete item;
}

void StdPropertyListView::createProperty( const IngredientProperty &property )
{
	( void ) new Q3ListViewItem( this, property.name, property.units, QString::number( property.id ) );
}



PropertyConstraintListView::PropertyConstraintListView( QWidget *parent, RecipeDB *db ) : PropertyListView( parent, db )
{
	addColumn( i18nc( "@title:column", "Enabled" ) );
	addColumn( i18nc( "@title:column", "Property" ) );
	addColumn( i18nc( "@title:column", "Min. Value" ) );
	addColumn( i18nc( "@title:column", "Max. Value" ) );
	addColumn( "Id", 0 ); //hidden, only for internal purposes

	setRenameable( 0, true );
	setRenameable( 1, true );
}

void PropertyConstraintListView::removeProperty( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 4 );

	Q_ASSERT( item );

	delete item;
}

void PropertyConstraintListView::createProperty( const IngredientProperty &property )
{
	( void ) new ConstraintsListItem( this, property );
}


CheckPropertyListView::CheckPropertyListView( QWidget *parent, RecipeDB *db, bool editable ) : StdPropertyListView( parent, db, editable )
{
}

void CheckPropertyListView::createProperty( const IngredientProperty &property )
{
	( void ) new HidePropertyCheckListItem( this, property, (m_loading)?false:true );
}

#include "propertylistview.moc"
