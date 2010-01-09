/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "categoryactionshandler.h"

#include <KLocale>
#include <K3ListView>
#include <KMenu>
#include <KAction>
#include <KIcon>
#include <QPointer>
#include <KMessageBox>
class Q3ListViewItem;

#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "widgets/categorylistview.h"

#include "dialogs/createcategorydialog.h"
#include "dialogs/dependanciesdialog.h"


CategoryActionsHandler::CategoryActionsHandler( StdCategoryListView *_parentListView, RecipeDB *db ):
	ActionsHandlerBase( _parentListView, db ),
	clipboard_item( 0 ),
	pasteAction( 0 ),
	pasteAsSubAction( 0 )
{
	connect( kpop, SIGNAL( aboutToShow() ), SLOT( preparePopup() ) );
	connect( parentListView,
		SIGNAL( moved( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) ),
		SLOT( changeCategoryParent( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) )
	);
}

void CategoryActionsHandler::setCategoryPasteAction( KAction * action )
{
	kpop->addAction( action );
	pasteAction = action;
}

void CategoryActionsHandler::setPasteAsSubcategoryAction( KAction * action )
{
	kpop->addAction( action );
	pasteAsSubAction = action;
}

void CategoryActionsHandler::createNew()
{
	ElementList categories;
	database->loadCategories( &categories );
	QPointer<CreateCategoryDialog> categoryDialog = new CreateCategoryDialog( parentListView, categories );

	if ( categoryDialog->exec() == QDialog::Accepted ) {
		QString result = categoryDialog->newCategoryName();
		int subcategory = categoryDialog->subcategory();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewCategory( result, subcategory ); // Create the new category in the database
	}
	delete categoryDialog;
}

void CategoryActionsHandler::cut()
{
	//restore a never used cut
	if ( clipboard_item ) {
		if ( clipboard_parent )
			clipboard_parent->insertItem( clipboard_item );
		else
			parentListView->insertItem( clipboard_item );
		clipboard_item = 0;
	}

	Q3ListViewItem *item = parentListView->currentItem();

	if ( item ) {
		clipboard_item = item;
		clipboard_parent = item->parent();

		if ( item->parent() )
			item->parent() ->takeItem( item );
		else
			parentListView->takeItem( item );
	}
}

void CategoryActionsHandler::paste()
{
	Q3ListViewItem * item = parentListView->currentItem();
	if ( item && clipboard_item ) {
		if ( item->parent() )
			item->parent() ->insertItem( clipboard_item );
		else
			parentListView->insertItem( clipboard_item );

		database->modCategory( clipboard_item->text( 1 ).toInt(), item->parent() ? item->parent() ->text( 1 ).toInt() : -1 );
		clipboard_item = 0;
	}
}

void CategoryActionsHandler::pasteAsSub()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item && clipboard_item ) {
		item->insertItem( clipboard_item );
		database->modCategory( clipboard_item->text( 1 ).toInt(), item->text( 1 ).toInt() );
		clipboard_item = 0;
	}
}

void CategoryActionsHandler::changeCategoryParent(Q3ListViewItem *item,
	Q3ListViewItem * /*afterFirst*/, Q3ListViewItem * /*afterNow*/ )
{
	int new_parent_id = -1;
	if ( Q3ListViewItem * parent = item->parent() )
		new_parent_id = parent->text( 1 ).toInt();

	int cat_id = item->text( 1 ).toInt();

	database->modCategory( cat_id, new_parent_id, false );
}

void CategoryActionsHandler::remove()
{
	Q3ListViewItem * item = parentListView->currentItem();

	if ( item ) {
		int id = item->text( 1 ).toInt();

		ElementList recipeDependancies;
		database->findUseOfCategoryInRecipes( &recipeDependancies, id );

		if ( recipeDependancies.isEmpty() ) {
			switch ( KMessageBox::warningContinueCancel( parentListView,
			i18n( "Are you sure you want to delete this category and all its subcategories?" ) ) ) {
				case KMessageBox::Continue:
					database->removeCategory( id );
					break;
			}
			return;
		}
		else { // need warning!
			ListInfo info;
			info.list = recipeDependancies;
			info.name = i18n("Recipes");
			QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( parentListView, info, false );

			if ( warnDialog->exec() == QDialog::Accepted )
				database->removeCategory( id );

			delete warnDialog;
		}
	}
}

bool CategoryActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxCategoryNameLength()) ) {
		KMessageBox::error( parentListView,
		i18np( "Category name cannot be longer than 1 character.",
		"Category name cannot be longer than %1 characters." ,
		database->maxCategoryNameLength() ));
		return false;
	}

	return true;
}

void CategoryActionsHandler::preparePopup()
{
	//only enable the paste actions if clipboard_item isn't null
	pasteAction->setEnabled( clipboard_item );
	pasteAsSubAction->setEnabled( clipboard_item );
}

void CategoryActionsHandler::saveElement( Q3ListViewItem* i )
{
	CategoryListItem * cat_it = ( CategoryListItem* ) i;

	if ( !checkBounds( cat_it->categoryName() ) ) {
		parentListView->reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingCategoryByName( cat_it->categoryName() );
	int cat_id = cat_it->categoryId();
	if ( existing_id != -1 && existing_id != cat_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( parentListView,
		i18n( "This category already exists. Continuing will merge these two categories into one. Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeCategories( existing_id, cat_id );
				break;
			}
		default:
			parentListView->reload(ForceReload);
			break;
		}
	}
	else
		database->modCategory( cat_id, cat_it->categoryName() );
}

CategoryActionsHandler::~CategoryActionsHandler()
{
	delete clipboard_item;
}
