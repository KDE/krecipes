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

#include "shoppinglistdialog.h"

#include <klocale.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kapplication.h>
#include <KConfigGroup>

#include "backends/recipedb.h"
#include "refineshoppinglistdialog.h"
#include "datablocks/recipelist.h"
#include "widgets/recipelistview.h"
#include "recipefilter.h"
#include "actionshandlers/recipeactionshandler.h"
#include <QDropEvent>
#include <QGridLayout>
#include <QVBoxLayout>
#include <kvbox.h>

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
	layout = new QGridLayout( this );
	layout->cellRect( 2, 2 );

	recipeListView = new KreListView ( this, i18nc( "@title", "Full recipe list" ), true, 1 );
	layout->addWidget( recipeListView, 0, 0 );
	listview = new RecipeListView( recipeListView, database );
	listview->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
	listview->setDragEnabled( true );
	listview->setAcceptDrops( true );
	listview->setDropVisualizer( false );

	//FIXME: Connecting to ensurePopulated here is so disgusting, RecipeFilter should handle it.
	KConfigGroup config (KGlobal::config(), "Performance" );
	if ( config.readEntry("SearchAsYouType",true) )
		connect( recipeListView, SIGNAL( textChanged(const QString&) ), SLOT( ensurePopulated() ) );
	else
		connect( recipeListView, SIGNAL( returnPressed(const QString&) ), SLOT( ensurePopulated() ) );

	connect( listview, SIGNAL( dropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ),
		      this, SLOT( slotDropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ) );
	recipeListView->setListView( listview );
	recipeListView->setCustomFilter( new RecipeFilter( recipeListView->listView() ), SLOT( filter( const QString & ) ) );

	QVBoxLayout* vboxl = new QVBoxLayout();
	vboxl->addStretch();
	addRecipeButton = new KPushButton( this );
	addRecipeButton->setIcon( KIcon( "arrow-right" ) );
	addRecipeButton->setFixedSize( QSize( 32, 32 ) );
	vboxl->addWidget( addRecipeButton );

	removeRecipeButton = new KPushButton( this );
	removeRecipeButton->setIcon( KIcon( "arrow-left" ) );
	removeRecipeButton->setFixedSize( QSize( 32, 32 ) );
	vboxl->addWidget( removeRecipeButton );
	vboxl->addStretch();

	layout->addLayout( vboxl, 0, 1 );

	shopRecipeListView = new KreListView ( this, i18nc("@title", "Shopping List") );
	ShoppingListView *slistview = new ShoppingListView( shopRecipeListView );
	slistview->setDragEnabled( true );
	slistview->setAcceptDrops( true );
	slistview->setDropVisualizer( false );
	connect( slistview, SIGNAL( dropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ),
		      this, SLOT( slotDropped( K3ListView*, QDropEvent*, Q3ListViewItem* ) ) );
	shopRecipeListView->setListView( slistview );
	layout->addWidget( shopRecipeListView, 0, 2 );

	shopRecipeListView->listView() ->addColumn( i18nc( "@title:column", "Recipe Title" ) );

	config.changeGroup( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	shopRecipeListView->listView() ->addColumn( "Id" , show_id ? -1 : 0 );

	shopRecipeListView->listView() ->setSorting( -1 );
	shopRecipeListView->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
	shopRecipeListView->listView() ->setAllColumnsShowFocus( true );

	buttonBar = new KHBox( this );
	layout->addWidget( buttonBar, 1, 1, 1, 2, 0 );

	layout->setColumnStretch( 0, 1 );
	layout->setColumnStretch( 1, 0 );
	layout->setColumnStretch( 2, 1 );

	okButton = new KPushButton( buttonBar );
	okButton->setObjectName( "okButton" );
	okButton->setText( i18nc( "@action:button", "&OK" ) );
	okButton->setIcon( KIcon( "dialog-ok" ) );

	//buttonBar->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton = new KPushButton( buttonBar );
	clearButton->setObjectName( "clearButton" );
	clearButton->setText( i18nc( "@action:button Clear list", "Clear" ) );
	clearButton->setIcon( KIcon( "edit-clear" ) );

	//Takes care of all recipe actions and provides a popup menu to 'recipeListView'
	actionHandler = new RecipeActionsHandler( recipeListView->listView(), database );

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
	QList<Q3ListViewItem*> items = recipeListView->listView()->selectedItems();

	     for ( int i = 0;i <items.count();i++ )
	     {
	         addRecipe( items.at( i ) );

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

	QPointer<RefineShoppingListDialog> refineDialog = new RefineShoppingListDialog( this, database, recipeList );
	refineDialog->exec();
	delete refineDialog;
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
