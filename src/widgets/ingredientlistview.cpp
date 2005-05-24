/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
//Added by qt3to4:
#include <Q3ValueList>

IngredientCheckListItem::IngredientCheckListItem( IngredientCheckListView* qlv, const Element &ing ) : Q3CheckListItem( qlv, QString::null, Q3CheckListItem::CheckBox ),
	m_listview(qlv)
{
	// Initialize the ingredient data with the the property data
	ingStored = new Element();
	ingStored->id = ing.id;
	ingStored->name = ing.name;
}

IngredientCheckListItem::IngredientCheckListItem( IngredientCheckListView* qlv, Q3ListViewItem *after, const Element &ing ) : Q3CheckListItem( qlv, after, QString::null, Q3CheckListItem::CheckBox ),
	m_listview(qlv)
{
	// Initialize the ingredient data with the the property data
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
		return QString::null;
	}
}

void IngredientCheckListItem::stateChange( bool on )
{
	m_listview->stateChange(this,on);
}

IngredientListView::IngredientListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db, db->ingredientCount() )
{
	connect( database, SIGNAL( ingredientCreated( const Element & ) ), SLOT( checkCreateIngredient( const Element & ) ) );
	connect( database, SIGNAL( ingredientRemoved( int ) ), SLOT( removeIngredient( int ) ) );

	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
}

void IngredientListView::load( int limit, int offset )
{
	ElementList ingredientList;
	database->loadIngredients( &ingredientList, limit, offset );

	for ( ElementList::const_iterator ing_it = ingredientList.begin(); ing_it != ingredientList.end(); ++ing_it )
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
	addColumn( i18n( "Ingredient" ) );

	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 1, true );

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon( "filenew", KIcon::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::Key_C );
		kpop->insertItem( il->loadIcon( "editdelete", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), Qt::CTRL + Qt::Key_R );
		kpop->polish();

		delete il;

		connect( this, SIGNAL( contextMenu( KListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem* ) ), this, SLOT( modIngredient( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( Q3ListViewItem* ) ), this, SLOT( saveIngredient( Q3ListViewItem* ) ) );
	}
}

void StdIngredientListView::showPopup( KListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdIngredientListView::createNew()
{
	CreateElementDialog * elementDialog = new CreateElementDialog( this, i18n( "New Ingredient" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		if ( checkBounds( result ) )
			database->createNewIngredient( result ); // Create the new author in the database
	}
}

void StdIngredientListView::remove
	()
{
	Q3ListViewItem * it = currentItem();

	if ( it ) {
		int ingredientID = it->text( 1 ).toInt();

		ElementList dependingRecipes;
		database->findIngredientDependancies( ingredientID, &dependingRecipes );
		if ( dependingRecipes.isEmpty() )
			database->removeIngredient( ingredientID );
		else { // Need Warning!
			DependanciesDialog *warnDialog = new DependanciesDialog( 0, &dependingRecipes );
			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeIngredient( ingredientID );
			delete warnDialog;
		}
	}
}

void StdIngredientListView::rename()
{
	Q3ListViewItem * item = currentItem();

	if ( item )
		IngredientListView::rename( item, 0 );
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

void StdIngredientListView::modIngredient( Q3ListViewItem* i )
{
	if ( i )
		IngredientListView::rename( i, 0);
}

void StdIngredientListView::saveIngredient( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		reload(); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingIngredientByName( i->text( 0 ) );
	int ing_id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != ing_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This ingredient already exists.  Continuing will merge these two ingredients into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeIngredients( existing_id, ing_id );
				break;
			}
		default:
			reload();
			break; //we have to reload because the ingredient was renamed, and we need to reset it
		}
	}
	else {
		database->modIngredient( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
	}
}

bool StdIngredientListView::checkBounds( const QString &name )
{
	if ( name.length() > database->maxIngredientNameLength() ) {
		KMessageBox::error( this, QString( i18n( "Ingredient name cannot be longer than %1 characters." ) ).arg( database->maxIngredientNameLength() ) );
		return false;
	}

	return true;
}



IngredientCheckListView::IngredientCheckListView( QWidget *parent, RecipeDB *db ) : IngredientListView( parent, db )
{
	addColumn( i18n( "Ingredient" ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );
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

	for ( Q3ValueList<Element>::const_iterator ing_it = m_selections.begin(); ing_it != m_selections.end(); ++ing_it ) {
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
			m_selections.remove(it->ingredient());
	}
}

#include "ingredientlistview.moc"
