/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "selectrecipedialog.h"

#include <KTabWidget>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kvbox.h>
#include <KPushButton>

#include "advancedsearchdialog.h"
#include "datablocks/categorytree.h"
#include "backends/recipedb.h"
#include "datablocks/recipe.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"
#include "widgets/krerecipeslistwidget.h"
#include "widgets/categorylistview.h"
#include "widgets/categorycombobox.h"
#include "actionshandlers/krerecipeactionshandler.h"


BasicSearchTab::BasicSearchTab( QWidget * parent )
	: QFrame(parent), actionHandler(0)
{
}

void BasicSearchTab::setActionsHandler( KreRecipeActionsHandler * actionHandler )
{
	this->actionHandler = actionHandler;
}

void BasicSearchTab::showEvent( QShowEvent * /*event*/ )
{
	if ( actionHandler )
		actionHandler->selectionChangedSlot();
}

SelectRecipeDialog::SelectRecipeDialog( QWidget *parent, RecipeDB* db )
		: QWidget( parent )
{
	//Store pointer to Recipe Database
	database = db;

	QVBoxLayout *tabLayout = new QVBoxLayout( this );
	tabWidget = new KTabWidget( this );
	tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	tabLayout->addWidget( tabWidget );

	basicSearchTab = new BasicSearchTab( this );
	basicSearchTab->setFrameStyle( QFrame::NoFrame );
	//basicSearchTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	//Design dialog

	layout = new QVBoxLayout;
	basicSearchTab->setLayout( layout );

	recipeListWidget = new KreRecipesListWidget( basicSearchTab, database );
	layout->addWidget( recipeListWidget );

	buttonBar = new KHBox( basicSearchTab );
	layout->addWidget( buttonBar );

	openButton = new KPushButton( buttonBar );
	openButton->setText( i18nc( "@action:button", "Show Recipe(s)" ) );
	openButton->setDisabled( true );
	openButton->setIcon( KIcon( "system-search" ) );
	editButton = new KPushButton( buttonBar );
	editButton->setText( i18nc( "@action:button", "Edit Recipe" ) );
	editButton->setDisabled( true );
	editButton->setIcon( KIcon( "document-edit" ) );
	removeButton = new KPushButton( buttonBar );
	removeButton->setText( i18nc( "@action:button", "Delete" ) );
	removeButton->setDisabled( true );
	removeButton->setMaximumWidth( 100 );
	removeButton->setIcon( KIcon("edit-delete-shred" ) );

	tabWidget->insertTab( -1, basicSearchTab, i18nc( "@title:tab Basic search", "Basic" ) );

	advancedSearch = new AdvancedSearchDialog( this, database );
	tabWidget->insertTab( -1, advancedSearch, i18nc( "@title:tab Advanced search", "Advanced" ) );

	//Takes care of all recipe actions and provides a popup menu to 'recipeListWidget'
	actionHandler = new KreRecipeActionsHandler( recipeListWidget, database );
	basicSearchTab->setActionsHandler( actionHandler );


	// Signals & Slots
	connect( openButton, SIGNAL( clicked() ), actionHandler, SLOT( open() ) );
	connect( this, SIGNAL( recipeSelected( bool ) ), openButton, SLOT( setEnabled( bool ) ) );
	connect( editButton, SIGNAL( clicked() ), actionHandler, SLOT( edit() ) );
	connect( this, SIGNAL( recipeSelected( bool ) ), editButton, SLOT( setEnabled( bool ) ) );
	connect( removeButton, SIGNAL( clicked() ), actionHandler, SLOT( remove() ) );
	connect( this, SIGNAL( recipeSelected( bool ) ), removeButton, SLOT( setEnabled( bool ) ) );


	connect( recipeListWidget, SIGNAL( selectionChanged() ), this, SLOT( haveSelectedItems() ) );
	connect( recipeListWidget, SIGNAL( elementSelected(const QList<int> &, const QList<int> &) ), 
		this, SLOT( selectionChanged(const QList<int> &, const QList<int> &) ) );

	connect( advancedSearch, SIGNAL( recipeSelected( bool ) ), SIGNAL( recipeSelected( bool ) ) );
	connect( advancedSearch, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( advancedSearch, SIGNAL( recipesSelected( const QList<int> &, int ) ), SIGNAL( recipesSelected( const QList<int> &, int ) ) );

	connect( actionHandler, SIGNAL( recipeSelected( bool ) ), SIGNAL( recipeSelected( bool ) ) );
	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( actionHandler, SIGNAL( recipesSelected( const QList<int> &, int ) ), SIGNAL( recipesSelected( const QList<int> &, int ) ) );
}

SelectRecipeDialog::~SelectRecipeDialog()
{
}


void SelectRecipeDialog::selectionChanged( const QList<int> & recipes,
	const QList<int> & categories )
{
	selectedRecipesIds = recipes;
	selectedCategoriesIds = categories;
	kDebug() << recipes;
	kDebug() << categories;
	haveSelectedItems();
}

void SelectRecipeDialog::reload( ReloadFlags flag )
{
	recipeListWidget->reload( flag );
}

void SelectRecipeDialog::haveSelectedItems()
{
	if ( tabWidget->currentWidget() == basicSearchTab ) {
		if ( !selectedRecipesIds.isEmpty() ) {
			emit recipeSelected( true );
		} else {
			emit recipeSelected( false );
		}
	} else { //advanced search tab
		advancedSearch->haveSelectedItems();
	}
}

void SelectRecipeDialog::getCurrentRecipe( Recipe *recipe )
{
	QList<int> ids = recipeListWidget->selectedRecipes();
	if ( ids.count() == 1 ) {
		database->loadRecipe( recipe, RecipeDB::All, ids.first() );
	}
}

KreRecipeActionsHandler* SelectRecipeDialog::getActionsHandler() const
{
	if ( tabWidget->currentWidget() == basicSearchTab ) {
		return actionHandler;
	} else {
		return advancedSearch->actionHandler;
	}
}

void SelectRecipeDialog::addSelectRecipeAction( KAction * action )
{
	actionHandler->addRecipeAction( action );
}

void SelectRecipeDialog::addFindRecipeAction( KAction * action )
{
	advancedSearch->addAction( action );
}

void SelectRecipeDialog::addCategoryAction( KAction * action )
{
	actionHandler->addCategoryAction( action );
}

void SelectRecipeDialog::setCategorizeAction( KAction * action )
{
	actionHandler->setCategorizeAction( action );
}

void SelectRecipeDialog::setRemoveFromCategoryAction( KAction * action )
{
	actionHandler->setRemoveFromCategoryAction( action );
}

#include "selectrecipedialog.moc"
