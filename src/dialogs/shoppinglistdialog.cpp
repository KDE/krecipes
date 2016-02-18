/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "shoppinglistdialog.h"

#include "refineshoppinglistdialog.h"
#include "widgets/krerecipeslistwidget.h"
#include "widgets/kresearchresultlistwidget.h"
#include "widgets/recipelistview.h"
#include "actionshandlers/recipeactionshandler.h"
#include "backends/recipedb.h"
#include "datablocks/recipelist.h"

#include <KLocale>
#include <KPushButton>
#include <KVBox>

#include <QDropEvent>
#include <QGridLayout>
#include <QVBoxLayout>


ShoppingListDialog::ShoppingListDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{
	// Store pointer to database
	database = db;

	// Design dialog
	layout = new QGridLayout( this );
	layout->cellRect( 2, 2 );

	m_sourceListWidget = new KreRecipesListWidget( this, database );
	m_sourceListWidget->setListTitle( i18nc( "@title", "Full recipe list" ) );
	layout->addWidget( m_sourceListWidget, 0, 0 );

	QVBoxLayout* vboxl = new QVBoxLayout();
	vboxl->addStretch();
	addRecipeButton = new KPushButton( this );
	addRecipeButton->setIcon( KIcon( "arrow-right" ) );
	vboxl->addWidget( addRecipeButton );

	removeRecipeButton = new KPushButton( this );
	removeRecipeButton->setIcon( KIcon( "arrow-left" ) );
	vboxl->addWidget( removeRecipeButton );
	vboxl->addStretch();

	layout->addLayout( vboxl, 0, 1 );

	m_destListwidget = new KreSearchResultListWidget( this, database );
	m_destListwidget->setListTitle( i18nc("@title", "Shopping List") );
	layout->addWidget( m_destListwidget, 0, 2 );

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

	// Connect signals & slots
	connect( addRecipeButton, SIGNAL( clicked() ), this, SLOT( addRecipe() ) );
	connect( removeRecipeButton, SIGNAL( clicked() ), this, SLOT( removeRecipe() ) );
	connect( okButton, SIGNAL( clicked() ), this, SLOT( showShoppingList() ) );
	connect( clearButton, SIGNAL( clicked() ), this, SLOT( clear() ) );
}

ShoppingListDialog::~ShoppingListDialog()
{}

void ShoppingListDialog::createShopping( const RecipeList &rlist )
{
	clear();
	RecipeList::const_iterator it;
	for ( it = rlist.begin(); it != rlist.end(); ++it ) {
		new RecipeListItem( shopRecipeListView->listView(), shopRecipeListView->listView() ->lastItem(), *it );
	}
}

void ShoppingListDialog::reload( ReloadFlags flag )
{
	m_sourceListWidget->reload( flag );
}

void ShoppingListDialog::addRecipe( void )
{
	QList<RecipeDB::IdType> id_list;
	QList<RecipeDB::IdType>::iterator id_list_it;
	RecipeList recipe_list;

	id_list = m_sourceListWidget->selectedRecipes();
	for( id_list_it = id_list.begin(); id_list_it != id_list.end(); id_list_it++) {
		Recipe recipe;
		recipe.recipeID = *id_list_it;
		recipe.title = m_sourceListWidget->getRecipeName(*id_list_it);
		recipe_list << recipe;
	}

	m_destListwidget->displayRecipes( recipe_list );
}

void ShoppingListDialog::removeRecipe( void )
{
	QList<int> selected_rows = m_destListwidget->currentRows();

	QList<int>::iterator it;
	for ( it = selected_rows.begin(); it != selected_rows.end(); it ++)
	{
		m_destListwidget->model()->removeRow( *it );
	}
}

void ShoppingListDialog::showShoppingList( void )
{
	ElementList recipeList = m_destListwidget->displayedRecipes();

	QPointer<RefineShoppingListDialog> refineDialog = new RefineShoppingListDialog( this, database, recipeList );
	refineDialog->exec();
	delete refineDialog;
}

void ShoppingListDialog::addRecipeToShoppingList( int recipeID )
{
	Recipe r;
	r.title = database->recipeTitle( recipeID );
	r.recipeID = recipeID;

	RecipeList list;
	list << r;

	m_destListwidget->displayRecipes( list );
}

void ShoppingListDialog::clear()
{
	m_destListwidget->clear();
}

#include "shoppinglistdialog.moc"
