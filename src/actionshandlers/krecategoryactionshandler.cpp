/*****************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                    *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                           *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "krecategoryactionshandler.h"

#include "datablocks/elementlist.h"
#include "backends/recipedb.h"
#include "dialogs/createcategorydialog.h"
#include "dialogs/dependanciesdialog.h"
#include "widgets/krecategorieslistwidget.h"

#include <KLocale>
#include <KMenu>
#include <KAction>
#include <KMessageBox>

#include <QPointer>
#include <QModelIndex>
#include <QStandardItemModel>


KreCategoryActionsHandler::KreCategoryActionsHandler( KreCategoriesListWidget * listWidget, RecipeDB * db ):
	KreGenericActionsHandler( listWidget, db ),
	m_clipboard_id( RecipeDB::InvalidId ),
	m_pasteAction( 0 ),
	m_pasteAsSubAction( 0 )
{
	connect( m_contextMenu, SIGNAL( aboutToShow() ), SLOT( preparePopup() ) );
}

void KreCategoryActionsHandler::setCategoryPasteAction( KAction * action )
{
	m_contextMenu->addAction( action );
	m_pasteAction = action;
}

void KreCategoryActionsHandler::setPasteAsSubcategoryAction( KAction * action )
{
	m_contextMenu->addAction( action );
	m_pasteAsSubAction = action;
}

void KreCategoryActionsHandler::createNew()
{
	ElementList categories;
	m_database->loadCategories( &categories );
	QPointer<CreateCategoryDialog> categoryDialog = new CreateCategoryDialog( m_listWidget, categories );

	if ( categoryDialog->exec() == QDialog::Accepted ) {
		QString result = categoryDialog->newCategoryName();
		int subcategory = categoryDialog->subcategory();

		//check bounds first
		if ( checkBounds( result ) ) {
			// Create the new category in the database
			m_database->createNewCategory( result, subcategory );
		}
	}
	delete categoryDialog;
}

void KreCategoryActionsHandler::cut()
{
	m_clipboard_id = m_listWidget->selectedRowId();
	m_listWidget->model()->removeRow( m_listWidget->currentRow(), m_listWidget->currentParent() );
}

void KreCategoryActionsHandler::paste()
{
	m_database->modCategory( m_clipboard_id, RecipeDB::InvalidId );
	m_clipboard_id = RecipeDB::InvalidId;
}

void KreCategoryActionsHandler::pasteAsSub()
{
	RecipeDB::IdType parent_id = m_listWidget->selectedRowId();
	m_database->modCategory( m_clipboard_id, parent_id );
	m_clipboard_id = RecipeDB::InvalidId;
}

void KreCategoryActionsHandler::remove()
{
	RecipeDB::IdType id = m_listWidget->selectedRowId();
	if ( id == RecipeDB::InvalidId )
		return;

	ElementList recipeDependancies;
	m_database->findUseOfCategoryInRecipes( &recipeDependancies, id );
 
	if ( recipeDependancies.isEmpty() ) {
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "Are you sure you want to delete this category and all its subcategories?" ) ) ) {
			case KMessageBox::Continue:
				m_database->removeCategory( id );
				break;
		}
		return;
	} else { // need warning!
		ListInfo info;
		info.list = recipeDependancies;
		info.name = i18n("Recipes");
		QPointer<DependanciesDialog> warnDialog = new DependanciesDialog( m_listWidget, info, false );

		if ( warnDialog->exec() == QDialog::Accepted )
			m_database->removeCategory( id );

		delete warnDialog;
	}
}

bool KreCategoryActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > m_database->maxCategoryNameLength() ) {
		KMessageBox::error( m_listWidget,
		i18np( "Category name cannot be longer than 1 character.",
		"Category name cannot be longer than %1 characters." ,
		m_database->maxCategoryNameLength() ));
		return false;
	}

	return true;
}

void KreCategoryActionsHandler::preparePopup()
{
	//only enable the paste actions if we have something to paste.
	m_pasteAction->setEnabled( m_clipboard_id != RecipeDB::InvalidId );
	m_pasteAsSubAction->setEnabled( m_clipboard_id != RecipeDB::InvalidId );
}

void KreCategoryActionsHandler::saveElement( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
	//Unused parameter.
	Q_UNUSED( bottomRight )

	QString newCategoryName = topLeft.data().toString();
	if ( !checkBounds( newCategoryName ) ) {
		m_listWidget->reload(ForceReload); //reset the changed text
		return;
	}

	RecipeDB::IdType existing_id = m_database->findExistingCategoryByName( newCategoryName );
	RecipeDB::IdType cat_id = m_listWidget->selectedRowId();

	//If the row being modified is not the selected one, abort.
	if ( cat_id != topLeft.data( KreCategoriesListWidget::IdRole ).toInt() )
	{
		return;
	}

	if ( existing_id != RecipeDB::InvalidId && cat_id != RecipeDB::InvalidId
	&& existing_id != cat_id ) {
		//category already exists with this label... merge the two
		switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "This category already exists. Continuing will merge these two"
		" categories into one. Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
			m_database->mergeCategories( existing_id, cat_id );
			break;
		}
		default:
			m_listWidget->reload(ForceReload);
			break;
		}
	} else {
		m_database->modCategory( cat_id, newCategoryName );
	}
}

KreCategoryActionsHandler::~KreCategoryActionsHandler()
{
	//delete clipboard_item;
}
