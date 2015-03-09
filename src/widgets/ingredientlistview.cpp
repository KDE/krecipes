/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2008 Montel Laurent <montel@kde.org>                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmenu.h>
#include <QPointer>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
//Added by qt3to4:
#include <QList>

IngredientCheckListItem::IngredientCheckListItem( IngredientCheckListView* qlv, const Element &ing ) : Q3CheckListItem( qlv, QString(), Q3CheckListItem::CheckBox ),
	m_listview(qlv)
{
	// Initialize the ingredient data with the property data
	ingStored = new Element();
	ingStored->id = ing.id;
	ingStored->name = ing.name;
}

IngredientCheckListItem::IngredientCheckListItem( IngredientCheckListView* qlv, Q3ListViewItem *after, const Element &ing ) : Q3CheckListItem( qlv, after, QString(), Q3CheckListItem::CheckBox ),
	m_listview(qlv)
{
	// Initialize the ingredient data with the property data
	ingStored = new Element();
	ingStored->id = ing.id;
	ingStored->name = ing.name;
}

IngredientCheckListItem::~IngredientCheckListItem( void )
{
	delete ingStored;
}
int IngredientCheckListItem::id( void ) const
{
	return ingStored->id;
}
QString IngredientCheckListItem::name( void ) const
{
	return ingStored->name;
}
Element IngredientCheckListItem::ingredient() const
{
	return *ingStored;
}

QString IngredientCheckListItem::text( int column ) const
{
	switch ( column ) {
	case 0:
		return ( ingStored->name );
	case 1:
		return ( QString::number( ingStored->id ) );
	default:
		return QString();
	}
}

void IngredientCheckListItem::stateChange( bool on )
{
	m_listview->stateChange(this,on);
}

IngredientListView::IngredientListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db, db->ingredientCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
	connect( database, SIGNAL( ingredientCreated( const Element & ) ), SLOT( checkCreateIngredient( const Element & ) ) );
	connect( database, SIGNAL( ingredientRemoved( int ) ), SLOT( removeIngredient( int ) ) );
}

void IngredientListView::load( int limit, int offset )
{
	ElementList ingredientList;
	database->loadIngredients( &ingredientList, limit, offset );

	setTotalItems(ingredientList.count());

	for ( ElementList::const_iterator ing_it = ingredientList.constBegin(); ing_it != ingredientList.constEnd(); ++ing_it )
		createIngredient( *ing_it );
}

void IngredientListView::checkCreateIngredient( const Element &el )
{
	if ( handleElement(el.name) ) { //only create this ingredient if the base class okays it
		createIngredient(el);
	}
}


StdIngredientListView::StdIngredientListView( QWidget *parent, RecipeDB *db, bool editable ) : IngredientListView( parent, db )
{
	addColumn( i18nc( "@title:column", "Ingredient" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18nc( "@title:column", "Id" ) , show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
	}
}

void StdIngredientListView::createIngredient( const Element &ing )
{
	createElement(new Q3ListViewItem( this, ing.name, QString::number( ing.id ) ));
}

void StdIngredientListView::removeIngredient( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}


IngredientCheckListView::IngredientCheckListView( QWidget *parent, RecipeDB *db ) : IngredientListView( parent, db )
{
	addColumn( i18nc( "@title:column", "Ingredient" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18nc( "@title:column", "Id" ) , show_id ? -1 : 0 );
}

void IngredientCheckListView::createIngredient( const Element &ing )
{
	createElement(new IngredientCheckListItem( this, ing ));
}

void IngredientCheckListView::removeIngredient( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

void IngredientCheckListView::load( int limit, int offset )
{
	IngredientListView::load(limit,offset);

	for ( QList<Element>::const_iterator ing_it = m_selections.constBegin(); ing_it != m_selections.constEnd(); ++ing_it ) {
		Q3CheckListItem * item = ( Q3CheckListItem* ) findItem( QString::number( (*ing_it).id ), 1 );
		if ( item ) {
			item->setOn(true);
		}
	}
}

void IngredientCheckListView::stateChange(IngredientCheckListItem *it,bool on)
{
	if ( !reloading() ) {
		if ( on )
			m_selections.append(it->ingredient());
		else
			m_selections.removeAll(it->ingredient());
	}
}

#include "ingredientlistview.moc"
