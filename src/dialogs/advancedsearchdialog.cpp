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
#include <qlabel.h>
#include <qlineedit.h>
#include <qscrollview.h>

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

AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent ),
		database( db )
{
	///
	///BEGIN OF AUTOMATICALLY GENERATED GUI CODE///
	///
	AdvancedSearchDialogLayout = new QHBoxLayout( this, 11, 6, "AdvancedSearchDialogLayout"); 
	
	layout7 = new QVBoxLayout( 0, 0, 6, "layout7"); 
	
	textLabel1_4 = new QLabel( this, "textLabel1_4" );
	layout7->addWidget( textLabel1_4 );
	
	scrollView1 = new QScrollView( this, "scrollView1" );
	scrollView1->enableClipper(true);
	
	parametersFrame = new QFrame( scrollView1, "parametersFrame" );
	parametersFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, parametersFrame->sizePolicy().hasHeightForWidth() ) );
	parametersFrame->setFrameShape( QFrame::NoFrame );
	parametersFrame->setFrameShadow( QFrame::Plain );
	parametersFrame->setLineWidth( 0 );
	parametersFrameLayout = new QVBoxLayout( parametersFrame, 0, 0, "parametersFrameLayout"); 
	
	ingredientButton = new QPushButton( parametersFrame, "ingredientButton" );
	ingredientButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( ingredientButton );
	
	ingredientFrame = new QFrame( parametersFrame, "ingredientFrame" );
	ingredientFrame->setFrameShape( QFrame::StyledPanel );
	ingredientFrame->setFrameShadow( QFrame::Raised );
	ingredientFrameLayout = new QGridLayout( ingredientFrame, 1, 1, 11, 6, "ingredientFrameLayout"); 
	
	ingredientsAllEdit = new QLineEdit( ingredientFrame, "ingredientsAllEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAllEdit, 0, 1 );
	
	ingredientsAnyEdit = new QLineEdit( ingredientFrame, "ingredientsAnyEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAnyEdit, 1, 1 );
	
	textLabel1_2 = new QLabel( ingredientFrame, "textLabel1_2" );
	
	ingredientFrameLayout->addWidget( textLabel1_2, 1, 0 );
	
	textLabel1 = new QLabel( ingredientFrame, "textLabel1" );
	
	ingredientFrameLayout->addWidget( textLabel1, 0, 0 );
	
	ingredientsWithoutEdit = new QLineEdit( ingredientFrame, "ingredientsWithoutEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsWithoutEdit, 2, 1 );
	
	textLabel1_3 = new QLabel( ingredientFrame, "textLabel1_3" );
	
	ingredientFrameLayout->addWidget( textLabel1_3, 2, 0 );
	parametersFrameLayout->addWidget( ingredientFrame );
	spacer3_2_3_2_2 = new QSpacerItem( 31, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2_2 );
	
	categoriesButton = new QPushButton( parametersFrame, "categoriesButton" );
	categoriesButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( categoriesButton );
	
	categoryFrame = new QFrame( parametersFrame, "categoryFrame" );
	categoryFrame->setFrameShape( QFrame::StyledPanel );
	categoryFrame->setFrameShadow( QFrame::Raised );
	categoryFrameLayout = new QGridLayout( categoryFrame, 1, 1, 11, 6, "categoryFrameLayout"); 
	
	categoriesAllEdit = new QLineEdit( categoryFrame, "categoriesAllEdit" );
	
	categoryFrameLayout->addWidget( categoriesAllEdit, 0, 2 );
	
	textLabel1_5 = new QLabel( categoryFrame, "textLabel1_5" );
	
	categoryFrameLayout->addMultiCellWidget( textLabel1_5, 0, 0, 0, 1 );
	
	textLabel1_3_3 = new QLabel( categoryFrame, "textLabel1_3_3" );
	
	categoryFrameLayout->addWidget( textLabel1_3_3, 2, 0 );
	
	categoriesAnyEdit = new QLineEdit( categoryFrame, "categoriesAnyEdit" );
	
	categoryFrameLayout->addWidget( categoriesAnyEdit, 1, 2 );
	
	textLabel1_2_3 = new QLabel( categoryFrame, "textLabel1_2_3" );
	
	categoryFrameLayout->addMultiCellWidget( textLabel1_2_3, 1, 1, 0, 1 );
	
	categoriesNotEdit = new QLineEdit( categoryFrame, "categoriesNotEdit" );
	
	categoryFrameLayout->addMultiCellWidget( categoriesNotEdit, 2, 2, 1, 2 );
	parametersFrameLayout->addWidget( categoryFrame );
	spacer3_2_3_2 = new QSpacerItem( 31, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2 );
	
	authorsButton = new QPushButton( parametersFrame, "authorsButton" );
	authorsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( authorsButton );
	
	authorsFrame = new QFrame( parametersFrame, "authorsFrame" );
	authorsFrame->setFrameShape( QFrame::StyledPanel );
	authorsFrame->setFrameShadow( QFrame::Raised );
	authorsFrameLayout = new QGridLayout( authorsFrame, 1, 1, 11, 6, "authorsFrameLayout"); 
	
	textLabel1_2_4 = new QLabel( authorsFrame, "textLabel1_2_4" );
	
	authorsFrameLayout->addWidget( textLabel1_2_4, 1, 0 );
	
	textLabel1_6 = new QLabel( authorsFrame, "textLabel1_6" );
	
	authorsFrameLayout->addMultiCellWidget( textLabel1_6, 0, 0, 0, 1 );
	
	textLabel1_3_4 = new QLabel( authorsFrame, "textLabel1_3_4" );
	
	authorsFrameLayout->addMultiCellWidget( textLabel1_3_4, 2, 2, 0, 2 );
	
	authorsAnyEdit = new QLineEdit( authorsFrame, "authorsAnyEdit" );
	
	authorsFrameLayout->addMultiCellWidget( authorsAnyEdit, 1, 1, 1, 3 );
	
	authorsAllEdit = new QLineEdit( authorsFrame, "authorsAllEdit" );
	
	authorsFrameLayout->addMultiCellWidget( authorsAllEdit, 0, 0, 2, 3 );
	
	authorsWithoutEdit = new QLineEdit( authorsFrame, "authorsWithoutEdit" );
	
	authorsFrameLayout->addWidget( authorsWithoutEdit, 2, 3 );
	parametersFrameLayout->addWidget( authorsFrame );
	spacer3_2_3 = new QSpacerItem( 31, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3 );
	
	servingsButton = new QPushButton( parametersFrame, "servingsButton" );
	servingsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( servingsButton );
	
	servingsFrame = new QFrame( parametersFrame, "servingsFrame" );
	servingsFrame->setFrameShape( QFrame::StyledPanel );
	servingsFrame->setFrameShadow( QFrame::Raised );
	servingsFrameLayout = new QVBoxLayout( servingsFrame, 11, 6, "servingsFrameLayout"); 
	
	enableServingsCheckBox = new QCheckBox( servingsFrame, "enableServingsCheckBox" );
	servingsFrameLayout->addWidget( enableServingsCheckBox );
	
	layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 
	
	servingsComboBox = new QComboBox( FALSE, servingsFrame, "servingsComboBox" );
	servingsComboBox->setEnabled( FALSE );
	servingsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, servingsComboBox->sizePolicy().hasHeightForWidth() ) );
	layout5->addWidget( servingsComboBox );
	
	servingsSpinBox = new QSpinBox( servingsFrame, "servingsSpinBox" );
	servingsSpinBox->setEnabled( FALSE );
	servingsSpinBox->setMinValue( 1 );
	layout5->addWidget( servingsSpinBox );
	servingsFrameLayout->addLayout( layout5 );
	parametersFrameLayout->addWidget( servingsFrame );
	spacer3_2_2 = new QSpacerItem( 31, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_2 );
	
	prepTimeButton = new QPushButton( parametersFrame, "prepTimeButton" );
	prepTimeButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( prepTimeButton );
	
	prepTimeFrame = new QFrame( parametersFrame, "prepTimeFrame" );
	prepTimeFrame->setFrameShape( QFrame::StyledPanel );
	prepTimeFrame->setFrameShadow( QFrame::Raised );
	prepTimeFrameLayout = new QVBoxLayout( prepTimeFrame, 11, 6, "prepTimeFrameLayout"); 
	
	enablePrepTimeCheckBox = new QCheckBox( prepTimeFrame, "enablePrepTimeCheckBox" );
	prepTimeFrameLayout->addWidget( enablePrepTimeCheckBox );
	
	layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 
	
	prepTimeComboBox = new QComboBox( FALSE, prepTimeFrame, "prepTimeComboBox" );
	prepTimeComboBox->setEnabled( FALSE );
	prepTimeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, prepTimeComboBox->sizePolicy().hasHeightForWidth() ) );
	layout6->addWidget( prepTimeComboBox );
	
	prepTimeEdit = new QTimeEdit( prepTimeFrame, "prepTimeEdit" );
	prepTimeEdit->setEnabled( FALSE );
	prepTimeEdit->setDisplay( int( QTimeEdit::Minutes | QTimeEdit::Hours ) );
	layout6->addWidget( prepTimeEdit );
	prepTimeFrameLayout->addLayout( layout6 );
	parametersFrameLayout->addWidget( prepTimeFrame );
	spacer15 = new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
	parametersFrameLayout->addItem( spacer15 );
	scrollView1->addChild( parametersFrame );
	layout7->addWidget( scrollView1 );
	
	layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 
	
	clearButton = new KPushButton( this, "clearButton" );
	clearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, clearButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( clearButton );
	spacer3 = new QSpacerItem( 136, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout9->addItem( spacer3 );
	
	findButton = new KPushButton( this, "findButton" );
	findButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, findButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( findButton );
	layout7->addLayout( layout9 );
	AdvancedSearchDialogLayout->addLayout( layout7 );
	
	resultsListView = new KListView( this, "resultsListView" );
	resultsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 1, resultsListView->sizePolicy().hasHeightForWidth() ) );
	AdvancedSearchDialogLayout->addWidget( resultsListView );
	languageChange();
	clearWState( WState_Polished );
	///
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	///

	resultsListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

	scrollView1->setHScrollBarMode( QScrollView::AlwaysOff );
	scrollView1->setResizePolicy( QScrollView::AutoOneFit );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	resultsListView->addColumn( i18n( "Title" ) );
	resultsListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( resultsListView, database, RecipeActionsHandler::Open | RecipeActionsHandler::Edit | RecipeActionsHandler::Export );

	connect( findButton, SIGNAL( clicked() ), SLOT( search() ) );
	connect( clearButton, SIGNAL( clicked() ), SLOT( clear() ) );

	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsSpinBox, SLOT( setEnabled( bool ) ) );
	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsComboBox, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepTimeEdit, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepTimeComboBox, SLOT( setEnabled( bool ) ) );

	connect( ingredientButton, SIGNAL( toggled( bool ) ), ingredientFrame, SLOT( setShown( bool ) ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), authorsFrame, SLOT( setShown( bool ) ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), categoryFrame, SLOT( setShown( bool ) ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), servingsFrame, SLOT( setShown( bool ) ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), prepTimeFrame, SLOT( setShown( bool ) ) );
	
	ingredientFrame->setShown(false);
	authorsFrame->setShown(false);
	categoryFrame->setShown(false);
	servingsFrame->setShown(false);
	prepTimeFrame->setShown(false);

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{}

void AdvancedSearchDialog::languageChange()
{
	textLabel1_4->setText( i18n( "Search using the following criteria:" ) );
	ingredientButton->setText( i18n( "Ingredients >>" ) );
	textLabel1_2->setText( i18n( "Uses any of the following:" ) );
	textLabel1->setText( i18n( "Uses all the following:" ) );
	textLabel1_3->setText( i18n( "Without the following:" ) );
	categoriesButton->setText( i18n( "Categories >>" ) );
	textLabel1_5->setText( i18n( "In all the following:" ) );
	textLabel1_3_3->setText( i18n( "Not in the following:" ) );
	textLabel1_2_3->setText( i18n( "In any of the following:" ) );
	authorsButton->setText( i18n( "Authors >>" ) );
	textLabel1_2_4->setText( i18n( "By any of the following:" ) );
	textLabel1_6->setText( i18n( "By all the following:" ) );
	textLabel1_3_4->setText( i18n( "Not by the following:" ) );
	servingsButton->setText( i18n( "Servings >>" ) );
	enableServingsCheckBox->setText( i18n( "Enabled" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Serves at least:" ) );
	servingsComboBox->insertItem( i18n( "Serves at most:" ) );
	servingsComboBox->insertItem( i18n( "Serves about:" ) );
	prepTimeButton->setText( i18n( "Preparation Time >>" ) );
	enablePrepTimeCheckBox->setText( i18n( "Enabled" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( i18n( "Ready in at least:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in more than:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in about:" ) );
	clearButton->setText( i18n( "C&lear" ) );
	clearButton->setAccel( QKeySequence( i18n( "Alt+L" ) ) );
	findButton->setText( i18n( "Search" ) );
}

void AdvancedSearchDialog::clear()
{
	resultsListView->clear();
	authorsAllEdit->clear();
	authorsWithoutEdit->clear();
	categoriesAllEdit->clear();
	categoriesNotEdit->clear();
	ingredientsAllEdit->clear();
	ingredientsWithoutEdit->clear();
}

void AdvancedSearchDialog::search()
{
	KApplication::setOverrideCursor( KCursor::waitCursor() );

	//we need to load more than just the title because we'll be doing further refining of the search
	int load_items = RecipeDB::Title;
	if ( !authorsAllEdit->text().isEmpty() || !authorsWithoutEdit->text().isEmpty() )
		load_items |= RecipeDB::Authors;
	if ( !ingredientsAllEdit->text().isEmpty() || !ingredientsWithoutEdit->text().isEmpty() )
		load_items |= RecipeDB::Ingredients;
	if ( !categoriesAllEdit->text().isEmpty() || !categoriesNotEdit->text().isEmpty() )
		load_items |= RecipeDB::Categories;

	RecipeList allRecipes;
	database->search( &allRecipes, load_items,
		"", //title
		"", //instructions
		split(ingredientsAnyEdit->text()),
		split(categoriesAnyEdit->text()),
		split(authorsAnyEdit->text()),
		enablePrepTimeCheckBox->isChecked()?prepTimeEdit->time():QTime(),
		prepTimeComboBox->currentItem(), //prep_param
		enableServingsCheckBox->isChecked()?servingsSpinBox->value():-1, //servings
		servingsComboBox->currentItem() //servings_param
	);

	/*
	 * Ideally, the following would be done by the above SQL query, but I have no idea how to accomplish this.
	 */

	//narrow down by authors
	QStringList items = split(authorsAllEdit->text());
	for ( QStringList::const_iterator author_it = items.begin(); author_it != items.end(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( *author_it ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(authorsWithoutEdit->text());
	for ( QStringList::const_iterator author_it = items.begin(); author_it != items.end(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( *author_it ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by categories
	items = split(categoriesAllEdit->text());
	for ( QStringList::const_iterator cat_it = items.begin(); cat_it != items.end(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( *cat_it ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(categoriesNotEdit->text());
	for ( QStringList::const_iterator cat_it = items.begin(); cat_it != items.end(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( *cat_it ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by ingredients
	items = split(ingredientsAllEdit->text());
	for ( QStringList::const_iterator ing_it = items.begin(); ing_it != items.end(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( *ing_it ).ingredientID == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(ingredientsWithoutEdit->text());
	for ( QStringList::const_iterator ing_it = items.begin(); ing_it != items.end(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( *ing_it ).ingredientID != -1 ) {
				it = allRecipes.remove( it );
				it--;
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

QStringList AdvancedSearchDialog::split( const QString &text ) const
{
	QStringList result;

	// To keep quoted words together, first split on quotes,
	// and then split again on the even numbered items
	
	QStringList temp = QStringList::split('"',text,true);
	for ( int i = 0; i < temp.count(); ++i ) {
		if ( i & 1 ) //odd
			result += temp[i].stripWhiteSpace();
		else         //even
			result += QStringList::split(' ',temp[i]);
	}

	for ( QStringList::iterator it = result.begin(); it != result.end(); ++it ) {
		(*it).replace("*","%");
		(*it).replace("?","_");
	}

	return result;
}

#include "advancedsearchdialog.moc"
