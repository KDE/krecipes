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
	AdvancedSearchDialogLayout = new QVBoxLayout( this, 11, 6, "AdvancedSearchDialogLayout"); 
	
	layout17 = new QHBoxLayout( 0, 0, 6, "layout17"); 
	
	paramsTabWidget = new QTabWidget( this, "paramsTabWidget" );
	
	ingTab = new QWidget( paramsTabWidget, "ingTab" );
	ingTabLayout = new QGridLayout( ingTab, 1, 1, 11, 6, "ingTabLayout"); 
	
	ingredientsAllEdit = new QLineEdit( ingTab, "ingredientsAllEdit" );
	
	ingTabLayout->addWidget( ingredientsAllEdit, 0, 1 );
	
	ingredientsAnyEdit = new QLineEdit( ingTab, "ingredientsAnyEdit" );
	
	ingTabLayout->addWidget( ingredientsAnyEdit, 1, 1 );
	
	textLabel1_3 = new QLabel( ingTab, "textLabel1_3" );
	
	ingTabLayout->addWidget( textLabel1_3, 2, 0 );
	
	textLabel1_2 = new QLabel( ingTab, "textLabel1_2" );
	
	ingTabLayout->addWidget( textLabel1_2, 1, 0 );
	
	textLabel1 = new QLabel( ingTab, "textLabel1" );
	
	ingTabLayout->addWidget( textLabel1, 0, 0 );
	
	ingredientsWithoutEdit = new QLineEdit( ingTab, "ingredientsWithoutEdit" );
	
	ingTabLayout->addWidget( ingredientsWithoutEdit, 2, 1 );
	paramsTabWidget->insertTab( ingTab, QString::fromLatin1("") );
	
	catTab = new QWidget( paramsTabWidget, "catTab" );
	catTabLayout = new QGridLayout( catTab, 1, 1, 11, 6, "catTabLayout"); 
	
	categoriesAnyEdit = new QLineEdit( catTab, "categoriesAnyEdit" );
	
	catTabLayout->addWidget( categoriesAnyEdit, 1, 1 );
	
	textLabel1_3_3 = new QLabel( catTab, "textLabel1_3_3" );
	
	catTabLayout->addWidget( textLabel1_3_3, 2, 0 );
	
	textLabel1_2_3 = new QLabel( catTab, "textLabel1_2_3" );
	
	catTabLayout->addWidget( textLabel1_2_3, 1, 0 );
	
	textLabel1_5 = new QLabel( catTab, "textLabel1_5" );
	
	catTabLayout->addWidget( textLabel1_5, 0, 0 );
	
	categoriesNotEdit = new QLineEdit( catTab, "categoriesNotEdit" );
	
	catTabLayout->addWidget( categoriesNotEdit, 2, 1 );
	
	categoriesAllEdit = new QLineEdit( catTab, "categoriesAllEdit" );
	
	catTabLayout->addWidget( categoriesAllEdit, 0, 1 );
	paramsTabWidget->insertTab( catTab, QString::fromLatin1("") );
	
	servPrepTab = new QWidget( paramsTabWidget, "servPrepTab" );
	servPrepTabLayout = new QHBoxLayout( servPrepTab, 11, 6, "servPrepTabLayout"); 
	
	servingsBox = new QGroupBox( servPrepTab, "servingsBox" );
	servingsBox->setAlignment( int( QGroupBox::AlignVCenter ) );
	servingsBox->setColumnLayout(0, Qt::Vertical );
	servingsBox->layout()->setSpacing( 6 );
	servingsBox->layout()->setMargin( 11 );
	servingsBoxLayout = new QVBoxLayout( servingsBox->layout() );
	servingsBoxLayout->setAlignment( Qt::AlignTop );
	
	enableServingsCheckBox = new QCheckBox( servingsBox, "enableServingsCheckBox" );
	servingsBoxLayout->addWidget( enableServingsCheckBox );
	
	servingsFrame = new QFrame( servingsBox, "servingsFrame" );
	servingsFrame->setEnabled( FALSE );
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
	servPrepTabLayout->addWidget( servingsBox );
	
	prepTimeBox = new QGroupBox( servPrepTab, "prepTimeBox" );
	prepTimeBox->setAlignment( int( QGroupBox::WordBreak | QGroupBox::AlignJustify | QGroupBox::AlignCenter | QGroupBox::AlignRight | QGroupBox::AlignLeft ) );
	prepTimeBox->setColumnLayout(0, Qt::Vertical );
	prepTimeBox->layout()->setSpacing( 6 );
	prepTimeBox->layout()->setMargin( 11 );
	prepTimeBoxLayout = new QVBoxLayout( prepTimeBox->layout() );
	prepTimeBoxLayout->setAlignment( Qt::AlignTop );
	
	enablePrepTimeCheckBox = new QCheckBox( prepTimeBox, "enablePrepTimeCheckBox" );
	prepTimeBoxLayout->addWidget( enablePrepTimeCheckBox );
	
	prepFrame = new QFrame( prepTimeBox, "prepFrame" );
	prepFrame->setEnabled( FALSE );
	prepFrame->setFrameShape( QFrame::NoFrame );
	prepFrame->setFrameShadow( QFrame::Plain );
	prepFrame->setLineWidth( 0 );
	prepFrameLayout = new QHBoxLayout( prepFrame, 0, 0, "prepFrameLayout"); 
	
	prepTimeComboBox = new QComboBox( FALSE, prepFrame, "prepTimeComboBox" );
	prepTimeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, prepTimeComboBox->sizePolicy().hasHeightForWidth() ) );
	prepFrameLayout->addWidget( prepTimeComboBox );
	
	prepTimeEdit = new QTimeEdit( prepFrame, "prepTimeEdit" );
	prepTimeEdit->setDisplay( int( QTimeEdit::Minutes | QTimeEdit::Hours ) );
	prepFrameLayout->addWidget( prepTimeEdit );
	prepTimeBoxLayout->addWidget( prepFrame );
	servPrepTabLayout->addWidget( prepTimeBox );
	paramsTabWidget->insertTab( servPrepTab, QString::fromLatin1("") );
	
	authorTab = new QWidget( paramsTabWidget, "authorTab" );
	authorTabLayout = new QGridLayout( authorTab, 1, 1, 11, 6, "authorTabLayout"); 
	
	authorsAllEdit = new QLineEdit( authorTab, "authorsAllEdit" );
	
	authorTabLayout->addWidget( authorsAllEdit, 0, 2 );
	
	textLabel1_6 = new QLabel( authorTab, "textLabel1_6" );
	
	authorTabLayout->addMultiCellWidget( textLabel1_6, 0, 0, 0, 1 );
	
	textLabel1_3_4 = new QLabel( authorTab, "textLabel1_3_4" );
	
	authorTabLayout->addMultiCellWidget( textLabel1_3_4, 2, 2, 0, 1 );
	
	authorsWithoutEdit = new QLineEdit( authorTab, "authorsWithoutEdit" );
	
	authorTabLayout->addWidget( authorsWithoutEdit, 2, 2 );
	
	textLabel1_2_4 = new QLabel( authorTab, "textLabel1_2_4" );
	
	authorTabLayout->addWidget( textLabel1_2_4, 1, 0 );
	
	authorsAnyEdit = new QLineEdit( authorTab, "authorsAnyEdit" );
	
	authorTabLayout->addMultiCellWidget( authorsAnyEdit, 1, 1, 1, 2 );
	paramsTabWidget->insertTab( authorTab, QString::fromLatin1("") );
	layout17->addWidget( paramsTabWidget );
	
	layout9 = new QVBoxLayout( 0, 0, 6, "layout9"); 
	
	findButton = new KPushButton( this, "findButton" );
	findButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, findButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( findButton );
	
	clearButton = new KPushButton( this, "clearButton" );
	clearButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, clearButton->sizePolicy().hasHeightForWidth() ) );
	layout9->addWidget( clearButton );
	spacer7 = new QSpacerItem( 21, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout9->addItem( spacer7 );
	layout17->addLayout( layout9 );
	AdvancedSearchDialogLayout->addLayout( layout17 );
	
	resultsListView = new KListView( this, "resultsListView" );
	resultsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 1, resultsListView->sizePolicy().hasHeightForWidth() ) );
	AdvancedSearchDialogLayout->addWidget( resultsListView );
	languageChange();
	clearWState( WState_Polished );
	///
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	///

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	resultsListView->addColumn( i18n( "Title" ) );
	resultsListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( resultsListView, database, RecipeActionsHandler::Open | RecipeActionsHandler::Edit | RecipeActionsHandler::Export );

	connect( findButton, SIGNAL( clicked() ), SLOT( search() ) );
	connect( clearButton, SIGNAL( clicked() ), SLOT( clear() ) );

	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsFrame, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepFrame, SLOT( setEnabled( bool ) ) );

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{}

void AdvancedSearchDialog::languageChange()
{
	textLabel1_3->setText( i18n( "Without the following:" ) );
	textLabel1_2->setText( i18n( "Uses any of the following:" ) );
	textLabel1->setText( i18n( "Uses all the following:" ) );
	paramsTabWidget->changeTab( ingTab, i18n( "Ingredients" ) );
	textLabel1_3_3->setText( i18n( "Not in the following:" ) );
	textLabel1_2_3->setText( i18n( "In any of the following:" ) );
	textLabel1_5->setText( i18n( "In all the following:" ) );
	paramsTabWidget->changeTab( catTab, i18n( "Categories" ) );
	servingsBox->setTitle( i18n( "Servings" ) );
	enableServingsCheckBox->setText( i18n( "Enabled" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Serves at least:" ) );
	servingsComboBox->insertItem( i18n( "Serves at most:" ) );
	servingsComboBox->insertItem( i18n( "Serves about:" ) );
	prepTimeBox->setTitle( i18n( "Preparation Time" ) );
	enablePrepTimeCheckBox->setText( i18n( "Enabled" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( i18n( "Ready in at least:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in more than:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in about:" ) );
	paramsTabWidget->changeTab( servPrepTab, i18n( "Servings/Preparation Time" ) );
	textLabel1_6->setText( i18n( "By all the following:" ) );
	textLabel1_3_4->setText( i18n( "Without the following:" ) );
	textLabel1_2_4->setText( i18n( "By any of the following:" ) );
	paramsTabWidget->changeTab( authorTab, i18n( "Authors" ) );
	findButton->setText( i18n( "Find" ) );
	clearButton->setText( i18n( "C&lear" ) );
	clearButton->setAccel( QKeySequence( i18n( "Alt+L" ) ) );
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
