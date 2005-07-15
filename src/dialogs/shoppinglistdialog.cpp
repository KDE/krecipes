/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "shoppinglistdialog.h"

#include <klocale.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kapplication.h>

#include "backends/recipedb.h"
#include "refineshoppinglistdialog.h"
#include "datablocks/recipelist.h"
#include "widgets/recipelistview.h"
#include "recipefilter.h"
#include "recipeactionshandler.h"

/** A simple listview to accept dropping a RecipeItemDrag */
class ShoppingListView : public KListView
{
public:
	ShoppingListView( QWidget *parent ) : KListView( parent )
	{}

protected:
	bool acceptDrag( QDropEvent *event ) const
	{
		return RecipeItemDrag::canDecode( event );
	}

	QDragObject *dragObject()
	{
		RecipeListItem * item = dynamic_cast<RecipeListItem*>( selectedItem() );
		if ( item != 0 ) {

			RecipeItemDrag * obj = new RecipeItemDrag( item, this, "Recipe drag item" );
			/*const QPixmap *pm = item->pixmap(0);
			if( pm )
				obj->setPixmap( *pm );*/ 
			return obj;
		}
		return 0;
	}
};


ShoppingListDialog::ShoppingListDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{
	// Store pointer to database
	database = db;

	// Design dialog
	layout = new QGridLayout( this, 2, 2, KDialog::marginHint(), KDialog::spacingHint() );

	recipeListView = new KreListView ( this, i18n( "Full recipe list" ), true, 1 );
	layout->addWidget( recipeListView, 0, 0 );
	listview = new RecipeListView( recipeListView, database );
	listview->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
	listview->setDragEnabled( true );
	listview->setAcceptDrops( true );
	listview->setDropVisualizer( false );
	connect( recipeListView, SIGNAL( textChanged(const QString&) ), SLOT( ensurePopulated() ) );
	connect( listview, SIGNAL( dropped( KListView*, QDropEvent*, QListViewItem* ) ),
	         this, SLOT( slotDropped( KListView*, QDropEvent*, QListViewItem* ) ) );
	listview->reload();
	recipeListView->setListView( listview );
	recipeListView->setCustomFilter( new RecipeFilter( recipeListView->listView() ), SLOT( filter( const QString & ) ) );
	recipeListView->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );

	QBoxLayout* vboxl = new QVBoxLayout( KDialog::spacingHint() );
	KIconLoader il;
	addRecipeButton = new QPushButton( this );
	addRecipeButton->setIconSet( il.loadIconSet( "forward", KIcon::Small ) );
	addRecipeButton->setFixedSize( QSize( 32, 32 ) );
	addRecipeButton->setFlat( true );
	vboxl->addWidget( addRecipeButton );

	removeRecipeButton = new QPushButton( this );
	removeRecipeButton->setIconSet( il.loadIconSet( "back", KIcon::Small ) );
	removeRecipeButton->setFixedSize( QSize( 32, 32 ) );
	removeRecipeButton->setFlat( true );
	vboxl->addWidget( removeRecipeButton );
	vboxl->addStretch();

	layout->addItem( vboxl, 0, 1 );

	shopRecipeListView = new KreListView ( this, i18n("Shopping List") );
	ShoppingListView *slistview = new ShoppingListView( shopRecipeListView );
	slistview->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
	slistview->setDragEnabled( true );
	slistview->setAcceptDrops( true );
	slistview->setDropVisualizer( false );
	connect( slistview, SIGNAL( dropped( KListView*, QDropEvent*, QListViewItem* ) ),
	         this, SLOT( slotDropped( KListView*, QDropEvent*, QListViewItem* ) ) );
	shopRecipeListView->setListView( slistview );
	layout->addWidget( shopRecipeListView, 0, 2 );

	shopRecipeListView->listView() ->addColumn( i18n( "Recipe Title" ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	shopRecipeListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	shopRecipeListView->listView() ->setSorting( -1 );
	shopRecipeListView->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	shopRecipeListView->listView() ->setAllColumnsShowFocus( true );

	buttonBar = new QHBox( this, "buttonBar" );
	layout->addMultiCellWidget( buttonBar, 1, 1, 0, 2 );

	layout->setColStretch( 0, 1 );
	layout->setColStretch( 1, 0 );
	layout->setColStretch( 2, 1 );

	okButton = new QPushButton( buttonBar, "okButton" );
	okButton->setText( i18n( "&OK" ) );
	QPixmap pm = il.loadIcon( "ok", KIcon::NoGroup, 16 );
	okButton->setIconSet( pm );

	//buttonBar->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton = new QPushButton( buttonBar, "clearButton" );
	clearButton->setText( i18n( "Clear" ) );
	pm = il.loadIcon( "editclear", KIcon::NoGroup, 16 );
	clearButton->setIconSet( pm );

	//Takes care of all recipe actions and provides a popup menu to 'recipeListView'
	actionHandler = new RecipeActionsHandler( recipeListView->listView(), database, RecipeActionsHandler::ExpandAll | RecipeActionsHandler::CollapseAll );

	// Connect signals & slots
	connect( addRecipeButton, SIGNAL( clicked() ), this, SLOT( addRecipe() ) );
	connect( removeRecipeButton, SIGNAL( clicked() ), this, SLOT( removeRecipe() ) );
	connect( okButton, SIGNAL( clicked() ), this, SLOT( showShoppingList() ) );
	connect( clearButton, SIGNAL( clicked() ), this, SLOT( clear() ) );
}

ShoppingListDialog::~ShoppingListDialog()
{}

void ShoppingListDialog::ensurePopulated()
{
	listview->populateAll();
}

void ShoppingListDialog::createShopping( const RecipeList &rlist )
{
	clear();
	RecipeList::const_iterator it;
	for ( it = rlist.begin(); it != rlist.end(); ++it ) {
		new RecipeListItem( shopRecipeListView->listView(), shopRecipeListView->listView() ->lastItem(), *it );
	}
}

void ShoppingListDialog::reloadRecipeList( void )
{
	( ( RecipeListView* ) recipeListView->listView() ) ->reload();
}

void ShoppingListDialog::reload( void )
{
	reloadRecipeList (); // Missing: check if there's non-existing recipes in the list now, and if so, delete.
}

void ShoppingListDialog::addRecipe( void )
{
	QListViewItem * it = recipeListView->listView() ->selectedItem();
	addRecipe( it );
}

void ShoppingListDialog::addRecipe( QListViewItem *item )
{
	if ( item ) {
		if ( item->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) item;

			Recipe r;
			r.title = recipe_it->title();
			r.recipeID = recipe_it->recipeID();
			( void ) new RecipeListItem( shopRecipeListView->listView(), r );
		}
		else if ( item->rtti() == 1001 ) { //add everything in the category
			//do this to only iterate over children of 'item'
			QListViewItem * pEndItem = NULL;
			QListViewItem *pStartItem = item;
			do {
				if ( pStartItem->nextSibling() )
					pEndItem = pStartItem->nextSibling();
				else
					pStartItem = pStartItem->parent();
			}
			while ( pStartItem && !pEndItem );

			QListViewItemIterator list_it = QListViewItemIterator( item );
			while ( list_it.current() != pEndItem ) {
				if ( list_it.current() ->rtti() == 1000 && list_it.current() ->isVisible() ) {
					RecipeListItem * recipe_it = ( RecipeListItem* ) list_it.current();
					Recipe r;
					r.title = recipe_it->title();
					r.recipeID = recipe_it->recipeID();
					( void ) new RecipeListItem( shopRecipeListView->listView(), r );
				}
				list_it++;
			}
		}
	}
}

void ShoppingListDialog::removeRecipe( void )
{
	QListViewItem * it;
	it = shopRecipeListView->listView() ->selectedItem();
	if ( it )
		delete it;
}

void ShoppingListDialog::showShoppingList( void )
{
	// Store the recipe list in ElementList object first
	ElementList recipeList;
	RecipeListItem *it;
	for ( it = ( RecipeListItem* ) shopRecipeListView->listView() ->firstChild();it;it = ( RecipeListItem* ) it->nextSibling() ) {
		Element newEl;
		newEl.id = it->recipeID();
		newEl.name = it->title(); // Storing the title is not necessary, but do it just in case it's used later on
		recipeList.append( newEl );
	}

	RefineShoppingListDialog refineDialog( this, database, recipeList );
	refineDialog.exec();
}

void ShoppingListDialog::addRecipeToShoppingList( int recipeID )
{
	Recipe r;
	r.title = database->recipeTitle( recipeID );
	r.recipeID = recipeID;

	new RecipeListItem( shopRecipeListView->listView(), r );
}

void ShoppingListDialog::clear()
{
	shopRecipeListView->listView() ->clear();
}

void ShoppingListDialog::slotDropped( KListView *list, QDropEvent *e, QListViewItem * /*after*/ )
{
	Recipe r;
	RecipeListItem *item = new RecipeListItem( recipeListView->listView(), r ); // needs parent, use this temporarily
	if ( !RecipeItemDrag::decode( e, *item ) ) {
		delete item;
		return ;
	}

	if ( list == shopRecipeListView->listView() ) {
		addRecipe( item );
	}
	//find and delete the item if we just dropped onto the recipe list from the shopping list
	else if ( list == recipeListView->listView() && e->source() == shopRecipeListView->listView() ) {
		QListViewItemIterator list_it = QListViewItemIterator( shopRecipeListView->listView() );
		while ( list_it.current() ) {
			if ( ( ( RecipeListItem* ) list_it.current() ) ->recipeID() == item->recipeID() ) {
				delete list_it.current();
				break;
			}
			list_it++;
		}
	}

	delete item;
	item = 0; // not needed anymore
}

#include "shoppinglistdialog.moc"
