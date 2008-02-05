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
//Added by qt3to4:
#include <QDropEvent>
#include <Q3GridLayout>
#include <Q3PtrList>
#include <Q3VBoxLayout>

/** A simple listview to accept dropping a RecipeItemDrag */
class ShoppingListView : public K3ListView
{
public:
	ShoppingListView( QWidget *parent ) : K3ListView( parent )
	{}

protected:
	bool acceptDrag( QDropEvent *event ) const
	{
		return RecipeItemDrag::canDecode( event );
	}

	Q3DragObject *dragObject()
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
	layout = new Q3GridLayout( this, 2, 2, KDialog::marginHint(), KDialog::spacingHint() );

	recipeListView = new KreListView ( this, i18n( "Full recipe list" ), true, 1 );
	layout->addWidget( recipeListView, 0, 0 );
	listview = new RecipeListView( recipeListView, database );
	listview->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
	listview->setDragEnabled( true );
	listview->setAcceptDrops( true );
	listview->setDropVisualizer( false );
	connect( recipeListView, SIGNAL( textChanged(const QString&) ), SLOT( ensurePopulated() ) );
	connect( listview, SIGNAL( dropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ),
	         this, SLOT( slotDropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ) );
	recipeListView->setListView( listview );
	recipeListView->setCustomFilter( new RecipeFilter( recipeListView->listView() ), SLOT( filter( const QString & ) ) );
	recipeListView->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );

	Q3BoxLayout* vboxl = new Q3VBoxLayout( KDialog::spacingHint() );
	KIconLoader *il = KIconLoader::global();
	addRecipeButton = new QPushButton( this );
	addRecipeButton->setIconSet( il->loadIconSet( "go-next", KIcon::Small ) );
	addRecipeButton->setFixedSize( QSize( 32, 32 ) );
	addRecipeButton->setFlat( true );
	vboxl->addWidget( addRecipeButton );

	removeRecipeButton = new QPushButton( this );
	removeRecipeButton->setIconSet( il->loadIconSet( "go-previous", KIcon::Small ) );
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
	connect( slistview, SIGNAL( dropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ),
	         this, SLOT( slotDropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ) );
	shopRecipeListView->setListView( slistview );
	layout->addWidget( shopRecipeListView, 0, 2 );

	shopRecipeListView->listView() ->addColumn( i18n( "Recipe Title" ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readEntry( "ShowID", false );
	shopRecipeListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	shopRecipeListView->listView() ->setSorting( -1 );
	shopRecipeListView->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	shopRecipeListView->listView() ->setAllColumnsShowFocus( true );

	buttonBar = new Q3HBox( this, "buttonBar" );
	layout->addMultiCellWidget( buttonBar, 1, 1, 0, 2 );

	layout->setColStretch( 0, 1 );
	layout->setColStretch( 1, 0 );
	layout->setColStretch( 2, 1 );

	okButton = new QPushButton( buttonBar, "okButton" );
	okButton->setText( i18n( "&OK" ) );
	okButton->setIconSet( il->loadIcon( "ok", KIconLoader::NoGroup, 16 ) );

	//buttonBar->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton = new QPushButton( buttonBar, "clearButton" );
	clearButton->setText( i18n( "Clear" ) );
	clearButton->setIconSet( il->loadIcon( "edit-clear", KIconLoader::NoGroup, 16 ) );

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

void ShoppingListDialog::reloadRecipeList( ReloadFlags flag )
{
	( ( RecipeListView* ) recipeListView->listView() ) ->reload( flag );
}

void ShoppingListDialog::reload( ReloadFlags flag )
{
	reloadRecipeList ( flag ); // Missing: check if there's non-existing recipes in the list now, and if so, delete.
}

void ShoppingListDialog::addRecipe( void )
{
	Q3PtrList<Q3ListViewItem> items = recipeListView->listView()->selectedItems();

	Q3PtrListIterator<Q3ListViewItem> it(items);
	Q3ListViewItem *item;
	while ( (item = it.current()) != 0 ) {
		addRecipe( item );
		++it;
	}
}

void ShoppingListDialog::addRecipe( Q3ListViewItem *item )
{
	if ( item ) {
		if ( item->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) item;

			Recipe r;
			r.title = recipe_it->title();
			r.recipeID = recipe_it->recipeID();
			( void ) new RecipeListItem( shopRecipeListView->listView(), r );
		}
	}
}

void ShoppingListDialog::removeRecipe( void )
{
	Q3ListViewItem * it;
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

void ShoppingListDialog::slotDropped( K3ListView *list, QDropEvent *e, Q3ListViewItem * /*after*/ )
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
		Q3ListViewItemIterator list_it = Q3ListViewItemIterator( shopRecipeListView->listView() );
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
