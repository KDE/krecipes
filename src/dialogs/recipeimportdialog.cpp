/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeimportdialog.h"

#include <klocale.h>
#include <kpushbutton.h>
#include <k3listview.h>
#include <kdebug.h>
#include <kvbox.h>


#include <q3header.h>
#include <q3dict.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "datablocks/recipe.h"

RecipeImportDialog::RecipeImportDialog( const RecipeList &list, QWidget *parent )
		: KDialog( parent ),
		list_copy( list )
{
	this->setObjectName( "RecipeImportDialog" );
	this->setModal( true );
	this->setCaption( i18nc( "@title:window", "Import Recipes" ) );
	this->setButtons( KDialog::Ok | KDialog::Cancel );
	this->setDefaultButton( KDialog::Ok );
	setButtonsOrientation( Qt::Vertical );

	KVBox *page = new KVBox( this );
	setMainWidget( page );


	kListView = new K3ListView( page );
	kListView->addColumn( i18nc( "@title:column", "Recipes" ) );
	kListView->setProperty( "selectionMode", "NoSelection" );
	kListView->setRootIsDecorated( true );
	kListView->setAllColumnsShowFocus( true );

	languageChange();

	setInitialSize( QSize( 600, 480 ).expandedTo( minimumSizeHint() ) );

	loadListView();
}

RecipeImportDialog::~RecipeImportDialog()
{
	delete recipe_items;
}

void RecipeImportDialog::languageChange()
{
}

void RecipeImportDialog::loadListView()
{
	CustomCheckListItem * head_item = new CustomCheckListItem( kListView, i18nc( "@item:inlistbox All items", "All (%1)", list_copy.count() ), Q3CheckListItem::CheckBox );
	head_item->setOpen( true );

	//get all categories
	QStringList categoryList;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = list_copy.begin(); recipe_it != list_copy.end(); ++recipe_it ) {
		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
			if ( categoryList.contains( ( *cat_it ).name ) == false )
				categoryList << ( *cat_it ).name;
		}
	}

	//create all category check list items
	Q3Dict<CustomCheckListItem> all_categories;

	QStringList::iterator it;
	for ( it = categoryList.begin(); it != categoryList.end(); ++it ) {
		CustomCheckListItem *category_item = new CustomCheckListItem( head_item, *it, Q3CheckListItem::CheckBox );
		//category_item->setOpen(true);

		all_categories.insert( *it, category_item );
	}

	//add recipes to category check list items
	recipe_items = new QMap<CustomCheckListItem*, RecipeList::const_iterator>; //we won't be able to identify a recipe later if we  just put a value in here. The iterator will be unique so we'll use it.  This is safe since the list is constant (iterators won't become invlalid).

	CustomCheckListItem *item = 0;
	CustomCheckListItem *category_item = 0;

	for ( recipe_it = list_copy.begin(); recipe_it != list_copy.end(); ++recipe_it ) {
		if ( ( *recipe_it ).categoryList.count() == 0 ) {
			if ( !category_item )  //don't create this until there are recipes to put in it
			{
				category_item = new CustomCheckListItem( head_item, i18nc( "@item", "Uncategorized" ), Q3CheckListItem::CheckBox );
				all_categories.insert( i18nc( "@item", "Uncategorized" ), category_item );
			}
			CustomCheckListItem *item = new CustomCheckListItem( category_item, ( *recipe_it ).title, Q3CheckListItem::CheckBox );
			recipe_items->insert( item, recipe_it );
		}
		else {
			for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {

				CustomCheckListItem *category_item = all_categories[ ( *cat_it ).name ];

				item = new CustomCheckListItem( category_item, item, ( *recipe_it ).title, Q3CheckListItem::CheckBox );
				recipe_items->insert( item, recipe_it );
			}
		}
	}

	//append the number of recipes in each category to the check list item text
	Q3DictIterator<CustomCheckListItem> categories_it( all_categories );
	for ( ; categories_it.current(); ++categories_it ) {
		int count = 0;
		for ( Q3CheckListItem * it = static_cast<Q3CheckListItem*>( categories_it.current() ->firstChild() ); it; it = static_cast<Q3CheckListItem*>( it->nextSibling() ) ) {
			count++;
		}
		categories_it.current() ->setText( 0, categories_it.current() ->text( 0 ) + QString( " (%1)" ).arg( count ) );
	}

	head_item->setOn( true ); //this will check all recipes
}

RecipeList RecipeImportDialog::getSelectedRecipes()
{
	RecipeList selected_recipes;

	Q3ValueList<RecipeList::const_iterator> already_included_recipes;

	QMap<CustomCheckListItem*, RecipeList::const_iterator>::const_iterator it;
	for ( it = recipe_items->constBegin(); it != recipe_items->constEnd(); ++it ) {
		if ( static_cast<CustomCheckListItem*>( it.key() ) ->isOn() &&
		        ( already_included_recipes.contains( it.value() ) == 0 ) )  //make sure it isn't already in the list
		{
			already_included_recipes.prepend( it.value() );
			selected_recipes.prepend( *it.value() );
		}
	}

	return selected_recipes;
}

CustomCheckListItem::CustomCheckListItem( Q3ListView *parent, const QString & s, Type t )
		: Q3CheckListItem( parent, s, t ), m_locked( false )
{}

CustomCheckListItem::CustomCheckListItem( CustomCheckListItem *parent, const QString & s, Type t )
		: Q3CheckListItem( parent, s, t ), m_locked( false )
{}

CustomCheckListItem::CustomCheckListItem( Q3CheckListItem *parent, Q3CheckListItem *after, const QString & s, Type t )
		: Q3CheckListItem( parent, after, s, t ), m_locked( false )
{}

void CustomCheckListItem::stateChange( bool on )
{
	if ( !m_locked ) {
		for ( Q3CheckListItem * it = static_cast<Q3CheckListItem*>( firstChild() ); it; it = static_cast<Q3CheckListItem*>( it->nextSibling() ) ) {
			it->setOn( on );
		}
	}

	if ( !on ) {
		Q3ListViewItem * parent = this->parent();
		if ( parent && ( parent->rtti() == 1 ) ) {
			CustomCheckListItem * item = static_cast<CustomCheckListItem*>( parent );
			item->setLocked( true );
			item->setOn( on );
			item->setLocked( false );
		}
	}

	QString thisText = text(0);
	Q3ListViewItemIterator it( listView() );
	while ( it.current() ) {
		if ( it.current()->rtti() == 1 && it.current()->text(0) == thisText ) {
			CustomCheckListItem * item = static_cast<CustomCheckListItem*>( it.current() );
			item->setOn( on );
		}
		++it;
	}
}

