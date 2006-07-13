/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientmatcherdialog.h"

#include "datablocks/recipelist.h"
#include "widgets/ingredientlistview.h"
#include "datablocks/elementlist.h"
#include "backends/recipedb.h"
#include "widgets/krelistview.h"
#include "datablocks/mixednumber.h"
#include "recipeactionshandler.h"

#include <qheader.h>
#include <qpainter.h>
#include <qstringlist.h>
#include <qlayout.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>

#include "profiling.h"

IngredientMatcherDialog::IngredientMatcherDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{
	// Initialize internal variables
	database = db;

	//Design the dialog
	//setSpacing( 10 );

	QVBoxLayout *dialogLayout = new QVBoxLayout( this, 11, 6 );

	// Ingredient list
	QHBoxLayout *layout2 = new QHBoxLayout( 0, 0, 6, "layout2" );

	allIngListView = new KreListView( this, QString::null, true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView(allIngListView,database);
 	allIngListView->setListView(list_view);
	layout2->addWidget( allIngListView );

	QVBoxLayout *layout1 = new QVBoxLayout( 0, 0, 6, "layout1" );

	KIconLoader il;

	addButton = new QPushButton( this, "addButton" );
	addButton->setIconSet( il.loadIconSet( "forward", KIcon::Small ) );
	addButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( addButton );

	removeButton = new QPushButton( this, "removeButton" );
	removeButton->setIconSet( il.loadIconSet( "back", KIcon::Small ) );
	removeButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( removeButton );
	QSpacerItem *spacer1 = new QSpacerItem( 51, 191, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout1->addItem( spacer1 );
	layout2->addLayout( layout1 );

	ingListView = new KreListView( this, QString::null, true );
	ingListView->listView() ->addColumn( i18n( "Ingredients" ) );
	ingListView->listView() ->addColumn( i18n( "Amount Available" ) );
	ingListView->listView() ->setItemsRenameable( true );
	ingListView->listView() ->setRenameable( 0, false );
	ingListView->listView() ->setRenameable( 1, true );
	layout2->addWidget( ingListView );
	dialogLayout->addLayout( layout2 );

	// Box to select allowed number of missing ingredients
	missingBox = new QHBox( this );
	missingNumberLabel = new QLabel( missingBox );
	missingNumberLabel->setText( i18n( "Missing ingredients allowed:" ) );
	missingNumberSpinBox = new KIntSpinBox( missingBox );
	missingNumberSpinBox->setMinValue( -1 );
	missingNumberSpinBox->setSpecialValueText( i18n( "Any" ) );
	dialogLayout->addWidget(missingBox);

	// Found recipe list
	recipeListView = new KreListView( this, i18n( "Matching Recipes" ), false, 1, missingBox );
	recipeListView->listView() ->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	recipeListView->listView() ->setAllColumnsShowFocus( true );
	recipeListView->listView() ->addColumn( i18n( "Title" ) );
	dialogLayout->addWidget(recipeListView);

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	recipeListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	recipeListView->listView() ->addColumn( i18n( "Missing Ingredients" ) );
	recipeListView->listView() ->setSorting( -1 );

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( recipeListView->listView(), database, RecipeActionsHandler::Open | RecipeActionsHandler::Edit | RecipeActionsHandler::Export );

	QHBox *buttonBox = new QHBox( this );

	okButton = new QPushButton( buttonBox );
	okButton->setIconSet( il.loadIconSet( "button_ok", KIcon::Small ) );
	okButton->setText( i18n( "Find matching recipes" ) );

	//buttonBox->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton = new QPushButton( buttonBox );
	clearButton->setIconSet( il.loadIconSet( "editclear", KIcon::Small ) );
	clearButton->setText( i18n( "Clear" ) );
	dialogLayout->addWidget(buttonBox);

	// Connect signals & slots
	connect ( okButton, SIGNAL( clicked() ), this, SLOT( findRecipes() ) );
	connect ( clearButton, SIGNAL( clicked() ), recipeListView->listView(), SLOT( clear() ) );
	connect ( clearButton, SIGNAL( clicked() ), this, SLOT( unselectIngredients() ) );
	connect ( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( addButton, SIGNAL( clicked() ), this, SLOT( addIngredient() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeIngredient() ) );
	connect( ingListView->listView(), SIGNAL( itemRenamed( QListViewItem*, const QString &, int ) ), SLOT( itemRenamed( QListViewItem*, const QString &, int ) ) );

}

IngredientMatcherDialog::~IngredientMatcherDialog()
{
}

void IngredientMatcherDialog::addIngredient()
{
	QListViewItem * item = allIngListView->listView() ->selectedItem();
	if ( item ) {
		QListViewItem * new_item = new QListViewItem( ingListView->listView(), item->text( 0 ) );
		ingListView->listView() ->setSelected( new_item, true );
		ingListView->listView() ->ensureItemVisible( new_item );
		allIngListView->listView() ->setSelected( item, false );

		m_item_ing_map.insert( new_item, m_ingredientList.append( Ingredient( item->text( 0 ), 0, Unit(), -1, item->text( 1 ).toInt() ) ) );
	}
}

void IngredientMatcherDialog::removeIngredient()
{
	QListViewItem * item = ingListView->listView() ->selectedItem();
	if ( item ) {
		for ( IngredientList::iterator it = m_ingredientList.begin(); it != m_ingredientList.end(); ++it ) {
			if ( *m_item_ing_map.find( item ) == it ) {
				m_ingredientList.remove( it );
				m_item_ing_map.remove( item );
				break;
			}
		}
		delete item;
	}
}

void IngredientMatcherDialog::itemRenamed( QListViewItem* item, const QString &new_text, int col )
{
	if ( col == 1 ) {
		IngredientList::iterator found_it = *m_item_ing_map.find( item );

		bool ok;
		MixedNumber amount = MixedNumber::fromString( new_text, &ok );
		if ( ok ) {
			( *found_it ).amount = amount.toDouble();
		}
		else { //revert back to the valid amount string
			QString amount_str;
			if ( ( *found_it ).amount > 0 ) {
				KConfig * config = kapp->config();
				config->setGroup( "Formatting" );
		
				if ( config->readBoolEntry( "Fraction" ) )
					amount_str = MixedNumber( ( *found_it ).amount ).toString();
				else
					amount_str = beautify( KGlobal::locale() ->formatNumber( ( *found_it ).amount, 5 ) );
			}

			item->setText( 1, amount_str );
		}
	}
	else if ( col == 2 ) {
		IngredientList::iterator found_it = *m_item_ing_map.find( item );

		if ( ( *found_it ).amount > 1 )
			( *found_it ).units.plural = new_text;
		else
			( *found_it ).units.name = new_text;
	}
}

void IngredientMatcherDialog::unselectIngredients()
{
	ingListView->listView()->clear();
}

void IngredientMatcherDialog::findRecipes( void )
{
	KApplication::setOverrideCursor( KCursor::waitCursor() );

	START_TIMER("Ingredient Matcher: loading database data");

	RecipeList rlist;
	database->loadRecipes( &rlist, RecipeDB::Ingredients | RecipeDB::NamesOnly | RecipeDB::Title );

	END_TIMER();
	START_TIMER("Ingredient Matcher: analyzing data for matching recipes");

	// Clear the list
	recipeListView->listView() ->clear();
	// Add the section header
	new SectionItem( recipeListView->listView(), i18n( "Possible recipes with the specified ingredients" ) );

	// Now show the recipes with ingredients that are contained in the previous set
	// of ingredients
	RecipeList incompleteRecipes;
	QValueList <int> missingNumbers;
	QValueList <IngredientList> missingIngredients;

	RecipeList::Iterator it;
	for ( it = rlist.begin();it != rlist.end();++it ) {
		IngredientList il = ( *it ).ingList;
		if ( il.isEmpty() )
			continue;

		IngredientList missing;
		if ( m_ingredientList.containsSubSet( il, missing ) ) {
			new CustomRecipeListItem( recipeListView->listView(), *it );
		}
		else {
			incompleteRecipes.append( *it );
			missingIngredients.append( missing );
			missingNumbers.append( missing.count() );
		}
	}
	END_TIMER();

	//Check if the user wants to show missing ingredients

	if ( missingNumberSpinBox->value() == 0 ) {
		KApplication::restoreOverrideCursor();
		return ;
	} //"None"



	START_TIMER("Ingredient Matcher: searching for and displaying partial matches");
	// Classify recipes with missing ingredients in different lists by ammount
	QValueList<int>::Iterator nit;
	QValueList<IngredientList>::Iterator ilit;
	int missingNoAllowed = missingNumberSpinBox->value();

	if ( missingNoAllowed == -1 )  // "Any"
	{
		for ( nit = missingNumbers.begin();nit != missingNumbers.end();++nit )
			if ( ( *nit ) > missingNoAllowed )
				missingNoAllowed = ( *nit );
	}


	for ( int missingNo = 1; missingNo <= missingNoAllowed; missingNo++ ) {
		nit = missingNumbers.begin();
		ilit = missingIngredients.begin();

		bool titleShownYet = false;

		for ( it = incompleteRecipes.begin();it != incompleteRecipes.end();++it, ++nit, ++ilit ) {
			if ( !( *it ).ingList.containsAny( m_ingredientList ) )
				continue;

			if ( ( *nit ) == missingNo ) {
				if ( !titleShownYet ) {
					new SectionItem( recipeListView->listView(), i18n( "You are missing 1 ingredient for:", "You are missing %n ingredients for:", missingNo ) );
					titleShownYet = true;
				}
				new CustomRecipeListItem( recipeListView->listView(), *it, *ilit );
			}
		}
	}
	END_TIMER();

	KApplication::restoreOverrideCursor();
}

void IngredientMatcherDialog::reload( void )
{
	( ( StdIngredientListView* ) allIngListView->listView() ) ->reload();
}

void SectionItem::paintCell ( QPainter * p, const QColorGroup & /*cg*/, int column, int width, int /*align*/ )
{
	// Draw the section's deco
	p->setPen( KGlobalSettings::activeTitleColor() );
	p->setBrush( KGlobalSettings::activeTitleColor() );
	p->drawRect( 0, 0, width, height() );

	// Draw the section's text
	if ( column == 0 ) {
		QFont titleFont = KGlobalSettings::windowTitleFont ();
		p->setFont( titleFont );

		p->setPen( KGlobalSettings::activeTextColor() );
		p->drawText( 0, 0, width, height(), Qt::AlignLeft | Qt::AlignVCenter, mText );
	}
}

#include "ingredientmatcherdialog.moc"
