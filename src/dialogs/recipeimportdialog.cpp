/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recipeimportdialog.h"

#include <klocale.h>
#include <kpushbutton.h>
#include <klistview.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qheader.h>
#include <qvariant.h>
#include <qdict.h>

#include "recipe.h"

RecipeImportDialog::RecipeImportDialog( const RecipeList &list, QWidget *parent ) : QDialog(parent,0,true), list_copy(list)
{
	Form1Layout = new QHBoxLayout( this, 11, 6 );

	kListView = new KListView( this );
	kListView->addColumn( i18n( "Recipes" ) );
	kListView->setProperty( "selectionMode", "NoSelection" );
	kListView->setRootIsDecorated( true );
	kListView->setAllColumnsShowFocus(true);

	Form1Layout->addWidget( kListView );

	layout2 = new QVBoxLayout( 0, 0, 6 );

	okButton = new KPushButton( this );
	okButton->setDefault( TRUE );
	layout2->addWidget( okButton );

	cancelButton = new KPushButton( this );
	layout2->addWidget( cancelButton );
	QSpacerItem* spacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
	layout2->addItem( spacer );
	Form1Layout->addLayout( layout2 );

	languageChange();

	resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );

	connect( okButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );

	loadListView();
}

RecipeImportDialog::~RecipeImportDialog()
{
	delete recipe_items;
}

void RecipeImportDialog::languageChange()
{
	setCaption( i18n( "Import Recipes" ) );

	okButton->setText( i18n( "&OK" ) );
	cancelButton->setText( i18n( "&Cancel" ) );
}

void RecipeImportDialog::loadListView()
{
	CustomCheckListItem *head_item = new CustomCheckListItem( kListView, QString(i18n("All (%1)")).arg(list_copy.count()), QCheckListItem::CheckBox );
	head_item->setOpen(true);

	//get all categories
	QStringList categoryList;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = list_copy.begin(); recipe_it != list_copy.end(); ++recipe_it )
	{
		QPtrListIterator<Element> cat_it( (*recipe_it).categoryList );
		Element *cat;
		while ( (cat = cat_it.current()) != 0 )
		{
			++cat_it;
			if ( categoryList.contains( cat->name ) < 1 )
				categoryList << cat->name;
		}
	}

	//create all category check list items
	QDict<CustomCheckListItem> all_categories;

	QStringList::iterator it;
	for ( it = categoryList.begin(); it != categoryList.end(); ++it )
	{
		CustomCheckListItem *category_item = new CustomCheckListItem( head_item, *it, QCheckListItem::CheckBox );
		//category_item->setOpen(true);

		all_categories.insert( *it, category_item );
	}

	//add recipes to category check list items
	recipe_items = new QMap<CustomCheckListItem*,RecipeList::const_iterator>; //we won't be able to identify a recipe later if we  just put a value in here. The iterator will be unique so we'll use it.  This is safe since the list is constant (iterators won't become invlalid).

	CustomCheckListItem *item = 0;
	CustomCheckListItem *category_item = 0;

	for ( recipe_it = list_copy.begin(); recipe_it != list_copy.end(); ++recipe_it )
	{
		if ( (*recipe_it).categoryList.count() == 0 )
		{
			if ( !category_item ) //don't create this until there are recipes to put in it
			{
				category_item = new CustomCheckListItem( head_item, i18n("Uncategorized"), QCheckListItem::CheckBox );
				all_categories.insert( i18n("Uncategorized"), category_item );
			}
			CustomCheckListItem *item = new CustomCheckListItem(category_item,(*recipe_it).title, QCheckListItem::CheckBox);
			recipe_items->insert(item,recipe_it);
		}
		else
		{
			QPtrListIterator<Element> cat_it( (*recipe_it).categoryList );
			Element *cat;
			while ( (cat = cat_it.current()) != 0 )
			{
				++cat_it;

				CustomCheckListItem *category_item = all_categories[cat->name];

				item = new CustomCheckListItem( category_item, item, (*recipe_it).title, QCheckListItem::CheckBox );
				recipe_items->insert(item,recipe_it);
			}
		}
	}

	//append the number of recipes in each category to the check list item text
	QDictIterator<CustomCheckListItem> categories_it( all_categories );
	for( ; categories_it.current(); ++categories_it )
	{
		int count = 0;
		for (QCheckListItem *it=static_cast<QCheckListItem*>(categories_it.current()->firstChild()); it; it = static_cast<QCheckListItem*>(it->nextSibling()))
		{
			count++;
		}
		categories_it.current()->setText( 0, categories_it.current()->text(0) + QString(" (%1)").arg(count) );
	}
}

RecipeList RecipeImportDialog::getSelectedRecipes()
{
	RecipeList selected_recipes;

	QValueList<RecipeList::const_iterator> already_included_recipes;

	QMap<CustomCheckListItem*,RecipeList::const_iterator>::const_iterator it;
	for ( it = recipe_items->begin(); it != recipe_items->end(); ++it )
	{
		if ( static_cast<CustomCheckListItem*>(it.key())->isOn() &&
		     (already_included_recipes.contains( it.data() ) == 0) ) //make sure it isn't already in the list
		{
			already_included_recipes.prepend( it.data() );
			selected_recipes.prepend( *it.data() );
		}
	}

	return selected_recipes;
}

CustomCheckListItem::CustomCheckListItem(QListView *parent, const QString & s, Type t)
 : QCheckListItem(parent,s,t), m_locked(false)
{}

CustomCheckListItem::CustomCheckListItem(CustomCheckListItem *parent, const QString & s, Type t)
 : QCheckListItem(parent,s,t), m_locked(false)
{}

CustomCheckListItem::CustomCheckListItem(QCheckListItem *parent, QCheckListItem *after, const QString & s, Type t)
 : QCheckListItem(parent,after,s,t), m_locked(false)
{}

void CustomCheckListItem::stateChange( bool on )
{
	if ( !m_locked )
	{
		for (QCheckListItem *it=static_cast<QCheckListItem*>(this->firstChild()); it; it = static_cast<QCheckListItem*>(it->nextSibling()))
		{
			it->setOn(on);
		}
	}

	if ( !on )
	{
		QListViewItem *parent = this->parent();
		if ( parent && (parent->rtti() == 1) )
		{
			CustomCheckListItem *item = static_cast<CustomCheckListItem*>(parent);
			item->setLocked(true);
			item->setOn(on);
			item->setLocked(false);
		}
	}
}

