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
#include <q3frame.h>
#include <qcombobox.h>
#include <q3header.h>
#include <q3listview.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <q3datetimeedit.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <kpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3scrollview.h>
#include <q3hbox.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

#include "backends/recipedb.h"
#include "recipeactionshandler.h"
#include "widgets/recipelistview.h"

AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent ),
		database( db )
{
	///
	///BEGIN OF AUTOMATICALLY GENERATED GUI CODE///
	///
	AdvancedSearchDialogLayout = new QHBoxLayout( this, 5, 3, "AdvancedSearchDialogLayout"); 
	
	layout7 = new QVBoxLayout( 0, 0, 3, "layout7"); 
	
	textLabel1_4 = new QLabel( this, "textLabel1_4" );
	layout7->addWidget( textLabel1_4 );
	
	scrollView1 = new Q3ScrollView( this, "scrollView1" );
	scrollView1->enableClipper(true);
	
	parametersFrame = new Q3Frame( scrollView1, "parametersFrame" );
	parametersFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, parametersFrame->sizePolicy().hasHeightForWidth() ) );
	parametersFrame->setFrameShape( Q3Frame::NoFrame );
	parametersFrame->setFrameShadow( Q3Frame::Plain );
	parametersFrame->setLineWidth( 0 );
	parametersFrameLayout = new QVBoxLayout( parametersFrame, 0, 0, "parametersFrameLayout"); 

	titleButton = new QPushButton( parametersFrame, "titleButton" );
	titleButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( titleButton );
	
	titleFrame = new Q3Frame( parametersFrame, "titleFrame" );
	titleFrame->setFrameShape( Q3Frame::StyledPanel );
	titleFrame->setFrameShadow( Q3Frame::Raised );
	titleFrameLayout = new QVBoxLayout( titleFrame, 5, 3, "titleFrameLayout"); 

	requireAllTitle = new QCheckBox( i18n("Require All Words"), titleFrame );
	titleFrameLayout->addWidget( requireAllTitle );

	Q3HBox *titleHBox = new Q3HBox( titleFrame );
	QLabel *titleInfoLabel = new QLabel(i18n("Keywords:"),titleHBox);
	titleEdit = new QLineEdit( titleHBox, "titleEdit" );
	titleFrameLayout->addWidget( titleHBox );

	parametersFrameLayout->addWidget( titleFrame );
	titleFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( titleFrameSpacer );


	ingredientButton = new QPushButton( parametersFrame, "ingredientButton" );
	ingredientButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( ingredientButton );
	
	ingredientFrame = new Q3Frame( parametersFrame, "ingredientFrame" );
	ingredientFrame->setFrameShape( Q3Frame::StyledPanel );
	ingredientFrame->setFrameShadow( Q3Frame::Raised );
	ingredientFrameLayout = new QGridLayout( ingredientFrame, 1, 1, 3, 3, "ingredientFrameLayout"); 

	QLabel *ingredientInfoLabel = new QLabel(i18n("Enter ingredients: (e.g. chicken pasta \"white wine\")"),ingredientFrame);
	ingredientInfoLabel->setTextFormat( Qt::RichText );
	ingredientFrameLayout->addMultiCellWidget( ingredientInfoLabel, 0, 0, 0, 1 );

	ingredientsAllEdit = new QLineEdit( ingredientFrame, "ingredientsAllEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAllEdit, 1, 1 );
	
	ingredientsAnyEdit = new QLineEdit( ingredientFrame, "ingredientsAnyEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAnyEdit, 2, 1 );
	
	textLabel1_2 = new QLabel( ingredientFrame, "textLabel1_2" );
	
	ingredientFrameLayout->addWidget( textLabel1_2, 2, 0 );
	
	textLabel1 = new QLabel( ingredientFrame, "textLabel1" );
	
	ingredientFrameLayout->addWidget( textLabel1, 1, 0 );
	
	ingredientsWithoutEdit = new QLineEdit( ingredientFrame, "ingredientsWithoutEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsWithoutEdit, 3, 1 );
	
	textLabel1_3 = new QLabel( ingredientFrame, "textLabel1_3" );
	
	ingredientFrameLayout->addWidget( textLabel1_3, 3, 0 );
	parametersFrameLayout->addWidget( ingredientFrame );
	spacer3_2_3_2_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2_2 );


	categoriesButton = new QPushButton( parametersFrame, "categoriesButton" );
	categoriesButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( categoriesButton );
	
	categoryFrame = new Q3Frame( parametersFrame, "categoryFrame" );
	categoryFrame->setFrameShape( Q3Frame::StyledPanel );
	categoryFrame->setFrameShadow( Q3Frame::Raised );
	categoryFrameLayout = new QGridLayout( categoryFrame, 1, 1, 3, 3, "categoryFrameLayout");

	QLabel *categoryInfoLabel = new QLabel(i18n("Enter categories: (e.g. Desserts Pastas \"Main Dishes\")"),categoryFrame);
	categoryInfoLabel->setTextFormat( Qt::RichText );
	categoryFrameLayout->addMultiCellWidget( categoryInfoLabel, 0, 0, 0, 1 );
	
	categoriesAllEdit = new QLineEdit( categoryFrame, "categoriesAllEdit" );
	
	categoryFrameLayout->addWidget( categoriesAllEdit, 1, 1 );
	
	textLabel1_5 = new QLabel( categoryFrame, "textLabel1_5" );
	
	categoryFrameLayout->addWidget( textLabel1_5, 1, 0 );
	
	textLabel1_3_3 = new QLabel( categoryFrame, "textLabel1_3_3" );
	
	categoryFrameLayout->addWidget( textLabel1_3_3, 3, 0 );
	
	categoriesAnyEdit = new QLineEdit( categoryFrame, "categoriesAnyEdit" );
	
	categoryFrameLayout->addWidget( categoriesAnyEdit, 2, 1 );
	
	textLabel1_2_3 = new QLabel( categoryFrame, "textLabel1_2_3" );
	
	categoryFrameLayout->addWidget( textLabel1_2_3, 2, 0 );
	
	categoriesNotEdit = new QLineEdit( categoryFrame, "categoriesNotEdit" );
	
	categoryFrameLayout->addWidget( categoriesNotEdit, 3, 1 );
	parametersFrameLayout->addWidget( categoryFrame );
	spacer3_2_3_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2 );


	authorsButton = new QPushButton( parametersFrame, "authorsButton" );
	authorsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( authorsButton );
	
	authorsFrame = new Q3Frame( parametersFrame, "authorsFrame" );
	authorsFrame->setFrameShape( Q3Frame::StyledPanel );
	authorsFrame->setFrameShadow( Q3Frame::Raised );
	authorsFrameLayout = new QGridLayout( authorsFrame, 1, 1, 3, 3, "authorsFrameLayout"); 

	QLabel *authorsInfoLabel = new QLabel(i18n("Enter author name (e.g. Smith or \"Jane Doe\")"),authorsFrame);
	authorsInfoLabel->setTextFormat( Qt::RichText );
	authorsFrameLayout->addMultiCellWidget( authorsInfoLabel, 0, 0, 0, 1 );

	textLabel1_2_4 = new QLabel( authorsFrame, "textLabel1_2_4" );
	
	authorsFrameLayout->addWidget( textLabel1_2_4, 1, 0 );
	
	textLabel1_6 = new QLabel( authorsFrame, "textLabel1_6" );
	
	authorsFrameLayout->addWidget( textLabel1_6, 2, 0 );
	
	textLabel1_3_4 = new QLabel( authorsFrame, "textLabel1_3_4" );
	
	authorsFrameLayout->addWidget( textLabel1_3_4, 3, 0 );
	
	authorsAnyEdit = new QLineEdit( authorsFrame, "authorsAnyEdit" );
	
	authorsFrameLayout->addWidget( authorsAnyEdit, 1, 1 );
	
	authorsAllEdit = new QLineEdit( authorsFrame, "authorsAllEdit" );
	
	authorsFrameLayout->addWidget( authorsAllEdit, 2, 1 );
	
	authorsWithoutEdit = new QLineEdit( authorsFrame, "authorsWithoutEdit" );
	
	authorsFrameLayout->addWidget( authorsWithoutEdit, 3, 1 );
	parametersFrameLayout->addWidget( authorsFrame );
	spacer3_2_3 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3 );


	servingsButton = new QPushButton( parametersFrame, "servingsButton" );
	servingsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( servingsButton );
	
	servingsFrame = new Q3Frame( parametersFrame, "servingsFrame" );
	servingsFrame->setFrameShape( Q3Frame::StyledPanel );
	servingsFrame->setFrameShadow( Q3Frame::Raised );
	servingsFrameLayout = new QVBoxLayout( servingsFrame, 3, 3, "servingsFrameLayout"); 
	
	enableServingsCheckBox = new QCheckBox( servingsFrame, "enableServingsCheckBox" );
	servingsFrameLayout->addWidget( enableServingsCheckBox );
	
	layout5 = new QHBoxLayout( 0, 0, 3, "layout5"); 
	
	servingsComboBox = new QComboBox( FALSE, servingsFrame, "servingsComboBox" );
	servingsComboBox->setEnabled( FALSE );
	servingsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, servingsComboBox->sizePolicy().hasHeightForWidth() ) );
	layout5->addWidget( servingsComboBox );
	
	servingsSpinBox = new QSpinBox( servingsFrame, "servingsSpinBox" );
	servingsSpinBox->setEnabled( FALSE );
	servingsSpinBox->setMinValue( 1 );
	servingsSpinBox->setMaxValue( 9999 );
	layout5->addWidget( servingsSpinBox );
	servingsFrameLayout->addLayout( layout5 );
	parametersFrameLayout->addWidget( servingsFrame );
	spacer3_2_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_2 );


	prepTimeButton = new QPushButton( parametersFrame, "prepTimeButton" );
	prepTimeButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( prepTimeButton );
	
	prepTimeFrame = new Q3Frame( parametersFrame, "prepTimeFrame" );
	prepTimeFrame->setFrameShape( Q3Frame::StyledPanel );
	prepTimeFrame->setFrameShadow( Q3Frame::Raised );
	prepTimeFrameLayout = new QVBoxLayout( prepTimeFrame, 3, 3, "prepTimeFrameLayout"); 
	
	enablePrepTimeCheckBox = new QCheckBox( prepTimeFrame, "enablePrepTimeCheckBox" );
	prepTimeFrameLayout->addWidget( enablePrepTimeCheckBox );
	
	layout6 = new QHBoxLayout( 0, 0, 3, "layout6"); 
	
	prepTimeComboBox = new QComboBox( FALSE, prepTimeFrame, "prepTimeComboBox" );
	prepTimeComboBox->setEnabled( FALSE );
	prepTimeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, prepTimeComboBox->sizePolicy().hasHeightForWidth() ) );
	layout6->addWidget( prepTimeComboBox );
	
	prepTimeEdit = new Q3TimeEdit( prepTimeFrame, "prepTimeEdit" );
	prepTimeEdit->setEnabled( FALSE );
	prepTimeEdit->setDisplay( int( Q3TimeEdit::Minutes | Q3TimeEdit::Hours ) );
	layout6->addWidget( prepTimeEdit );
	prepTimeFrameLayout->addLayout( layout6 );
	parametersFrameLayout->addWidget( prepTimeFrame );
	spacer15 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer15 );


	instructionsButton = new QPushButton( parametersFrame, "instructionsButton" );
	instructionsButton->setToggleButton( TRUE );
	parametersFrameLayout->addWidget( instructionsButton );
	
	instructionsFrame = new Q3Frame( parametersFrame, "instructionsFrame" );
	instructionsFrame->setFrameShape( Q3Frame::StyledPanel );
	instructionsFrame->setFrameShadow( Q3Frame::Raised );
	instructionsFrameLayout = new QVBoxLayout( instructionsFrame, 5, 3, "instructionsFrameLayout"); 

	requireAllInstructions = new QCheckBox( i18n("Require All Words"), instructionsFrame );
	instructionsFrameLayout->addWidget( requireAllInstructions );

	Q3HBox *instructionsHBox = new Q3HBox(instructionsFrame);
	QLabel *instructionsInfoLabel = new QLabel(i18n("Keywords:"),instructionsHBox);

	instructionsEdit = new QLineEdit( instructionsHBox, "instructionsEdit" );
	instructionsFrameLayout->addWidget( instructionsHBox );

	parametersFrameLayout->addWidget( instructionsFrame );
	instructionsFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
	parametersFrameLayout->addItem( instructionsFrameSpacer );


	scrollView1->addChild( parametersFrame );
	layout7->addWidget( scrollView1 );
	
	layout9 = new QHBoxLayout( 0, 0, 3, "layout9"); 
	
	clearButton = new KPushButton( this, "clearButton" );
	clearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, clearButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( clearButton );
	spacer3 = new QSpacerItem( 110, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
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

	///
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	///

	resultsListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	scrollView1->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );

	AdvancedSearchDialogLayout->setStretchFactor( resultsListView, 2 );

	scrollView1->setHScrollBarMode( Q3ScrollView::AlwaysOff );
	scrollView1->setResizePolicy( Q3ScrollView::AutoOneFit );

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

	connect( titleButton, SIGNAL( toggled( bool ) ), titleFrame, SLOT( setShown( bool ) ) );
	connect( ingredientButton, SIGNAL( toggled( bool ) ), ingredientFrame, SLOT( setShown( bool ) ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), authorsFrame, SLOT( setShown( bool ) ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), categoryFrame, SLOT( setShown( bool ) ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), servingsFrame, SLOT( setShown( bool ) ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), prepTimeFrame, SLOT( setShown( bool ) ) );
	connect( instructionsButton, SIGNAL( toggled( bool ) ), instructionsFrame, SLOT( setShown( bool ) ) );

	connect( titleButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( ingredientButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( instructionsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );

	titleFrame->setShown(false);
	ingredientFrame->setShown(false);
	authorsFrame->setShown(false);
	categoryFrame->setShown(false);
	servingsFrame->setShown(false);
	prepTimeFrame->setShown(false);
	instructionsFrame->setShown(false);

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{}

void AdvancedSearchDialog::languageChange()
{
	titleButton->setText( QString("%1 >>").arg(i18n("Title")) );
	textLabel1_4->setText( i18n( "Search using the following criteria:" ) );
	ingredientButton->setText( QString("%1 >>").arg(i18n("Ingredients")) );
	textLabel1_2->setText( i18n( "Uses any of:" ) );
	textLabel1->setText( i18n( "Uses all:" ) );
	textLabel1_3->setText( i18n( "Without:" ) );
	categoriesButton->setText( i18n( "Categories >>" ) );
	textLabel1_5->setText( i18n( "In all:" ) );
	textLabel1_3_3->setText( i18n( "Not in:" ) );
	textLabel1_2_3->setText( i18n( "In any of:" ) );
	authorsButton->setText( QString("%1 >>").arg(i18n("Authors")) );
	textLabel1_2_4->setText( i18n( "By any of:" ) );
	textLabel1_6->setText( i18n( "By all:" ) );
	textLabel1_3_4->setText( i18n( "Not by:" ) );
	servingsButton->setText( QString("%1 >>").arg(i18n("Servings")) );
	enableServingsCheckBox->setText( i18n( "Enabled" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Serves at least:" ) );
	servingsComboBox->insertItem( i18n( "Serves at most:" ) );
	servingsComboBox->insertItem( i18n( "Serves about:" ) );
	prepTimeButton->setText( QString("%1 >>").arg(i18n("Preparation Time")) );
	enablePrepTimeCheckBox->setText( i18n( "Enabled" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( i18n( "Ready before:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in about:" ) );
	instructionsButton->setText( QString("%1 >>").arg(i18n("Instructions")) );
	clearButton->setText( i18n( "C&lear" ) );
	clearButton->setAccel( QKeySequence( i18n( "Alt+L" ) ) );
	findButton->setText( i18n( "Search" ) );
}

void AdvancedSearchDialog::clear()
{
	resultsListView->clear();
	authorsAllEdit->clear();
	authorsWithoutEdit->clear();
	authorsAnyEdit->clear();
	categoriesAllEdit->clear();
	categoriesNotEdit->clear();
	categoriesAnyEdit->clear();
	ingredientsAllEdit->clear();
	ingredientsWithoutEdit->clear();
	ingredientsAnyEdit->clear();
	titleEdit->clear();

	servingsSpinBox->setValue( 1 );
	prepTimeEdit->setTime( QTime(0,0) );

	enablePrepTimeCheckBox->setChecked(false);
	enableServingsCheckBox->setChecked(false);

	requireAllTitle->setChecked(false);
	requireAllInstructions->setChecked(false);
}

void AdvancedSearchDialog::buttonSwitched()
{
	const QObject *sent = sender();

	if ( sent->inherits("QPushButton") ) {
		QPushButton *pushed = (QPushButton*) sent;

		QString suffix = ( pushed->isDown() ) ? " <<" : " >>";
		pushed->setText( pushed->text().left( pushed->text().length() - 3 ) + suffix );
	}
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
		split(titleEdit->text(),true), requireAllTitle->isChecked(), //title
		split(instructionsEdit->text(),true), requireAllInstructions->isChecked(), //instructions
		split(ingredientsAnyEdit->text(),true),
		split(categoriesAnyEdit->text(),true),
		split(authorsAnyEdit->text(),true),
		enablePrepTimeCheckBox->isChecked()?prepTimeEdit->time():QTime(),
		prepTimeComboBox->currentItem(), //prep_param
		enableServingsCheckBox->isChecked()?servingsSpinBox->value():-1, //servings
		servingsComboBox->currentItem() //servings_param
	);

	/*
	 * Ideally, would be done by the above SQL query, but I have no idea how to accomplish this.
	 */

	//narrow down by authors
	QStringList items = split(authorsAllEdit->text());
	for ( QStringList::const_iterator author_it = items.begin(); author_it != items.end(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( QRegExp(*author_it,false, true) ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(authorsWithoutEdit->text());
	for ( QStringList::const_iterator author_it = items.begin(); author_it != items.end(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( QRegExp(*author_it,false,true) ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by categories
	items = split(categoriesAllEdit->text());
	for ( QStringList::const_iterator cat_it = items.begin(); cat_it != items.end(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( QRegExp(*cat_it,false,true) ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(categoriesNotEdit->text());
	for ( QStringList::const_iterator cat_it = items.begin(); cat_it != items.end(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( QRegExp(*cat_it,false,true) ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by ingredients
	items = split(ingredientsAllEdit->text());
	for ( QStringList::const_iterator ing_it = items.begin(); ing_it != items.end(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( QRegExp(*ing_it,false,true) ).ingredientID == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(ingredientsWithoutEdit->text());
	for ( QStringList::const_iterator ing_it = items.begin(); ing_it != items.end(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( QRegExp(*ing_it,false,true) ).ingredientID != -1 ) {
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

	if ( !resultsListView->firstChild() ) {
		( void ) new Q3ListViewItem( resultsListView, "--- "+i18n("No matching recipes found")+" ---");
	}

	KApplication::restoreOverrideCursor();
}

QStringList AdvancedSearchDialog::split( const QString &text, bool sql_wildcards ) const
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

	if ( sql_wildcards ) {
		for ( QStringList::iterator it = result.begin(); it != result.end(); ++it ) {
			(*it).replace("%","\\%");
			(*it).replace("_","\\_");

			(*it).replace("*","%");
			(*it).replace("?","_");
		}
	}

	return result;
}

#include "advancedsearchdialog.moc"
