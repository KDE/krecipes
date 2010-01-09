/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipefilter.h"

#include <kdebug.h>

#include "widgets/recipelistview.h"

RecipeFilter::RecipeFilter( K3ListView *klv ) : QObject( klv ),
		listview( klv ),
		currentCategory( 0 )
{}

void RecipeFilter::filter( const QString &s )
{
	//do this to only iterate over children of 'currentCategory'
	Q3ListViewItem * pEndItem = NULL;
	if ( currentCategory ) {
		Q3ListViewItem * pStartItem = currentCategory;
		do {
			if ( pStartItem->nextSibling() )
				pEndItem = pStartItem->nextSibling();
			else
				pStartItem = pStartItem->parent();
		}
		while ( pStartItem && !pEndItem );
	}

	//Re-show everything
	Q3ListViewItemIterator list_it;
	if ( currentCategory )
		list_it = Q3ListViewItemIterator( currentCategory );
	else
		list_it = Q3ListViewItemIterator( listview );
	while ( list_it.current() != pEndItem ) {
		list_it.current() ->setVisible( true );
		list_it++;
	}

	// Only filter if the filter text isn't empty
	if ( !s.isEmpty() ) {
		Q3ListViewItemIterator list_it( listview );
		while ( Q3ListViewItem * it = list_it.current() ) {
			if ( it->rtti() == 1000 )  // Its a recipe
			{
				RecipeListItem * recipe_it = ( RecipeListItem* ) it;

				if ( recipe_it->title().contains( s, Qt::CaseInsensitive ) )
				{
					if ( currentCategory ) {
						if ( isParentOf( currentCategory, recipe_it ) )
							recipe_it->setVisible( true );
						else
							recipe_it->setVisible( false );
					}
					else
						recipe_it->setVisible( true );
				}
				else
					recipe_it->setVisible( false );
			}

			++list_it;
		}
		hideIfEmpty();
	}
}

void RecipeFilter::filterCategory( int categoryID )
{
	kDebug() << "I got category :" << categoryID ;

	if ( categoryID == -1 )
		currentCategory = 0;
	else {
		Q3ListViewItemIterator list_it( listview );
		while ( Q3ListViewItem * it = list_it.current() ) {
			if ( it->rtti() == 1001 ) {
				CategoryListItem * cat_it = ( CategoryListItem* ) it;
				if ( cat_it->categoryId() == categoryID ) {
					currentCategory = cat_it;
					break;
				}
			}

			++list_it;
		}
	}

	Q3ListViewItemIterator list_it( listview );
	while ( Q3ListViewItem * it = list_it.current() ) {
		if ( categoryID == -1 )
			it->setVisible( true ); // We're not filtering categories
		else if ( it == currentCategory || isParentOf( it, currentCategory ) || isParentOf( currentCategory, it ) )
			it->setVisible( true );
		else
			it->setVisible( false );

		++list_it;
	}

	if ( currentCategory )
		currentCategory->setOpen( true );
}

bool RecipeFilter::hideIfEmpty( Q3ListViewItem *parent )
{
	Q3ListViewItem * it;
	if ( parent == 0 )
		it = listview->firstChild();
	else
		it = parent->firstChild();

	bool parent_should_show = false;
	for ( ; it; it = it->nextSibling() ) {
		if ( (it->rtti() == 1000 && it->isVisible()) || (it->rtti() == NEXTLISTITEM_RTTI || it->rtti() == PREVLISTITEM_RTTI) ) {
			parent_should_show = true;
		}
		else {
			bool result = hideIfEmpty( it );
			if ( parent_should_show == false )
				parent_should_show = result;
		}
	}

	if ( parent && parent->rtti() != 1000 ) {
		if ( parent_should_show )
			parent->setOpen( true );
		parent->setVisible( parent_should_show );
	}
	return parent_should_show;
}

bool RecipeFilter::isParentOf( Q3ListViewItem *parent, Q3ListViewItem *to_check )
{
	for ( Q3ListViewItem * it = to_check->parent(); it; it = it->parent() ) {
		if ( it == parent )
			return true;
	}

	return false;
}

#include "recipefilter.moc"

