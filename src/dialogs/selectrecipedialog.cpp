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

#include "selectrecipedialog.h"

#include <qsignalmapper.h>
#include <qtabwidget.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kprogress.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kcursor.h>

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
	tabWidget = new QTabWidget( this );
	tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	tabLayout->addWidget( tabWidget );

	basicSearchTab = new QGroupBox( this );
	basicSearchTab->setFrameStyle( QFrame::NoFrame );
	basicSearchTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	//Design dialog

	layout = new QGridLayout( basicSearchTab, 1, 1, 0, 0 );

	// Border Spacers
	QSpacerItem* spacer_left = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addMultiCell( spacer_left, 1, 4, 0, 0 );
	QSpacerItem* spacer_top = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addMultiCell( spacer_top, 0, 0, 1, 4 );

	searchBar = new QHBox( basicSearchTab );
	searchBar->setSpacing( 7 );
	layout->addWidget( searchBar, 1, 1 );

	KIconLoader *il = new KIconLoader;
	QPushButton *clearSearchButton = new QPushButton( searchBar );
	clearSearchButton->setPixmap( il->loadIcon( "locationbar_erase", KIcon::NoGroup, 16 ) );

	searchLabel = new QLabel( searchBar );
	searchLabel->setText( i18n( "Search:" ) );
	searchLabel->setFixedWidth( searchLabel->fontMetrics().width( i18n( "Search:" ) ) + 5 );
	searchBox = new KLineEdit( searchBar );

	QSpacerItem* searchSpacer = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( searchSpacer, 1, 2 );
	
	#ifdef ENABLE_SLOW
	categoryBox = new CategoryComboBox( basicSearchTab, database );
	layout->addWidget( categoryBox, 1, 3 );
	#endif

	QSpacerItem* spacerFromSearchBar = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( spacerFromSearchBar, 2, 1 );

	recipeListView = new RecipeListView( basicSearchTab, database );
	recipeListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Expanding );
	recipeListView->reload();
	layout->addMultiCellWidget( recipeListView, 3, 3, 1, 3 );

	buttonBar = new QHBox( basicSearchTab );
	layout->addMultiCellWidget( buttonBar, 4, 4, 1, 3 );

	openButton = new QPushButton( buttonBar );
	openButton->setText( i18n( "Open Recipe(s)" ) );
	openButton->setDisabled( true );
	QPixmap pm = il->loadIcon( "ok", KIcon::NoGroup, 16 );
	openButton->setIconSet( pm );
	editButton = new QPushButton( buttonBar );
	editButton->setText( i18n( "Edit Recipe" ) );
	editButton->setDisabled( true );
	pm = il->loadIcon( "edit", KIcon::NoGroup, 16 );
	editButton->setIconSet( pm );
	removeButton = new QPushButton( buttonBar );
	removeButton->setText( i18n( "Delete" ) );
	removeButton->setDisabled( true );
	removeButton->setMaximumWidth( 100 );
	pm = il->loadIcon( "editshred", KIcon::NoGroup, 16 );
	removeButton->setIconSet( pm );

	tabWidget->insertTab( basicSearchTab, i18n( "Basic" ) );

	advancedSearch = new AdvancedSearchDialog( this, database );
	tabWidget->insertTab( advancedSearch, i18n( "Advanced" ) );

	QToolTip::add( clearSearchButton, i18n( "Clear search" ) );

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

	connect( clearSearchButton, SIGNAL( clicked() ), this, SLOT( clearSearch() ) );

	KConfig * config = kapp->config();
	config->setGroup( "Performance" );
	if ( config->readBoolEntry("SearchAsYouType",true) ) {
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
	connect( advancedSearch, SIGNAL( recipesSelected( const QValueList<int> &, int ) ), SIGNAL( recipesSelected( const QValueList<int> &, int ) ) );

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( actionHandler, SIGNAL( recipesSelected( const QValueList<int> &, int ) ), SIGNAL( recipesSelected( const QValueList<int> &, int ) ) );
}

SelectRecipeDialog::~SelectRecipeDialog()
{
	delete il;
	delete recipeFilter;
}

void SelectRecipeDialog::clearSearch()
{
	searchBox->setText( QString::null );
	recipeFilter->filter( QString::null );
}

void SelectRecipeDialog::reload()
{
	recipeListView->reload();

	#ifdef ENABLE_SLOW
	categoryBox->reload();
	filterComboCategory( categoryBox->currentItem() );
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
	QPtrList<QListViewItem> items = recipeListView->selectedItems();
	if ( items.count() == 1 && items.at(0)->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* )items.at(0);
		database->loadRecipe( recipe, RecipeDB::All, recipe_it->recipeID() );
	}
}

void SelectRecipeDialog::filterComboCategory( int row )
{
	recipeListView->populateAll(); //TODO: this would be faster if we didn't need to load everything first

	kdDebug() << "I got row " << row << "\n";

	//First get the category ID corresponding to this combo row
	int categoryID = categoryBox->id( row );

	//Now filter
	recipeFilter->filterCategory( categoryID ); // if categoryID==-1 doesn't filter
	recipeFilter->filter( searchBox->text() );

	if ( categoryID != -1 ) {
	        QListViewItemIterator it( recipeListView );
		while ( it.current() ) {
			QListViewItem *item = it.current();
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
	if ( tabWidget->currentPage() == basicSearchTab )
		return actionHandler;
	else
		return advancedSearch->actionHandler;
}

#include "selectrecipedialog.moc"
