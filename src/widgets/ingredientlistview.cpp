/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Copyright (C) 2008 by Montel Laurent <montel@kde.org                  *
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
#include <kmenu.h>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
//Added by qt3to4:
#include <QList>

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
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
}

void IngredientListView::init()
{
	connect( database, SIGNAL( ingredientCreated( const Element & ) ), SLOT( checkCreateIngredient( const Element & ) ) );
	connect( database, SIGNAL( ingredientRemoved( int ) ), SLOT( removeIngredient( int ) ) );
}

void IngredientListView::load( int limit, int offset )
{
	ElementList ingredientList;
	database->loadIngredients( &ingredientList, limit, offset );

	setTotalItems(ingredientList.count());

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

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );

		KIconLoader *il = KIconLoader::global();

		kpop = new KMenu( this );
		kpop->addAction( il->loadIcon( "document-new", KIconLoader::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::Key_C );
		kpop->addAction( il->loadIcon( "edit-delete", KIconLoader::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
		kpop->addAction( il->loadIcon( "edit-rename", KIconLoader::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( slotRename() ), Qt::CTRL + Qt::Key_R );
		kpop->ensurePolished();

		connect( this, SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem* ) ), this, SLOT( modIngredient( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( Q3ListViewItem* ) ), this, SLOT( saveIngredient( Q3ListViewItem* ) ) );
	}
}

void StdIngredientListView::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdIngredientListView::slotRename()
{
    rename( 0, 0);
}

void StdIngredientListView::createNew()
{
	CreateElementDialog * elementDialog = new CreateElementDialog( this, i18n( "New Ingredient" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		if ( checkBounds( result ) )
			database->createNewIngredient( result ); // Create the new author in the database
	}
        delete elementDialog;
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
			ListInfo list;
			list.list = dependingRecipes;
			list.name = i18n( "Recipes" );

			DependanciesDialog warnDialog( this, list );
			warnDialog.setCustomWarning( i18n("You are about to permanantly delete recipes from your database.") );
			if ( warnDialog.exec() == QDialog::Accepted )
				database->removeIngredient( ingredientID );
		}
	}
}

void StdIngredientListView::rename( Q3ListViewItem* /*item*/,int /*c*/ )
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
		reload(ForceReload); //reset the changed text
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
			reload(ForceReload);
			break; //we have to reload because the ingredient was renamed, and we need to reset it
		}
	}
	else {
		database->modIngredient( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
	}
}

bool StdIngredientListView::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxIngredientNameLength()) ) {
		KMessageBox::error( this,i18np( "Ingredient name cannot be longer than 1 character.", "Ingredient name cannot be longer than %1 characters." ,  database->maxIngredientNameLength() ) );
		return false;
	}

	return true;
}



IngredientCheckListView::IngredientCheckListView( QWidget *parent, RecipeDB *db ) : IngredientListView( parent, db )
{
	addColumn( i18n( "Ingredient" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
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
