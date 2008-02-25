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
#include "widgets/unitcombobox.h"
#include "widgets/fractioninput.h"
#include "widgets/amountunitinput.h"
#include "datablocks/mixednumber.h"
#include "recipeactionshandler.h"

#include <q3header.h>
#include <qpainter.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <q3groupbox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3ValueList>
#include <QLabel>
#include <Q3VBoxLayout>

#include <kapplication.h>
#include <kcursor.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kvbox.h>

#include "profiling.h"

IngredientMatcherDialog::IngredientMatcherDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent )
{
	// Initialize internal variables
	database = db;

	//Design the dialog

	Q3VBoxLayout *dialogLayout = new Q3VBoxLayout( this, 11, 6 );

	// Ingredient list
	Q3HBoxLayout *layout2 = new Q3HBoxLayout( 0, 0, 6, "layout2" );

	allIngListView = new KreListView( this, QString::null, true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView(allIngListView,database);
	list_view->setSelectionMode( Q3ListView::Multi );
 	allIngListView->setListView(list_view);
	layout2->addWidget( allIngListView );

	Q3VBoxLayout *layout1 = new Q3VBoxLayout( 0, 0, 6, "layout1" );

	//KIconLoader *il = KIconLoader::global();

	addButton = new QPushButton( this );
   addButton->setObjectName( "addButton" );
	//addButton->setIconSet( il->loadIconSet( "go-next", KIcon::Small ) );
	addButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( addButton );

	removeButton = new QPushButton( this );
   removeButton->setObjectName( "removeButton" );
	//removeButton->setIconSet( il->loadIconSet( "go-previous", KIcon::Small ) );
	removeButton->setFixedSize( QSize( 32, 32 ) );
	layout1->addWidget( removeButton );
	QSpacerItem *spacer1 = new QSpacerItem( 51, 191, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout1->addItem( spacer1 );
	layout2->addLayout( layout1 );

	ingListView = new KreListView( this, QString::null, true );
	ingListView->listView() ->addColumn( i18n( "Ingredient (required?)" ) );
	ingListView->listView() ->addColumn( i18n( "Amount Available" ) );
	layout2->addWidget( ingListView );
	dialogLayout->addLayout( layout2 );

	// Box to select allowed number of missing ingredients
	missingBox = new KHBox( this );
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

	KConfigGroup config( KGlobal::config(), "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	recipeListView->listView() ->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	recipeListView->listView() ->addColumn( i18n( "Missing Ingredients" ) );

	recipeListView->listView() ->setSorting( -1 );
	dialogLayout->addWidget(recipeListView);

	RecipeActionsHandler *actionHandler = new RecipeActionsHandler( recipeListView->listView(), database, RecipeActionsHandler::Open | RecipeActionsHandler::Edit | RecipeActionsHandler::Export );

	KHBox *buttonBox = new KHBox( this );

	okButton = new QPushButton( buttonBox );
	//okButton->setIconSet( il->loadIconSet( "dialog-ok", KIcon::Small ) );
	okButton->setText( i18n( "Find matching recipes" ) );

	//buttonBox->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

	clearButton = new QPushButton( buttonBox );
	//clearButton->setIconSet( il->loadIconSet( "edit-clear", KIcon::Small ) );
	clearButton->setText( i18n( "Clear" ) );
	dialogLayout->addWidget(buttonBox);

	// Connect signals & slots
	connect ( okButton, SIGNAL( clicked() ), this, SLOT( findRecipes() ) );
	connect ( clearButton, SIGNAL( clicked() ), recipeListView->listView(), SLOT( clear() ) );
	connect ( clearButton, SIGNAL( clicked() ), this, SLOT( unselectIngredients() ) );
	connect ( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( addButton, SIGNAL( clicked() ), this, SLOT( addIngredient() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeIngredient() ) );
	connect( ingListView->listView(), SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ), SLOT( itemRenamed( Q3ListViewItem*, const QPoint &, int ) ) );
}

IngredientMatcherDialog::~IngredientMatcherDialog()
{
}

void IngredientMatcherDialog::itemRenamed( Q3ListViewItem* item, const QPoint &, int col )
{
	if ( col == 1 ) {
		KDialog amountEditDialog(this);
                amountEditDialog.setCaption(i18n("Enter amount"));
                amountEditDialog.setButtons(KDialog::Cancel | KDialog::Ok);
                amountEditDialog.setDefaultButton(KDialog::Ok);
                amountEditDialog.setModal( false );
		Q3GroupBox *box = new Q3GroupBox( 1, Qt::Horizontal, i18n("Amount"), &amountEditDialog );
		AmountUnitInput *amountEdit = new AmountUnitInput( box, database );
		// Set the values from the item
		if ( !item->text(1).isEmpty() ) {
			amountEdit->setAmount( MixedNumber::fromString(item->text(2)) );
			Unit u;
			u.id = item->text(3).toInt();
			amountEdit->setUnit( u );
		} else {
			amountEdit->setAmount( -1 );
			Unit u;
			u.id = -1;
			amountEdit->setUnit( u );
		}

		amountEditDialog.setMainWidget(box);

		if ( amountEditDialog.exec() == QDialog::Accepted ) {
			MixedNumber amount = amountEdit->amount();
			Unit unit = amountEdit->unit();

			if ( amount.toDouble() <= 1e-5 ) {
				amount = -1;
				unit.id = -1;

				item->setText(1,QString::null);
			} else {
				item->setText(1,amount.toString()+" "+((amount.toDouble()>1)?unit.plural:unit.name));
			}

			item->setText(2,amount.toString());
			item->setText(3,QString::number(unit.id));

			IngredientList::iterator ing_it = m_item_ing_map[item];
			(*ing_it).amount = amount.toDouble();
			(*ing_it).units = unit;
		}
	}
}

void IngredientMatcherDialog::addIngredient()
{
    /*
	QList<Q3ListViewItem> items = allIngListView->listView()->selectedItems();
	if ( !items.isEmpty() ) {
                for (int i = 0; i < items.size(); ++i) {
			bool dup = false;
			for ( Q3ListViewItem *exists_it = ingListView->listView()->firstChild(); exists_it; exists_it = exists_it->nextSibling() ) {
				if ( exists_it->text( 0 ) == item->text( 0 ) ) {
					dup = true;
					break;
				}
			}

			if ( !dup ) {
				Q3ListViewItem * new_item = new Q3CheckListItem( ingListView->listView(), item->text( 0 ), Q3CheckListItem::CheckBox );

				ingListView->listView() ->setSelected( new_item, true );
				ingListView->listView() ->ensureItemVisible( new_item );
				allIngListView->listView() ->setSelected( item, false );

				m_item_ing_map.insert( new_item, m_ingredientList.append( Ingredient( item->text( 0 ), 0, Unit(), -1, item->text( 1 ).toInt() ) ) );
			}
			++it;
		}
	}
    */
}

void IngredientMatcherDialog::removeIngredient()
{
    /*
	Q3ListViewItem * item = ingListView->listView() ->selectedItem();
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
    */
}

void IngredientMatcherDialog::unselectIngredients()
{
	ingListView->listView()->clear();
	for ( Q3ListViewItem *it = allIngListView->listView()->firstChild(); it; it = it->nextSibling() )
		allIngListView->listView()->setSelected(it,false);
}

void IngredientMatcherDialog::findRecipes( void )
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	START_TIMER("Ingredient Matcher: loading database data");

	RecipeList rlist;
	database->loadRecipes( &rlist, RecipeDB::Title | RecipeDB::NamesOnly | RecipeDB::Ingredients | RecipeDB::IngredientAmounts );

	END_TIMER();
	START_TIMER("Ingredient Matcher: analyzing data for matching recipes");

	// Clear the list
	recipeListView->listView() ->clear();

	// Now show the recipes with ingredients that are contained in the previous set
	// of ingredients
	RecipeList incompleteRecipes;
	QList <int> missingNumbers;
	Q3ValueList <IngredientList> missingIngredients;

	RecipeList::Iterator it;
	for ( it = rlist.begin();it != rlist.end();++it ) {
		IngredientList il = ( *it ).ingList;
		if ( il.isEmpty() )
			continue;

		IngredientList missing;
		if ( m_ingredientList.containsSubSet( il, missing, true, database ) ) {
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

	IngredientList requiredIngredients;
	for ( Q3ListViewItem *it = ingListView->listView()->firstChild(); it; it = it->nextSibling() ) {
		if ( ((Q3CheckListItem*)it)->isOn() )
			requiredIngredients << *m_item_ing_map[it];
	}

	// Classify recipes with missing ingredients in different lists by ammount
	QList<int>::Iterator nit;
	Q3ValueList<IngredientList>::Iterator ilit;
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

			if ( !( *it ).ingList.containsSubSet( requiredIngredients ) )
				continue;

			if ( ( *nit ) == missingNo ) {
				if ( !titleShownYet ) {
					new SectionItem( recipeListView->listView(), i18np( "You are missing 1 ingredient for:", "You are missing %n ingredients for:", missingNo ) );
					titleShownYet = true;
				}
				new CustomRecipeListItem( recipeListView->listView(), *it, *ilit );
			}
		}
	}
	END_TIMER();

	KApplication::restoreOverrideCursor();
}

void IngredientMatcherDialog::reload( ReloadFlags flag )
{
	( ( StdIngredientListView* ) allIngListView->listView() ) ->reload(flag);
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
