/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "advancedsearchdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>
#include <kpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

#include "DBBackend/recipedb.h"
#include "recipeactionshandler.h"
#include "widgets/recipelistview.h"

void updateLists( QListView *listview, QValueList<Element> &positiveSelections, QValueList<Element> &negativeSelections, bool state )
{
	for ( QValueList<Element>::const_iterator it = negativeSelections.begin(); it != negativeSelections.end(); ++it ) {
		QCheckListItem * item = ( QCheckListItem* ) listview->findItem( QString::number( (*it).id ), 1 );
		if ( item ) {
			item->setEnabled((state==0)?false:true);
			item->setOn((state==0)?false:true);
		}
	}
	for ( QValueList<Element>::const_iterator it = positiveSelections.begin(); it != positiveSelections.end(); ++it ) {
		QCheckListItem * item = ( QCheckListItem* ) listview->findItem( QString::number( (*it).id ), 1 );
		if ( item ) {
			item->setEnabled((state==0)?true:false);
			item->setOn((state==0)?true:false);
		}
	}
}

DualAuthorListView::DualAuthorListView( QWidget *parent, RecipeDB *db ) : AuthorCheckListView( parent, db ),
	last_state( 0 ),
	lock_updates(false)
{
}

void DualAuthorListView::load( int limit, int offset )
{
	AuthorListView::load(limit,offset);

	lock_updates = true; //the following shouldn't change the stored selection values
	updateLists(this,positiveSelections,negativeSelections,last_state);
	lock_updates = false;
}

void DualAuthorListView::change( int index )
{
	if ( index == last_state )  //don't do anything unless the selection has changed
		return ;

	lock_updates = true; //the following shouldn't change the stored selection values
	updateLists(this,positiveSelections,negativeSelections,index);
	lock_updates = false;

	last_state = index;
	clearSelection();
}

void DualAuthorListView::createAuthor( const Element &author )
{
	createElement(new AuthorCheckListItem( this, author ));
}

void DualAuthorListView::removeAuthor( int id )
{
	QCheckListItem * item = ( QCheckListItem* ) findItem( QString::number( id ), 1 );

	Element author; author.id = id; //don't use item->author() since 'item' may not exist
	negativeSelections.remove( author );
	positiveSelections.remove( author );
	delete item;
}

void DualAuthorListView::stateChange( AuthorCheckListItem *it, bool on )
{
	if ( lock_updates || reloading() ) return;

	if ( last_state == 0 ) { //we're storing positive matches
		if ( on )
			positiveSelections.append(it->author());
		else
			positiveSelections.remove(it->author());
	}
	else { //we're storing negative matches
		if ( on )
			negativeSelections.append(it->author());
		else
			negativeSelections.remove(it->author());
	}
}


DualIngredientListView::DualIngredientListView( QWidget *parent, RecipeDB *db ) : IngredientCheckListView( parent, db ),
	last_state( 0 ), lock_updates(false)
{
}

void DualIngredientListView::load( int limit, int offset )
{
	IngredientListView::load(limit,offset);

	lock_updates = true; //the following shouldn't change the stored selection values
	updateLists(this,positiveSelections,negativeSelections,last_state);
	lock_updates = false;
}

void DualIngredientListView::stateChange( IngredientCheckListItem *it, bool on )
{
	if ( lock_updates || reloading() ) return;

	if ( last_state == 0 ) { //we're storing positive matches
		if ( on )
			positiveSelections.append(it->ingredient());
		else
			positiveSelections.remove(it->ingredient());
	}
	else { //we're storing negative matches
		if ( on )
			negativeSelections.append(it->ingredient());
		else
			negativeSelections.remove(it->ingredient());
	}
}

void DualIngredientListView::change( int index )
{
	if ( index == last_state )  //don't do anything unless the selection has changed
		return;

	lock_updates = true; //the following shouldn't change the stored selection values
	updateLists(this,positiveSelections,negativeSelections,index);
	lock_updates = false;

	last_state = index;
	clearSelection();
}

void DualIngredientListView::createIngredient( const Element &ing )
{
	createElement(new IngredientCheckListItem( this, ing ));
}

void DualIngredientListView::removeIngredient( int id )
{
	IngredientCheckListItem * item = ( IngredientCheckListItem* ) findItem( QString::number( id ), 1 );

	Element ing; ing.id = id; //don't use item->ingredient() since 'item' may not exist
	negativeSelections.remove( ing );
	positiveSelections.remove( ing );
	delete item;
}


DualCategoryListView::DualCategoryListView( QWidget *parent, RecipeDB *db ) : CategoryCheckListView( parent, db ),
	last_state( 0 ),
	lock_updates(false)
{
}

void DualCategoryListView::load( int limit, int offset )
{
	CategoryListView::load(limit,offset);

	lock_updates = true; //the following shouldn't change the stored selection values
	updateLists(this,positiveSelections,negativeSelections,last_state);
	lock_updates = false;
}

void DualCategoryListView::change( int index )
{
	if ( index == last_state )  //don't do anything unless the selection has changed
		return ;

	lock_updates = true; //the following shouldn't change the stored selection values
	updateLists(this,positiveSelections,negativeSelections,index);
	lock_updates = false;

	last_state = index;
	clearSelection();
}

void DualCategoryListView::createCategory( const Element &category, int parent_id )
{
	CategoryCheckListItem * new_item;
	if ( parent_id == -1 ) {
		new_item = new CategoryCheckListItem( this, category, false );
		createElement(new_item);
	}
	else {
		QListViewItem *parent = findItem( QString::number( parent_id ), 1 );

		if ( parent )
			new_item = new CategoryCheckListItem( parent, category, false );
	}

	new_item->setOpen( true );
}

void DualCategoryListView::removeCategory( int id )
{
	QCheckListItem * item = ( QCheckListItem* ) findItem( QString::number( id ), 1 );

	Element cat; cat.id = id; //don't use item->category() since 'item' may not exist
	negativeSelections.remove( cat );
	positiveSelections.remove( cat );
	delete item;
}

void DualCategoryListView::modifyCategory( const Element &category )
{
	QListViewItem * item = findItem( QString::number( category.id ), 1 );

	Q_ASSERT( item );

	item->setText( 0, category.name );
}

void DualCategoryListView::modifyCategory( int id, int parent_id )
{
	QListViewItem * item = findItem( QString::number( id ), 1 );
	if ( !item->parent() )
		takeItem( item );
	else
		item->parent() ->takeItem( item );

	Q_ASSERT( item );

	if ( parent_id == -1 )
		insertItem( item );
	else
		findItem( QString::number( parent_id ), 1 ) ->insertItem( item );
}

void DualCategoryListView::mergeCategories( int id1, int id2 )
{
	CategoryCheckListItem * to_item = ( CategoryCheckListItem* ) findItem( QString::number( id1 ), 1 );
	CategoryCheckListItem *from_item = ( CategoryCheckListItem* ) findItem( QString::number( id2 ), 1 );

	//note that this takes care of any recipes that may be children as well
	QListViewItem *next_sibling;
	for ( QListViewItem * it = from_item->firstChild(); it; it = next_sibling ) {
		next_sibling = it->nextSibling(); //get the sibling before we move the item

		from_item->takeItem( it );
		to_item->insertItem( it );
	}

	removeCategory( id2 );
}

void DualCategoryListView::stateChange( CategoryCheckListItem *it, bool on )
{
	if ( lock_updates || reloading() ) return;

	if ( last_state == 0 ) { //we're storing positive matches
		if ( on )
			positiveSelections.append(it->element());
		else
			positiveSelections.remove(it->element());
	}
	else { //we're storing negative matches
		if ( on )
			negativeSelections.append(it->element());
		else
			negativeSelections.remove(it->element());
	}
}


AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent ),
		database( db )
{
	setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

	///
	///BEGIN OF AUTOMATICALLY GENERATED GUI CODE///
	///
	AdvancedSearchDialogLayout = new QVBoxLayout( this, 11, 6, "AdvancedSearchDialogLayout" );

	layout17 = new QHBoxLayout( 0, 0, 6, "layout17" );

	paramsTabWidget = new QTabWidget( this, "paramsTabWidget" );

	ingTab = new QWidget( paramsTabWidget, "ingTab" );
	ingTabLayout = new QHBoxLayout( ingTab, 4, 4, "ingTabLayout" );

	layout18 = new QVBoxLayout( 0, 0, 6, "layout18" );

	enableIngCheckBox = new QCheckBox( ingTab, "enableIngCheckBox" );
	layout18->addWidget( enableIngCheckBox );
	spacer6 = new QSpacerItem( 21, 70, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout18->addItem( spacer6 );

	ingSelectAllButton = new QPushButton( ingTab, "ingSelectAllButton" );
	layout18->addWidget( ingSelectAllButton );

	ingUnselectAllButton = new QPushButton( ingTab, "ingUnselectAllButton" );
	layout18->addWidget( ingUnselectAllButton );
	ingTabLayout->addLayout( layout18 );

	ingredientsFrame = new QFrame( ingTab, "ingredientsFrame" );
	ingredientsFrame->setEnabled( FALSE );
	ingredientsFrame->setFrameShape( QFrame::NoFrame );
	ingredientsFrame->setFrameShadow( QFrame::Plain );
	ingredientsFrame->setLineWidth( 0 );
	ingredientsFrameLayout = new QVBoxLayout( ingredientsFrame, 0, 0, "ingredientsFrameLayout" );

	ingTypeComboBox = new QComboBox( FALSE, ingredientsFrame, "ingTypeComboBox" );
	ingredientsFrameLayout->addWidget( ingTypeComboBox );

	ingListView = new DualIngredientListView( ingredientsFrame, database );
	ingListView->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 5, 0, 0, ingListView->sizePolicy().hasHeightForWidth() ) );
	ingListView->setAllColumnsShowFocus( TRUE );
	ingredientsFrameLayout->addWidget( ingListView );
	ingTabLayout->addWidget( ingredientsFrame );
	paramsTabWidget->insertTab( ingTab, QString( "" ) );

	catTab = new QWidget( paramsTabWidget, "catTab" );
	catTabLayout = new QHBoxLayout( catTab, 4, 4, "catTabLayout" );

	layout16 = new QVBoxLayout( 0, 0, 6, "layout16" );

	enableCatCheckBox = new QCheckBox( catTab, "enableCatCheckBox" );
	layout16->addWidget( enableCatCheckBox );
	spacer5 = new QSpacerItem( 51, 50, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout16->addItem( spacer5 );

	catSelectAllButton = new QPushButton( catTab, "catSelectAllButton" );
	layout16->addWidget( catSelectAllButton );

	catUnselectAllButton = new QPushButton( catTab, "catUnselectAllButton" );
	layout16->addWidget( catUnselectAllButton );
	catTabLayout->addLayout( layout16 );

	categoriesFrame = new QFrame( catTab, "categoriesFrame" );
	categoriesFrame->setEnabled( FALSE );
	categoriesFrame->setFrameShape( QFrame::NoFrame );
	categoriesFrame->setFrameShadow( QFrame::Plain );
	categoriesFrame->setLineWidth( 0 );
	categoriesFrameLayout = new QVBoxLayout( categoriesFrame, 0, 0, "categoriesFrameLayout" );

	catTypeComboBox = new QComboBox( FALSE, categoriesFrame, "catTypeComboBox" );
	categoriesFrameLayout->addWidget( catTypeComboBox );

	catListView = new DualCategoryListView( categoriesFrame, database );
	catListView->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 5, 0, 0, catListView->sizePolicy().hasHeightForWidth() ) );
	catListView->setAllColumnsShowFocus( TRUE );
	categoriesFrameLayout->addWidget( catListView );
	catTabLayout->addWidget( categoriesFrame );
	paramsTabWidget->insertTab( catTab, QString( "" ) );

	servPrepTab = new QWidget( paramsTabWidget, "servPrepTab" );
	servPrepTabLayout = new QHBoxLayout( servPrepTab, 11, 6, "servPrepTabLayout" );

	servingsBox = new QGroupBox( servPrepTab, "servingsBox" );
	servingsBox->setAlignment( int( QGroupBox::AlignVCenter ) );
	servingsBox->setColumnLayout( 0, Qt::Vertical );
	servingsBox->layout() ->setSpacing( 6 );
	servingsBox->layout() ->setMargin( 11 );
	servingsBoxLayout = new QVBoxLayout( servingsBox->layout() );
	servingsBoxLayout->setAlignment( Qt::AlignTop );

	enableServingsCheckBox = new QCheckBox( servingsBox, "enableServingsCheckBox" );
	servingsBoxLayout->addWidget( enableServingsCheckBox );

	servingsFrame = new QFrame( servingsBox, "servingsFrame" );
	servingsFrame->setEnabled( FALSE );
	servingsFrame->setFrameShape( QFrame::StyledPanel );
	servingsFrame->setFrameShadow( QFrame::Raised );
	servingsFrame->setLineWidth( 0 );
	servingsFrameLayout = new QHBoxLayout( servingsFrame, 0, 0, "servingsFrameLayout" );

	servingsComboBox = new QComboBox( FALSE, servingsFrame, "servingsComboBox" );
	servingsComboBox->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 1, ( QSizePolicy::SizeType ) 0, 1, 0, servingsComboBox->sizePolicy().hasHeightForWidth() ) );
	servingsFrameLayout->addWidget( servingsComboBox );

	servingsSpinBox = new QSpinBox( servingsFrame, "servingsSpinBox" );
	servingsSpinBox->setMinValue( 1 );
	servingsFrameLayout->addWidget( servingsSpinBox );
	servingsBoxLayout->addWidget( servingsFrame );
	servPrepTabLayout->addWidget( servingsBox );

	prepTimeBox = new QGroupBox( servPrepTab, "prepTimeBox" );
	prepTimeBox->setAlignment( int( QGroupBox::WordBreak | QGroupBox::AlignJustify | QGroupBox::AlignCenter | QGroupBox::AlignRight | QGroupBox::AlignLeft ) );
	prepTimeBox->setColumnLayout( 0, Qt::Vertical );
	prepTimeBox->layout() ->setSpacing( 6 );
	prepTimeBox->layout() ->setMargin( 11 );
	prepTimeBoxLayout = new QVBoxLayout( prepTimeBox->layout() );
	prepTimeBoxLayout->setAlignment( Qt::AlignTop );

	enablePrepTimeCheckBox = new QCheckBox( prepTimeBox, "enablePrepTimeCheckBox" );
	prepTimeBoxLayout->addWidget( enablePrepTimeCheckBox );

	prepFrame = new QFrame( prepTimeBox, "prepFrame" );
	prepFrame->setEnabled( FALSE );
	prepFrame->setFrameShape( QFrame::NoFrame );
	prepFrame->setFrameShadow( QFrame::Plain );
	prepFrame->setLineWidth( 0 );
	prepFrameLayout = new QHBoxLayout( prepFrame, 0, 0, "prepFrameLayout" );

	prepTimeComboBox = new QComboBox( FALSE, prepFrame, "prepTimeComboBox" );
	prepTimeComboBox->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 1, ( QSizePolicy::SizeType ) 0, 1, 0, prepTimeComboBox->sizePolicy().hasHeightForWidth() ) );
	prepFrameLayout->addWidget( prepTimeComboBox );

	prepTimeEdit = new QTimeEdit( prepFrame, "prepTimeEdit" );
	prepTimeEdit->setDisplay( int( QTimeEdit::Minutes | QTimeEdit::Hours ) );
	prepFrameLayout->addWidget( prepTimeEdit );
	prepTimeBoxLayout->addWidget( prepFrame );
	servPrepTabLayout->addWidget( prepTimeBox );
	paramsTabWidget->insertTab( servPrepTab, QString( "" ) );

	authorTab = new QWidget( paramsTabWidget, "authorTab" );
	authorTabLayout = new QHBoxLayout( authorTab, 4, 4, "authorTabLayout" );

	layout15 = new QVBoxLayout( 0, 0, 6, "layout15" );

	enableAuthorCheckBox = new QCheckBox( authorTab, "enableAuthorCheckBox" );
	layout15->addWidget( enableAuthorCheckBox );
	spacer4 = new QSpacerItem( 21, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout15->addItem( spacer4 );

	authorSelectAllButton = new QPushButton( authorTab, "authorSelectAllButton" );
	layout15->addWidget( authorSelectAllButton );

	authorUnselectAllButton = new QPushButton( authorTab, "authorUnselectAllButton" );
	layout15->addWidget( authorUnselectAllButton );
	authorTabLayout->addLayout( layout15 );

	authorsFrame = new QFrame( authorTab, "authorsFrame" );
	authorsFrame->setEnabled( FALSE );
	authorsFrame->setFrameShape( QFrame::StyledPanel );
	authorsFrame->setFrameShadow( QFrame::Raised );
	authorsFrame->setLineWidth( 0 );
	authorsFrameLayout = new QVBoxLayout( authorsFrame, 0, 0, "authorsFrameLayout" );

	authorTypeComboBox = new QComboBox( FALSE, authorsFrame, "authorTypeComboBox" );
	authorsFrameLayout->addWidget( authorTypeComboBox );

	authorListView = new DualAuthorListView( authorsFrame, database );
	authorListView->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 5, 1, 0, authorListView->sizePolicy().hasHeightForWidth() ) );
	authorListView->setAllColumnsShowFocus( TRUE );
	authorsFrameLayout->addWidget( authorListView );
	authorTabLayout->addWidget( authorsFrame );
	paramsTabWidget->insertTab( authorTab, QString( "" ) );
	layout17->addWidget( paramsTabWidget );

	layout9 = new QVBoxLayout( 0, 0, 6, "layout9" );

	findButton = new KPushButton( this, "findButton" );
	findButton->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 1, ( QSizePolicy::SizeType ) 0, 0, 0, findButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( findButton );

	clearButton = new KPushButton( this, "clearButton" );
	clearButton->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 1, ( QSizePolicy::SizeType ) 0, 0, 0, clearButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( clearButton );
	spacer7 = new QSpacerItem( 21, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout9->addItem( spacer7 );
	layout17->addLayout( layout9 );
	AdvancedSearchDialogLayout->addLayout( layout17 );

	resultsListView = new KListView( this, "resultsListView" );
	resultsListView->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 7, 0, 1, resultsListView->sizePolicy().hasHeightForWidth() ) );
	AdvancedSearchDialogLayout->addWidget( resultsListView );
	languageChange();
	clearWState( WState_Polished );
	///
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	///

	authorListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	catListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	ingListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	resultsListView->addColumn( i18n( "Title" ) );
	resultsListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( resultsListView, database, RecipeActionsHandler::Open | RecipeActionsHandler::Edit | RecipeActionsHandler::Export );

	connect( findButton, SIGNAL( clicked() ), SLOT( search() ) );
	connect( clearButton, SIGNAL( clicked() ), resultsListView, SLOT( clear() ) );

	connect( enableAuthorCheckBox, SIGNAL( toggled( bool ) ), authorsFrame, SLOT( setEnabled( bool ) ) );
	connect( enableCatCheckBox, SIGNAL( toggled( bool ) ), categoriesFrame, SLOT( setEnabled( bool ) ) );
	connect( enableIngCheckBox, SIGNAL( toggled( bool ) ), ingredientsFrame, SLOT( setEnabled( bool ) ) );
	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsFrame, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepFrame, SLOT( setEnabled( bool ) ) );

	connect( authorTypeComboBox, SIGNAL( activated( int ) ), authorListView, SLOT( change( int ) ) );
	connect( ingTypeComboBox, SIGNAL( activated( int ) ), ingListView, SLOT( change( int ) ) );
	connect( catTypeComboBox, SIGNAL( activated( int ) ), catListView, SLOT( change( int ) ) );
	connect( authorSelectAllButton, SIGNAL( clicked() ), SLOT( selectAllAuthors() ) );
	connect( catSelectAllButton, SIGNAL( clicked() ), SLOT( selectAllCategories() ) );
	connect( ingSelectAllButton, SIGNAL( clicked() ), SLOT( selectAllIngredients() ) );
	connect( authorUnselectAllButton, SIGNAL( clicked() ), SLOT( unselectAllAuthors() ) );
	connect( catUnselectAllButton, SIGNAL( clicked() ), SLOT( unselectAllCategories() ) );
	connect( ingUnselectAllButton, SIGNAL( clicked() ), SLOT( unselectAllIngredients() ) );
	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );

	reload();
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{}

void AdvancedSearchDialog::languageChange()
{
	enableIngCheckBox->setText( i18n( "Enabled" ) );
	ingTypeComboBox->clear();
	ingTypeComboBox->insertItem( i18n( "Use:" ) );
	ingTypeComboBox->insertItem( i18n( "Do not use:" ) );
	QWhatsThis::add
		( ingTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe uses certain ingredients.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	ingListView->header() ->setLabel( 0, i18n( "Name" ) );
	ingListView->header() ->setLabel( 1, i18n( "Id" ) );
	ingSelectAllButton->setText( i18n( "Select All" ) );
	ingUnselectAllButton->setText( i18n( "Unselect All" ) );
	paramsTabWidget->changeTab( ingTab, i18n( "Ingredients" ) );
	enableCatCheckBox->setText( i18n( "Enabled" ) );
	catTypeComboBox->clear();
	catTypeComboBox->insertItem( i18n( "Belong to:" ) );
	catTypeComboBox->insertItem( i18n( "Do not belong to:" ) );
	QWhatsThis::add
		( catTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe belongs to certain categories.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	catListView->header() ->setLabel( 0, i18n( "Name" ) );
	catListView->header() ->setLabel( 1, i18n( "Id" ) );
	catSelectAllButton->setText( i18n( "Select All" ) );
	catUnselectAllButton->setText( i18n( "Unselect All" ) );
	paramsTabWidget->changeTab( catTab, i18n( "Categories" ) );
	servingsBox->setTitle( i18n( "Servings" ) );
	enableServingsCheckBox->setText( i18n( "Enabled" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Serves at least:" ) );
	servingsComboBox->insertItem( i18n( "Serves at most:" ) );
	servingsComboBox->insertItem( i18n( "Serves exactly:" ) );
	prepTimeBox->setTitle( i18n( "Preparation Time" ) );
	enablePrepTimeCheckBox->setText( i18n( "Enabled" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( i18n( "Ready in at least:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in more than:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in exactly:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in about:" ) );
	paramsTabWidget->changeTab( servPrepTab, i18n( "Servings/Preparation Time" ) );
	enableAuthorCheckBox->setText( i18n( "Enabled" ) );
	authorTypeComboBox->clear();
	authorTypeComboBox->insertItem( i18n( "By:" ) );
	authorTypeComboBox->insertItem( i18n( "Not by:" ) );
	QWhatsThis::add
		( authorTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe is by certain authors.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	authorListView->header() ->setLabel( 0, i18n( "Name" ) );
	authorListView->header() ->setLabel( 1, i18n( "Id" ) );
	authorSelectAllButton->setText( i18n( "Select All" ) );
	authorUnselectAllButton->setText( i18n( "Unselect All" ) );
	paramsTabWidget->changeTab( authorTab, i18n( "Authors" ) );
	findButton->setText( i18n( "Find" ) );
	clearButton->setText( i18n( "C&lear" ) );
	clearButton->setAccel( QKeySequence( i18n( "Alt+L" ) ) );
	resultsListView->header() ->setLabel( 0, i18n( "Id" ) );
	resultsListView->header() ->setLabel( 1, i18n( "Title" ) );
}

void AdvancedSearchDialog::selectAllAuthors()
{
	for ( QCheckListItem * qlv_it = static_cast<QCheckListItem*>( authorListView->firstChild() ); qlv_it ; qlv_it = static_cast<QCheckListItem*>( qlv_it->nextSibling() ) ) {
		if ( qlv_it->isEnabled() )
			qlv_it->setOn( true );
	}
}

void AdvancedSearchDialog::selectAllCategories()
{
	QCheckListItem * current_item;
	QListViewItemIterator it( catListView );
	while ( it.current() ) {
		current_item = ( QCheckListItem* ) it.current();

		if ( current_item->isEnabled() )
			current_item->setOn( true );

		++it;
	}
}

void AdvancedSearchDialog::selectAllIngredients()
{
	for ( QCheckListItem * qlv_it = static_cast<QCheckListItem*>( ingListView->firstChild() ); qlv_it ; qlv_it = static_cast<QCheckListItem*>( qlv_it->nextSibling() ) ) {
		if ( qlv_it->isEnabled() )
			qlv_it->setOn( true );
	}
}

void AdvancedSearchDialog::unselectAllAuthors()
{
	for ( QCheckListItem * qlv_it = static_cast<QCheckListItem*>( authorListView->firstChild() ); qlv_it ; qlv_it = static_cast<QCheckListItem*>( qlv_it->nextSibling() ) ) {
		if ( qlv_it->isEnabled() )
			qlv_it->setOn( false );
	}
}

void AdvancedSearchDialog::unselectAllCategories()
{
	QCheckListItem * current_item;
	QListViewItemIterator it( catListView );
	while ( it.current() ) {
		current_item = ( QCheckListItem* ) it.current();

		if ( current_item->isEnabled() )
			current_item->setOn( false );

		++it;
	}
}

void AdvancedSearchDialog::unselectAllIngredients()
{
	for ( QCheckListItem * qlv_it = static_cast<QCheckListItem*>( ingListView->firstChild() ); qlv_it ; qlv_it = static_cast<QCheckListItem*>( qlv_it->nextSibling() ) ) {
		if ( qlv_it->isEnabled() )
			qlv_it->setOn( false );
	}
}

void AdvancedSearchDialog::reload()
{
	authorListView->reload();
	catListView->reload();
	ingListView->reload();
}

void AdvancedSearchDialog::search()
{
	KApplication::setOverrideCursor( KCursor::waitCursor() );

	//we'll load all the recipes and whittle down the list based on constraints
	RecipeList allRecipes;
	database->loadRecipeDetails( &allRecipes, true, true, false, true );

	//narrow down by servings
	if ( enableServingsCheckBox->isChecked() ) {
		int servings = servingsSpinBox->value();
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			switch ( servingsComboBox->currentItem() ) {
				//at least
			case 0:
				if ( ( *it ).persons < servings ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
				//at most
			case 1:
				if ( ( *it ).persons > servings ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
				//exactly
			case 2:
				if ( ( *it ).persons != servings ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
				//about
			case 3:
				if ( QABS( ( *it ).persons - servings ) > 5 ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
			}
		}
	}

	//narrow down by prep time
	if ( enablePrepTimeCheckBox->isChecked() ) {
		QTime time = prepTimeEdit->time();
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			switch ( prepTimeComboBox->currentItem() ) {
				//at least
			case 0:
				if ( ( *it ).prepTime > time ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
				//at most
			case 1:
				if ( ( *it ).prepTime < time ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
				//exactly
			case 2:
				if ( ( *it ).prepTime != time ) {
					it = allRecipes.remove( it );
					it--;
				}
				break;
				//about
			case 3: {
					int rec_minutes = ( *it ).prepTime.minute() + ( *it ).prepTime.hour() * 60;
					int test_minutes = time.minute() + time.hour() * 60;

					//TODO: have a configurable 'about'.  It tests within 15 minutes for now.
					if ( QABS( test_minutes - rec_minutes ) > 15 ) {
						it = allRecipes.remove( it );
						it--;
					}
					break;
				}
			}
		}
	}

	//narrow down by authors
	if ( enableAuthorCheckBox->isChecked() ) {
		//positive search
		for ( QValueList<Element>::const_iterator author_it = authorListView->positiveSelections.begin(); author_it != authorListView->positiveSelections.end(); ++author_it ) {
			for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
				Element i;
				i.id = (*author_it).id;
				if ( ( *it ).authorList.find( i ) == ( *it ).authorList.end() ) {
					it = allRecipes.remove( it );
					it--;
				}
			}
		}

		//negative search
		for ( QValueList<Element>::const_iterator author_it = authorListView->negativeSelections.begin(); author_it != authorListView->negativeSelections.end(); ++author_it ) {
			for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
				Element i;
				i.id = (*author_it).id;
				if ( ( *it ).authorList.find( i ) != ( *it ).authorList.end() ) {
					it = allRecipes.remove( it );
					it--;
				}
			}
		}
	}

	//narrow down by categories
	if ( enableCatCheckBox->isChecked() ) {
		//positive search
		for ( QValueList<Element>::const_iterator cat_it = catListView->positiveSelections.begin(); cat_it != catListView->positiveSelections.end(); ++cat_it ) {
			for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
				Element i;
				i.id = (*cat_it).id;
				if ( ( *it ).categoryList.find( i ) == ( *it ).categoryList.end() ) {
					it = allRecipes.remove( it );
					it--;
				}
			}
		}

		//negative search
		for ( QValueList<Element>::const_iterator cat_it = catListView->negativeSelections.begin(); cat_it != catListView->negativeSelections.end(); ++cat_it ) {
			for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
				Element i;
				i.id = (*cat_it).id;
				if ( ( *it ).categoryList.find( i ) != ( *it ).categoryList.end() ) {
					it = allRecipes.remove( it );
					it--;
				}
			}
		}
	}

	//narrow down by ingredients
	if ( enableIngCheckBox->isChecked() ) {
		//positive search
		for ( QValueList<Element>::const_iterator ing_it = ingListView->positiveSelections.begin(); ing_it != ingListView->positiveSelections.end(); ++ing_it ) {
			for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
				int id = (*ing_it).id;
				if ( ( *it ).ingList.find( id ) == -1 ) {
					it = allRecipes.remove( it );
					it--;
				}
			}
		}

		//negative search
		for ( QValueList<Element>::const_iterator ing_it = ingListView->negativeSelections.begin(); ing_it != ingListView->negativeSelections.end(); ++ing_it ) {
			for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
				int id = (*ing_it).id;
				if ( ( *it ).ingList.find( id ) != -1 ) {
					it = allRecipes.remove( it );
					it--;
				}
			}
		}
	}

	//now display the recipes left
	resultsListView->clear();
	for ( RecipeList::const_iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
		( void ) new RecipeListItem( resultsListView, *it );
	}

	KApplication::restoreOverrideCursor();
}

#include "advancedsearchdialog.moc"
