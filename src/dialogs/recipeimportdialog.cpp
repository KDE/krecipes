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

#include <qlayout.h>
#include <qheader.h>
#include <qvariant.h>

#include "recipe.h"

RecipeImportDialog::RecipeImportDialog( QPtrList<Recipe> *list, QWidget *parent ) : QDialog(parent,0,true),
  m_recipe_list(list)
{
	Form1Layout = new QHBoxLayout( this, 11, 6 );

	kListView = new KListView( this );
	kListView->addColumn( i18n( "Recipes" ) );
	kListView->setProperty( "selectionMode", "NoSelection" );
	kListView->setRootIsDecorated( true );

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

	loadListView(list);
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

void RecipeImportDialog::loadListView( QPtrList<Recipe> *list )
{
	head_item = new CustomCheckListItem( kListView, i18n("All"), QCheckListItem::CheckBox );
	head_item->setOpen(true);

	recipe_items = new QPtrVector<CustomCheckListItem>(list->count());

	CustomCheckListItem *item = 0;

	int i = 0;
	for ( Recipe *recipe = list->first(); recipe; recipe = list->next() )
	{
		item = new CustomCheckListItem( head_item, item, recipe->title, QCheckListItem::CheckBox );
		recipe_items->insert(i,item);
		i++;
	}
}

QPtrList<Recipe> * RecipeImportDialog::getSelectedRecipes()
{
	QPtrList<Recipe> *selected_recipes = new QPtrList<Recipe>;

	for ( int i = 0; i < m_recipe_list->count(); i++ )
	{
		if ( (*recipe_items)[i]->isOn() )
			selected_recipes->prepend(m_recipe_list->at(i));
	}

	return selected_recipes;
}

CustomCheckListItem::CustomCheckListItem(QListView *parent, const QString & s, Type t)
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

