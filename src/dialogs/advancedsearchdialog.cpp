/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "advancedsearchdialog.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qspinbox.h>
#include <qframe.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>

#include "DBBackend/recipedb.h"
#include "recipeactionshandler.h"

AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget(parent),
  authorLast(0),
  categoryLast(0),
  ingredientLast(0),
  database(db)
{
	///AUTOMATICALLY GENERATED GUI CODE///
	advancedSearchLayout = new QHBoxLayout( this, 0, 0, "advancedSearchLayout"); 
	
	widgetStack = new QWidgetStack( this, "widgetStack" );
	
	searchPage = new QWidget( widgetStack, "searchPage" );
	searchPageLayout = new QGridLayout( searchPage, 1, 1, 3, 3, "searchPageLayout"); 
	
	searchButtonsLayout = new QHBoxLayout( 0, 0, 6, "searchButtonsLayout"); 
	searchSpacer = new QSpacerItem( 481, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
	searchButtonsLayout->addItem( searchSpacer );
	
	searchButton = new QPushButton( searchPage, "searchButton" );
	searchButtonsLayout->addWidget( searchButton );
	
	searchPageLayout->addMultiCellLayout( searchButtonsLayout, 2, 2, 0, 2 );
	
	categoriesBox = new QGroupBox( searchPage, "categoriesBox" );
	categoriesBox->setAlignment( int( QGroupBox::AlignTop ) );
	categoriesBox->setCheckable( TRUE );
	categoriesBox->setChecked( FALSE );
	categoriesBox->setColumnLayout(0, Qt::Vertical );
	categoriesBox->layout()->setSpacing( 6 );
	categoriesBox->layout()->setMargin( 11 );
	categoriesBoxLayout = new QHBoxLayout( categoriesBox->layout() );
	categoriesBoxLayout->setAlignment( Qt::AlignTop );
	
	categoriesFrame = new QFrame( categoriesBox, "categoriesFrame" );
	categoriesFrame->setFrameShape( QFrame::StyledPanel );
	categoriesFrame->setFrameShadow( QFrame::Raised );
	categoriesFrame->setLineWidth( 0 );
	categoriesFrameLayout = new QGridLayout( categoriesFrame, 1, 1, 0, 0, "categoriesFrameLayout"); 
	catSpacer = new QSpacerItem( 158, 21, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	categoriesFrameLayout->addItem( catSpacer, 3, 2 );
	
	catTypeComboBox = new QComboBox( FALSE, categoriesFrame, "catTypeComboBox" );
	catTypeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, catTypeComboBox->sizePolicy().hasHeightForWidth() ) );
	
	categoriesFrameLayout->addMultiCellWidget( catTypeComboBox, 0, 0, 0, 2 );
	
	catListView = new QListView( categoriesFrame, "catListView" );
	catListView->addColumn( i18n( "Name" ) );
	catListView->addColumn( i18n( "Id" ) );
	catListView->setAllColumnsShowFocus( TRUE );
	
	categoriesFrameLayout->addMultiCellWidget( catListView, 1, 1, 0, 2 );
	
	catSelectAllButton = new QPushButton( categoriesFrame, "catSelectAllButton" );
	
	categoriesFrameLayout->addMultiCellWidget( catSelectAllButton, 2, 3, 0, 0 );
	
	catUnselectAllButton = new QPushButton( categoriesFrame, "catUnselectAllButton" );
	
	categoriesFrameLayout->addMultiCellWidget( catUnselectAllButton, 2, 3, 1, 1 );
	categoriesBoxLayout->addWidget( categoriesFrame );
	
	searchPageLayout->addWidget( categoriesBox, 0, 0 );
	
	authorsBox = new QGroupBox( searchPage, "authorsBox" );
	authorsBox->setAlignment( int( QGroupBox::AlignTop ) );
	authorsBox->setCheckable( TRUE );
	authorsBox->setChecked( FALSE );
	authorsBox->setColumnLayout(0, Qt::Vertical );
	authorsBox->layout()->setSpacing( 6 );
	authorsBox->layout()->setMargin( 11 );
	authorsBoxLayout = new QHBoxLayout( authorsBox->layout() );
	authorsBoxLayout->setAlignment( Qt::AlignTop );
	
	authorsFrame = new QFrame( authorsBox, "authorsFrame" );
	authorsFrame->setFrameShape( QFrame::StyledPanel );
	authorsFrame->setFrameShadow( QFrame::Raised );
	authorsFrame->setLineWidth( 0 );
	authorsFrameLayout = new QGridLayout( authorsFrame, 1, 1, 0, 0, "authorsFrameLayout"); 
	
	authorTypeComboBox = new QComboBox( FALSE, authorsFrame, "authorTypeComboBox" );
	authorTypeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, authorTypeComboBox->sizePolicy().hasHeightForWidth() ) );
	
	authorsFrameLayout->addMultiCellWidget( authorTypeComboBox, 0, 0, 0, 2 );
	
	authorUnselectAllButton = new QPushButton( authorsFrame, "authorUnselectAllButton" );
	
	authorsFrameLayout->addWidget( authorUnselectAllButton, 2, 1 );
	authorSpacer = new QSpacerItem( 27, 21, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	authorsFrameLayout->addItem( authorSpacer, 2, 2 );
	
	authorSelectAllButton = new QPushButton( authorsFrame, "authorSelectAllButton" );
	
	authorsFrameLayout->addWidget( authorSelectAllButton, 2, 0 );
	
	authorListView = new QListView( authorsFrame, "authorListView" );
	authorListView->addColumn( i18n( "Name" ) );
	authorListView->addColumn( i18n( "Id" ) );
	authorListView->setAllColumnsShowFocus( TRUE );
	
	authorsFrameLayout->addMultiCellWidget( authorListView, 1, 1, 0, 2 );
	authorsBoxLayout->addWidget( authorsFrame );
	
	searchPageLayout->addMultiCellWidget( authorsBox, 0, 0, 1, 2 );
	
	servingsBox = new QGroupBox( searchPage, "servingsBox" );
	servingsBox->setAlignment( int( QGroupBox::AlignVCenter ) );
	servingsBox->setCheckable( TRUE );
	servingsBox->setChecked( FALSE );
	servingsBox->setColumnLayout(0, Qt::Vertical );
	servingsBox->layout()->setSpacing( 6 );
	servingsBox->layout()->setMargin( 11 );
	servingsBoxLayout = new QHBoxLayout( servingsBox->layout() );
	servingsBoxLayout->setAlignment( Qt::AlignTop );
	
	servingsFrame = new QFrame( servingsBox, "servingsFrame" );
	servingsFrame->setFrameShape( QFrame::StyledPanel );
	servingsFrame->setFrameShadow( QFrame::Raised );
	servingsFrame->setLineWidth( 0 );
	servingsFrameLayout = new QHBoxLayout( servingsFrame, 0, 0, "servingsFrameLayout"); 
	
	servingsComboBox = new QComboBox( FALSE, servingsFrame, "servingsComboBox" );
	servingsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, servingsComboBox->sizePolicy().hasHeightForWidth() ) );
	servingsFrameLayout->addWidget( servingsComboBox );
	
	servingsSpinBox = new QSpinBox( servingsFrame, "servingsSpinBox" );
	servingsSpinBox->setMinValue( 1 );
	servingsFrameLayout->addWidget( servingsSpinBox );
	servingsBoxLayout->addWidget( servingsFrame );
	
	searchPageLayout->addWidget( servingsBox, 1, 2 );
	
	ingredientsBox = new QGroupBox( searchPage, "ingredientsBox" );
	ingredientsBox->setAlignment( int( QGroupBox::AlignTop ) );
	ingredientsBox->setCheckable( TRUE );
	ingredientsBox->setChecked( FALSE );
	ingredientsBox->setColumnLayout(0, Qt::Vertical );
	ingredientsBox->layout()->setSpacing( 6 );
	ingredientsBox->layout()->setMargin( 11 );
	ingredientsBoxLayout = new QHBoxLayout( ingredientsBox->layout() );
	ingredientsBoxLayout->setAlignment( Qt::AlignTop );
	
	ingredientsFrame = new QFrame( ingredientsBox, "ingredientsFrame" );
	ingredientsFrame->setFrameShape( QFrame::StyledPanel );
	ingredientsFrame->setFrameShadow( QFrame::Raised );
	ingredientsFrame->setLineWidth( 0 );
	ingredientsFrameLayout = new QGridLayout( ingredientsFrame, 1, 1, 0, 0, "ingredientsFrameLayout"); 
	
	ingListView = new QListView( ingredientsFrame, "ingListView" );
	ingListView->addColumn( i18n( "Name" ) );
	ingListView->addColumn( i18n( "Id" ) );
	ingListView->setAllColumnsShowFocus( TRUE );
	
	ingredientsFrameLayout->addMultiCellWidget( ingListView, 1, 1, 0, 3 );
	
	ingTypeComboBox = new QComboBox( FALSE, ingredientsFrame, "ingTypeComboBox" );
	ingTypeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, ingTypeComboBox->sizePolicy().hasHeightForWidth() ) );
	
	ingredientsFrameLayout->addMultiCellWidget( ingTypeComboBox, 0, 0, 0, 2 );
	ingSpacer = new QSpacerItem( 184, 21, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	ingredientsFrameLayout->addItem( ingSpacer, 2, 2 );
	
	ingSelectAllButton = new QPushButton( ingredientsFrame, "ingSelectAllButton" );
	
	ingredientsFrameLayout->addWidget( ingSelectAllButton, 2, 0 );
	
	ingUnselectAllButton = new QPushButton( ingredientsFrame, "ingUnselectAllButton" );
	
	ingredientsFrameLayout->addWidget( ingUnselectAllButton, 2, 1 );
	ingredientsBoxLayout->addWidget( ingredientsFrame );
	
	searchPageLayout->addMultiCellWidget( ingredientsBox, 1, 1, 0, 1 );
	widgetStack->addWidget( searchPage, 0 );
	
	resultPage = new QWidget( widgetStack, "resultPage" );
	resultPageLayout = new QVBoxLayout( resultPage, 11, 6, "resultPageLayout"); 
	
	resultBox = new QGroupBox( resultPage, "resultBox" );
	resultBox->setColumnLayout(0, Qt::Vertical );
	resultBox->layout()->setSpacing( 6 );
	resultBox->layout()->setMargin( 11 );
	resultBoxLayout = new QHBoxLayout( resultBox->layout() );
	resultBoxLayout->setAlignment( Qt::AlignTop );
	
	resultsListView = new KListView( resultBox, "resultsListView" );
	resultsListView->addColumn( i18n( "Recipe" ) );
	resultsListView->addColumn( i18n( "Id" ) );
	resultsListView->setAllColumnsShowFocus( TRUE );
	resultBoxLayout->addWidget( resultsListView );
	resultPageLayout->addWidget( resultBox );
	
	resultsButtonsLayout = new QHBoxLayout( 0, 0, 6, "resultsButtonsLayout"); 
	resultsSpacer = new QSpacerItem( 521, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
	resultsButtonsLayout->addItem( resultsSpacer );
	
	backButton = new QPushButton( resultPage, "backButton" );
	resultsButtonsLayout->addWidget( backButton );
	
	openButton = new QPushButton( resultPage, "openButton" );
	resultsButtonsLayout->addWidget( openButton );
	resultPageLayout->addLayout( resultsButtonsLayout );
	widgetStack->addWidget( resultPage, 1 );
	advancedSearchLayout->addWidget( widgetStack );
	languageChange();
	clearWState( WState_Polished );
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	
	// ### At present, this is the only way I can get this dialog's size down
	//     Should I put each element in its own tab to avoid squeezing them
	//     into this one page???
	authorListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	catListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	ingListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	
	KIconLoader *il=new KIconLoader;
	openButton->setIconSet(il->loadIcon("ok", KIcon::NoGroup,16));
	backButton->setIconSet(il->loadIcon("back", KIcon::NoGroup,16));
	searchButton->setIconSet(il->loadIcon("find", KIcon::NoGroup,16));
	

	authorsFrame->setEnabled(false);
	categoriesFrame->setEnabled(false);
	ingredientsFrame->setEnabled(false);
	servingsFrame->setEnabled(false);
	
	RecipeActionsHandler *actionHandler = new RecipeActionsHandler(resultsListView,database,0,1,-1,RecipeActionsHandler::Open|RecipeActionsHandler::Edit|RecipeActionsHandler::SaveAs);

	connect( searchButton, SIGNAL(clicked()), SLOT(search()) );
	connect( backButton, SIGNAL(clicked()), SLOT(back()) );
	connect( authorsBox, SIGNAL(toggled(bool)), authorsFrame, SLOT(setEnabled(bool)) );
	connect( categoriesBox, SIGNAL(toggled(bool)), categoriesFrame, SLOT(setEnabled(bool)) );
	connect( ingredientsBox, SIGNAL(toggled(bool)), ingredientsFrame, SLOT(setEnabled(bool)) );
	connect( servingsBox, SIGNAL(toggled(bool)), servingsFrame, SLOT(setEnabled(bool)) );
	connect( authorTypeComboBox, SIGNAL(activated(int)), SLOT(authorSwitchType(int)) );
	connect( ingTypeComboBox, SIGNAL(activated(int)), SLOT(ingredientSwitchType(int)) );
	connect( catTypeComboBox, SIGNAL(activated(int)), SLOT(categorySwitchType(int)) );
	connect( authorSelectAllButton, SIGNAL(clicked()), SLOT(selectAllAuthors()) );
	connect( catSelectAllButton, SIGNAL(clicked()), SLOT(selectAllCategories()) );
	connect( ingSelectAllButton, SIGNAL(clicked()), SLOT(selectAllIngredients()) );
	connect( authorUnselectAllButton, SIGNAL(clicked()), SLOT(unselectAllAuthors()) );
	connect( catUnselectAllButton, SIGNAL(clicked()), SLOT(unselectAllCategories()) );
	connect( ingUnselectAllButton, SIGNAL(clicked()), SLOT(unselectAllIngredients()) );
	connect( actionHandler, SIGNAL(recipeSelected(int,int)),SIGNAL(recipeSelected(int,int)) );
	
	connect( openButton, SIGNAL(clicked()), actionHandler, SLOT(open()) );

	reload();
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{
}

void AdvancedSearchDialog::languageChange()
{
	searchButton->setText( i18n( "Search" ) );
	categoriesBox->setTitle( i18n( "Categories" ) );
	catTypeComboBox->clear();
	catTypeComboBox->insertItem( i18n( "Belong to:" ) );
	catTypeComboBox->insertItem( i18n( "Do not belong to:" ) );
	QWhatsThis::add( catTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe belongs to certain categories.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	catListView->header()->setLabel( 0, i18n( "Name" ) );
	catListView->header()->setLabel( 1, i18n( "Id" ) );
	catSelectAllButton->setText( i18n( "Select All" ) );
	catUnselectAllButton->setText( i18n( "Unselect All" ) );
	authorsBox->setTitle( i18n( "Authors" ) );
	authorTypeComboBox->clear();
	authorTypeComboBox->insertItem( i18n( "By:" ) );
	authorTypeComboBox->insertItem( i18n( "Not by:" ) );
	QWhatsThis::add( authorTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe is by certain authors.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	authorUnselectAllButton->setText( i18n( "Unselect All" ) );
	authorSelectAllButton->setText( i18n( "Select All" ) );
	authorListView->header()->setLabel( 0, i18n( "Name" ) );
	authorListView->header()->setLabel( 1, i18n( "Id" ) );
	servingsBox->setTitle( i18n( "Servings" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Serves at least:" ) );
	servingsComboBox->insertItem( i18n( "Serves at most:" ) );
	servingsComboBox->insertItem( i18n( "Serves exactly:" ) );
	ingredientsBox->setTitle( i18n( "Ingredients" ) );
	ingListView->header()->setLabel( 0, i18n( "Name" ) );
	ingListView->header()->setLabel( 1, i18n( "Id" ) );
	ingTypeComboBox->clear();
	ingTypeComboBox->insertItem( i18n( "Use:" ) );
	ingTypeComboBox->insertItem( i18n( "Do not use:" ) );
	QWhatsThis::add( ingTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe uses or does not use certain ingredients.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	ingSelectAllButton->setText( i18n( "Select All" ) );
	ingUnselectAllButton->setText( i18n( "Unselect All" ) );
	resultBox->setTitle( i18n( "Search Results" ) );
	resultsListView->header()->setLabel( 0, i18n( "Recipe" ) );
	resultsListView->header()->setLabel( 1, i18n( "Id" ) );
	backButton->setText( i18n( "Back" ) );
	openButton->setText( i18n( "Open" ) );
}

void AdvancedSearchDialog::selectAllAuthors()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(authorListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(true);
	}
}

void AdvancedSearchDialog::selectAllCategories()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(catListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(true);
	}
}

void AdvancedSearchDialog::selectAllIngredients()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(ingListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(true);
	}
}

void AdvancedSearchDialog::unselectAllAuthors()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(authorListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(false);
	}
}

void AdvancedSearchDialog::unselectAllCategories()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(catListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(false);
	}
}

void AdvancedSearchDialog::unselectAllIngredients()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(ingListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(false);
	}
}

void AdvancedSearchDialog::reload()
{
	loadAuthorListView();
	loadCategoryListView();
	loadIngredientListView();
}

void AdvancedSearchDialog::loadAuthorListView()
{
	authorListView->clear();
	authorPosMap.clear();
	authorNegMap.clear();

	ElementList authorsList; database->loadAuthors( &authorsList );
	
	for ( ElementList::const_iterator it = authorsList.begin(); it != authorsList.end(); ++it )
	{
		QCheckListItem *item=new QCheckListItem(authorListView,(*it).name,QCheckListItem::CheckBox);
		item->setText(1,QString::number((*it).id));
		
		authorNegMap.insert(item,false);
		authorPosMap.insert(item,false);
	}
}

void AdvancedSearchDialog::loadCategoryListView()
{
	catListView->clear();
	categoryPosMap.clear();
	categoryNegMap.clear();

	ElementList catList; database->loadCategories( &catList );
	
	for ( ElementList::const_iterator it = catList.begin(); it != catList.end(); ++it )
	{
		QCheckListItem *item=new QCheckListItem(catListView,(*it).name,QCheckListItem::CheckBox);
		item->setText(1,QString::number((*it).id));
		
		categoryNegMap.insert(item,false);
		categoryPosMap.insert(item,false);
	}
}

void AdvancedSearchDialog::loadIngredientListView()
{
	ingListView->clear();
	ingredientPosMap.clear();
	ingredientNegMap.clear();

	ElementList ingList; database->loadIngredients( &ingList );
	
	for ( ElementList::const_iterator it = ingList.begin(); it != ingList.end(); ++it )
	{
		QCheckListItem *item=new QCheckListItem(ingListView,(*it).name,QCheckListItem::CheckBox);
		item->setText(1,QString::number((*it).id));
		
		ingredientNegMap.insert(item,false);
		ingredientPosMap.insert(item,false);
	}
}

void AdvancedSearchDialog::authorSwitchType(int index)
{
	if ( authorLast == authorTypeComboBox->currentItem() ) //don't do anything unless the selection has changed
		return;

	QMap<QCheckListItem*,bool> *map_to_store;
	QMap<QCheckListItem*,bool> *map_to_load;

	if ( index == 0 ) //store negative
	{
		map_to_load = &authorPosMap;
		map_to_store = &authorNegMap;
	}
	else //store positive
	{
		map_to_load = &authorNegMap;
		map_to_store = &authorPosMap;
	}
	
	updateMaps(map_to_load,map_to_store,authorListView);
	
	authorLast = authorTypeComboBox->currentItem();
	authorListView->clearSelection();
}

void AdvancedSearchDialog::categorySwitchType(int index)
{
	if ( categoryLast == catTypeComboBox->currentItem() ) //don't do anything unless the selection has changed
		return;

	QMap<QCheckListItem*,bool> *map_to_store;
	QMap<QCheckListItem*,bool> *map_to_load;

	if ( index == 0 ) //store negative
	{
		map_to_load = &categoryPosMap;
		map_to_store = &categoryNegMap;
	}
	else //store positive
	{
		map_to_load = &categoryNegMap;
		map_to_store = &categoryPosMap;
	}
	
	updateMaps(map_to_load,map_to_store,catListView);
	
	categoryLast = catTypeComboBox->currentItem();
	catListView->clearSelection();
}

void AdvancedSearchDialog::ingredientSwitchType(int index)
{
	if ( ingredientLast == ingTypeComboBox->currentItem() ) //don't do anything unless the selection has changed
		return;

	QMap<QCheckListItem*,bool> *map_to_store;
	QMap<QCheckListItem*,bool> *map_to_load;

	if ( index == 0 ) //store negative
	{
		map_to_load = &ingredientPosMap;
		map_to_store = &ingredientNegMap;
	}
	else //store positive
	{
		map_to_load = &ingredientNegMap;
		map_to_store = &ingredientPosMap;
	}
	
	updateMaps(map_to_load,map_to_store,ingListView);
	
	ingredientLast = ingTypeComboBox->currentItem();
	ingListView->clearSelection();
}

void AdvancedSearchDialog::updateMaps(QMap<QCheckListItem*,bool> *map_to_load, QMap<QCheckListItem*,bool> *map_to_store, QListView *listview)
{
	//store
	storeMap(map_to_store,listview);

	//restore
	for ( QMap<QCheckListItem*,bool>::Iterator it = map_to_load->begin(); it != map_to_load->end(); ++it )
	{
		it.key()->setOn(it.data());
		it.key()->setEnabled(true);
	}
	
	//be sensible and don't allow items to be simultaneously enabled and disabled
	for ( QMap<QCheckListItem*,bool>::Iterator it = map_to_store->begin(); it != map_to_store->end(); ++it )
		it.key()->setEnabled(!it.data());
}

void AdvancedSearchDialog::storeMap(QMap<QCheckListItem*,bool> *map_to_store, QListView *listview)
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(listview->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
		map_to_store->insert(qlv_it,qlv_it->isOn());
}

void AdvancedSearchDialog::search()
{
	KApplication::setOverrideCursor( KCursor::waitCursor() );
	
	//we'll load all the recipes and whittle down the list based on constraints
	RecipeList allRecipes; database->loadRecipeDetails( &allRecipes, true, true, false, true );
	
	QCheckListItem *qlv_it;

	//narrow down by servings
	if ( servingsBox->isChecked() )
	{
		int servings = servingsSpinBox->value();
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
		{
			switch ( servingsComboBox->currentItem() )
			{
			//at least
			case 0: if ( (*it).persons < servings ){ it = allRecipes.remove( it ); it--; }
				break;
			//at most
			case 1: if ( (*it).persons > servings ){ it = allRecipes.remove( it ); it--; }
				break;
			//exactly
			case 2: if ( (*it).persons != servings ){ it = allRecipes.remove( it ); it--; }
				break;
			}
		}
	}
	
	//narrow down by authors
	if ( authorsBox->isChecked() )
	{
		storeMap( (authorTypeComboBox->currentItem() == 0) ? &authorPosMap : &authorNegMap, authorListView ); //the other won't have been updated yet

		//positive search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = authorPosMap.begin(); map_it != authorPosMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).authorList.find( i ) == (*it).authorList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}

		//negative search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = authorNegMap.begin(); map_it != authorNegMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).authorList.find( i ) != (*it).authorList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}
	}
	
	//narrow down by categories
	if ( categoriesBox->isChecked() )
	{
		storeMap( (catTypeComboBox->currentItem() == 0) ? &categoryPosMap : &categoryNegMap, catListView ); //the other won't have been updated yet

		//positive search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = categoryPosMap.begin(); map_it != categoryPosMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
				if ( (*it).categoryList.find( i ) == (*it).categoryList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}

		//negative search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = categoryNegMap.begin(); map_it != categoryNegMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).categoryList.find( i ) != (*it).categoryList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}
	}

	//narrow down by ingredients
	if ( ingredientsBox->isChecked() )
	{
		storeMap( (ingTypeComboBox->currentItem() == 0) ? &ingredientPosMap : &ingredientNegMap, ingListView ); //the other won't have been updated yet

		//positive search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = ingredientPosMap.begin(); map_it != ingredientPosMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
 				{
					int id = map_it.key()->text(1).toInt();
					if ( (*it).ingList.find( id ) == -1 )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}

		//negative search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = ingredientNegMap.begin(); map_it != ingredientNegMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					int id = map_it.key()->text(1).toInt();
					if ( (*it).ingList.find( id ) != -1 )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}
	}
	
	//now display the recipes left
	resultsListView->clear();
	for ( RecipeList::const_iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
		(void)new QListViewItem(resultsListView,(*it).title,QString::number((*it).recipeID));

	widgetStack->raiseWidget( resultPage );
	KApplication::restoreOverrideCursor();
}

void AdvancedSearchDialog::back()
{
	widgetStack->raiseWidget( searchPage );
}

#include "advancedsearchdialog.moc"
