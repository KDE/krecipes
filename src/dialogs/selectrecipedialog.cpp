/***************************************************************************
*	Copyright (C) 2003 by                                                 *
*	Unai Garro (ugarro@users.sourceforge.net)                             *
*	Cyril Bosselut (bosselut@b1project.com)                               *
*	Jason Kivlighn (jkivlighn@gmail.com)                                  *
*	                                                                      *
*	This program is free software; you can redistribute it and/or modify  *
*	it under the terms of the GNU General Public License as published by  *
*	the Free Software Foundation; either version 2 of the License, or     *
*	(at your option) any later version.                                   *
***************************************************************************/

#include "selectrecipedialog.h"

#include <KTabWidget>
//Added by qt3to4:
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
#include "recipefilter.h"
#include "widgets/recipelistview.h"
#include "widgets/categorylistview.h"
#include "widgets/categorycombobox.h"

SelectRecipeDialog::SelectRecipeDialog( QWidget *parent, RecipeDB* db )
		: QWidget( parent )
{
	//Store pointer to Recipe Database
	database = db;

	QVBoxLayout *tabLayout = new QVBoxLayout( this );
	tabWidget = new KTabWidget( this );
	tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	tabLayout->addWidget( tabWidget );

	basicSearchTab = new QFrame( this );
	basicSearchTab->setFrameStyle( QFrame::NoFrame );
	//basicSearchTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	//Design dialog

	layout = new QGridLayout;
	basicSearchTab->setLayout( layout );
	layout->cellRect( 1, 1 );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacer_left, 1, 0, 4, 0, 0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacer_top, 0, 1, 1, 4, 0 );

	searchBar = new KHBox( basicSearchTab );
	searchBar->setSpacing( 7 );
	layout->addWidget( searchBar, 1, 1 );


	searchLabel = new QLabel( searchBar );
	searchLabel->setText( i18n( "Search:" ) );
	searchLabel->setFixedWidth( searchLabel->fontMetrics().width( i18n( "Search:" ) ) + 5 );
	searchBox = new KLineEdit( searchBar );
	searchBox->setClearButtonShown( true );
	connect( searchBox, SIGNAL(clearButtonClicked() ),this,SLOT( clearSearch() ) );

	QSpacerItem* searchSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( searchSpacer, 1, 2 );

	#ifdef ENABLE_SLOW
	categoryBox = new CategoryComboBox( basicSearchTab, database );
	layout->addWidget( categoryBox, 1, 3 );
	#endif

	QSpacerItem* spacerFromSearchBar = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerFromSearchBar, 2, 1 );

	recipeListView = new RecipeListView( basicSearchTab, database );
	recipeListView->reload();
	recipeListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Expanding );
	layout->addWidget( recipeListView, 3, 1, 1, 3, 0 );

	buttonBar = new KHBox( basicSearchTab );
	layout->addWidget( buttonBar, 4, 1, 1, 3, 0 );

	openButton = new KPushButton( buttonBar );
	openButton->setText( i18n( "Show Recipe(s)" ) );
	openButton->setDisabled( true );
	openButton->setIcon( KIcon( "system-search" ) );
	editButton = new KPushButton( buttonBar );
	editButton->setText( i18n( "Edit Recipe" ) );
	editButton->setDisabled( true );
	editButton->setIcon( KIcon( "document-edit" ) );
	removeButton = new KPushButton( buttonBar );
	removeButton->setText( i18n( "Delete" ) );
	removeButton->setDisabled( true );
	removeButton->setMaximumWidth( 100 );
	removeButton->setIcon( KIcon("edit-delete-shred" ) );

	tabWidget->insertTab( -1, basicSearchTab, i18n( "Basic" ) );

	advancedSearch = new AdvancedSearchDialog( this, database );
	tabWidget->insertTab( -1, advancedSearch, i18n( "Advanced" ) );

	//Takes care of all recipe actions and provides a popup menu to 'recipeListView'
	actionHandler = new RecipeActionsHandler( recipeListView, database );

	recipeFilter = new RecipeFilter( recipeListView );

	// Signals & Slots

	connect( openButton, SIGNAL( clicked() ), actionHandler, SLOT( open() ) );
	connect( this, SIGNAL( recipeSelected( bool ) ), openButton, SLOT( setEnabled( bool ) ) );
	connect( editButton, SIGNAL( clicked() ), actionHandler, SLOT( edit() ) );
	connect( this, SIGNAL( recipeSelected( bool ) ), editButton, SLOT( setEnabled( bool ) ) );
	connect( removeButton, SIGNAL( clicked() ), actionHandler, SLOT( remove() ) );
	connect( this, SIGNAL( recipeSelected( bool ) ), removeButton, SLOT( setEnabled( bool ) ) );

	KConfigGroup config (KGlobal::config(), "Performance" );
	if ( config.readEntry("SearchAsYouType",true) ) {
		connect( searchBox, SIGNAL( returnPressed( const QString& ) ), recipeFilter, SLOT( filter( const QString& ) ) );
		connect( searchBox, SIGNAL( textChanged( const QString& ) ), this, SLOT( ensurePopulated() ) );
		connect( searchBox, SIGNAL( textChanged( const QString& ) ), recipeFilter, SLOT( filter( const QString& ) ) );
	}
	else {
		connect( searchBox, SIGNAL( returnPressed( const QString& ) ), this, SLOT( ensurePopulated() ) );
		connect( searchBox, SIGNAL( returnPressed( const QString& ) ), recipeFilter, SLOT( filter( const QString& ) ) );
	}

	connect( recipeListView, SIGNAL( selectionChanged() ), this, SLOT( haveSelectedItems() ) );
	#ifdef ENABLE_SLOW
	connect( recipeListView, SIGNAL( nextGroupLoaded() ), categoryBox, SLOT( loadNextGroup() ) );
	connect( recipeListView, SIGNAL( prevGroupLoaded() ), categoryBox, SLOT( loadPrevGroup() ) );
	connect( categoryBox, SIGNAL( activated( int ) ), this, SLOT( filterComboCategory( int ) ) );
	#endif
	connect( recipeListView, SIGNAL( nextGroupLoaded() ), SLOT( refilter() ) );
	connect( recipeListView, SIGNAL( prevGroupLoaded() ), SLOT( refilter() ) );

	connect( advancedSearch, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( advancedSearch, SIGNAL( recipesSelected( const QList<int> &, int ) ), SIGNAL( recipesSelected( const QList<int> &, int ) ) );

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( actionHandler, SIGNAL( recipesSelected( const QList<int> &, int ) ), SIGNAL( recipesSelected( const QList<int> &, int ) ) );
}

SelectRecipeDialog::~SelectRecipeDialog()
{
	delete recipeFilter;
}

void SelectRecipeDialog::clearSearch()
{
	searchBox->setText( QString::null );
	recipeFilter->filter( QString::null );
}

void SelectRecipeDialog::reload( ReloadFlags flag )
{
	recipeListView->reload(flag);

	#ifdef ENABLE_SLOW
	categoryBox->reload();
	filterComboCategory( categoryBox->currentIndex() );
	#endif
}

void SelectRecipeDialog::refilter()
{
	if ( !searchBox->text().isEmpty() ) {
		ensurePopulated();
		recipeFilter->filter(searchBox->text());
	}
}

void SelectRecipeDialog::ensurePopulated()
{
	recipeListView->populateAll();
}

void SelectRecipeDialog::haveSelectedItems()
{
	if ( recipeListView->selectedItems().count() > 0 )
		emit recipeSelected( true );
	else
		emit recipeSelected( false );
}

void SelectRecipeDialog::getCurrentRecipe( Recipe *recipe )
{
	QList<Q3ListViewItem*> items = recipeListView->selectedItems();
	if ( items.count() == 1 && items.at(0)->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* )items.at(0);
		database->loadRecipe( recipe, RecipeDB::All, recipe_it->recipeID() );
	}
}

void SelectRecipeDialog::filterComboCategory( int row )
{
	recipeListView->populateAll(); //TODO: this would be faster if we didn't need to load everything first

	kDebug() << "I got row " << row << "\n";

	//First get the category ID corresponding to this combo row
	int categoryID = categoryBox->id( row );

	//Now filter
	recipeFilter->filterCategory( categoryID ); // if categoryID==-1 doesn't filter
	recipeFilter->filter( searchBox->text() );

	if ( categoryID != -1 ) {
		Q3ListViewItemIterator it( recipeListView );
		while ( it.current() ) {
			Q3ListViewItem *item = it.current();
			if ( item->isVisible() ) {
				item->setOpen( true ); 	//will only open if already populated
							//(could be the selected category's parent
				if ( !item->firstChild() ) {
					recipeListView->open( item ); //populates and opens the selected category
					break;
				}
			}
			++it;
		}

	}
}

RecipeActionsHandler* SelectRecipeDialog::getActionsHandler() const
{
	if ( tabWidget->currentWidget() == basicSearchTab )
		return actionHandler;
	else
		return advancedSearch->actionHandler;
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
