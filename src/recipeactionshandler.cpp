/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeactionshandler.h"

#include <qwidget.h>
#include <qclipboard.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <kapplication.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <k3listview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kprogressdialog.h>

#include "dialogs/selectcategoriesdialog.h"

#include "exporters/cookmlexporter.h"
#include "exporters/xsltexporter.h"
#include "exporters/htmlexporter.h"
#include "exporters/htmlbookexporter.h"
#include "exporters/kreexporter.h"
#include "exporters/mmfexporter.h"
#include "exporters/recipemlexporter.h"
#include "exporters/plaintextexporter.h"
#include "exporters/rezkonvexporter.h"

#include "widgets/recipelistview.h"
#include "widgets/categorylistview.h"

#include "backends/recipedb.h"

RecipeActionsHandler::RecipeActionsHandler( K3ListView *_parentListView, RecipeDB *db, int actions ) : QObject( _parentListView ),
		parentListView( _parentListView ),
		database( db )
{
	KIconLoader *il = KIconLoader::global();

	kpop = new KMenu( parentListView );
	if ( actions & Open )
		kpop->insertItem( il->loadIcon( "ok", KIconLoader::NoGroup, 16 ), i18n( "&Open" ), this, SLOT( open() ), Qt::CTRL + Qt::Key_L );
	if ( actions & Edit )
		kpop->insertItem( il->loadIcon( "edit", KIconLoader::NoGroup, 16 ), i18n( "&Edit" ), this, SLOT( edit() ), Qt::CTRL + Qt::Key_E );
	if ( actions & Export )
		kpop->insertItem( il->loadIcon( "document-export", KIconLoader::NoGroup, 16 ), i18n( "E&xport" ), this, SLOT( recipeExport() ), 0 );
	if ( actions & RemoveFromCategory )
		remove_from_cat_item = kpop->insertItem( il->loadIcon( "edit-delete-shred", KIconLoader::NoGroup, 16 ), i18n( "&Remove From Category" ), this, SLOT( removeFromCategory() ), Qt::CTRL + Qt::Key_R );
	if ( actions & Remove )
		kpop->insertItem( il->loadIcon( "edit-delete-shred", KIconLoader::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
	if ( actions & AddToShoppingList )
		kpop->insertItem( il->loadIcon( "trolley", KIconLoader::NoGroup, 16 ), i18n( "&Add to Shopping List" ), this, SLOT( addToShoppingList() ), Qt::CTRL + Qt::Key_A );
	if ( actions & CopyToClipboard )
		kpop->insertItem( il->loadIcon( "edit-copy", KIconLoader::NoGroup, 16 ), i18n( "&Copy to Clipboard" ), this, SLOT( recipesToClipboard() ), Qt::CTRL + Qt::Key_C );

	if ( actions & Categorize )
		categorize_item = kpop->insertItem( il->loadIcon( "categories", KIconLoader::NoGroup, 16 ), i18n( "Ca&tegorize..." ), this, SLOT(categorize()), Qt::CTRL + Qt::Key_T );

	kpop->polish();

	catPop = new KMenu( parentListView );
	if ( actions & ExpandAll )
		catPop->insertItem( i18n( "&Expand All" ), this, SLOT( expandAll() ), Qt::CTRL + Qt::Key_Plus );
	if ( actions & CollapseAll )
		catPop->insertItem( i18n( "&Collapse All" ), this, SLOT( collapseAll() ), Qt::CTRL + Qt::Key_Minus );
	if ( actions & Export )
		catPop->insertItem( il->loadIcon( "document-export", KIconLoader::NoGroup, 16 ), i18n( "E&xport" ), this, SLOT( recipeExport() ), 0 );

	catPop->polish();

	connect( parentListView, SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) ) );
	connect( parentListView, SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ), SLOT( open() ) );
}

void RecipeActionsHandler::exec( ItemType type, const QPoint &p )
{
	if ( type == Recipe ) {
		if ( kpop->idAt( 0 ) != -1 )
			kpop->exec( p );
	}
	else if ( type == Category ) {
		if ( catPop->idAt( 0 ) != -1 )
			catPop->exec( p );
	}
}

void RecipeActionsHandler::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i ) { // Check if the QListViewItem actually exists
		if ( i->rtti() == 1000 ) {
			kpop->setItemVisible( categorize_item, i->parent() && i->parent()->rtti() == 1006 );
			kpop->setItemVisible( remove_from_cat_item, i->parent() && i->parent()->rtti() == 1001 );
			exec( Recipe, p );
		}
		else if ( i->rtti() == 1001 )  //is a category... don't pop-up for an empty category though
			exec( Category, p );
	}
}

Q3ValueList<int> RecipeActionsHandler::recipeIDs( const QList<Q3ListViewItem *> &items ) const
{
	Q3ValueList<int> ids;

	QListIterator<Q3ListViewItem *> it(items);
	const Q3ListViewItem *item;
	while ( it.hasNext() ) {
		item = it.next();
		if ( item->rtti() == 1000 ) { //RecipeListItem
			RecipeListItem * recipe_it = ( RecipeListItem* ) item;
			if ( ids.find( recipe_it->recipeID() ) == ids.end() )
				ids << recipe_it->recipeID();
		}
		else if ( item->rtti() == 1001 ) {
			CategoryListItem *cat_it = ( CategoryListItem* ) item;
			ElementList list;
			database->loadRecipeList( &list, cat_it->element().id, true );
	
			for ( ElementList::const_iterator cat_it = list.begin(); cat_it != list.end(); ++cat_it ) {
				if ( ids.find( (*cat_it).id ) == ids.end() )
					ids << (*cat_it).id;
			}
		}
	}

	return ids;
}

void RecipeActionsHandler::open()
{
	const QList<Q3ListViewItem*> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		Q3ValueList<int> ids = recipeIDs(items);
		if ( ids.count() == 1 )
			emit recipeSelected(ids.first(),0);
		else if ( ids.count() > 0 )
			emit recipesSelected(ids,0);
		#if 0
		else if ( it->rtti() == 1001 && it->firstChild() )  //CategoryListItem and not empty
		{
			Q3ValueList<int> ids;

			//do this to only iterate over children of 'it'
			Q3ListViewItem *pEndItem = NULL;
			Q3ListViewItem *pStartItem = it;
			do
			{
				if ( pStartItem->nextSibling() )
					pEndItem = pStartItem->nextSibling();
				else
					pStartItem = pStartItem->parent();
			}
			while ( pStartItem && !pEndItem );

			Q3ListViewItemIterator iterator( it );
			while ( iterator.current() != pEndItem )
			{
				if ( iterator.current() ->rtti() == 1000 ) {
					RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
					if ( ids.find( recipe_it->recipeID() ) == ids.end() ) {
						ids.append( recipe_it->recipeID() );
					}
				}
				++iterator;
			}
			emit recipesSelected( ids, 0 );
		}
		#endif
	}
}

void RecipeActionsHandler::categorize()
{
	QList<Q3ListViewItem*> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		ElementList categoryList;
		SelectCategoriesDialog *editCategoriesDialog = new SelectCategoriesDialog( parentListView, categoryList, database );
	
		if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
			editCategoriesDialog->getSelectedCategories( &categoryList ); // get the category list chosen
			
			QListIterator<Q3ListViewItem *> it(items);
			Q3ListViewItem *item;
			while ( it.hasNext() ) {
				item = it.next();
				if ( item->parent() != 0 ) {
					RecipeListItem * recipe_it = ( RecipeListItem* ) item;
					int recipe_id = recipe_it->recipeID();
	
					database->categorizeRecipe( recipe_id, categoryList );
				}
			}
		}

		delete editCategoriesDialog;
	}
}

void RecipeActionsHandler::edit()
{
	QList<Q3ListViewItem *> items = parentListView->selectedItems();
	if ( items.count() > 1 )
		KMessageBox::sorry( kapp->mainWidget(), i18n("Please select only one recipe."), i18n("Edit Recipe") );
	else if ( items.count() == 1 && items.at(0)->rtti() == 1000 ) {
		RecipeListItem * recipe_it = ( RecipeListItem* ) items.at(0);
		emit recipeSelected( recipe_it->recipeID(), 1 );
	}
	else //either nothing was selected or a category was selected
		KMessageBox::sorry( kapp->mainWidget(), i18n("No recipes selected."), i18n("Edit Recipe") );
}

void RecipeActionsHandler::recipeExport()
{
	QList<Q3ListViewItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		Q3ValueList<int> ids = recipeIDs( items );

		QString title;
		if ( items.count() == 1 && items.at(0)->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) items.at(0);
			title = recipe_it->title();
		}
		else
			title = i18n( "Recipes" );

		exportRecipes( ids, i18n( "Export Recipe" ), title, database );
	}
	else //if nothing selected, export all visible recipes
	{
		Q3ValueList<int> ids = getAllVisibleItems();
		if ( ids.count() > 0 ) {
			switch ( KMessageBox::questionYesNo( kapp->mainWidget(), i18n("No recipes are currently selected.\nWould you like to export all recipes in the current view?")) )
			{
			case KMessageBox::Yes:
				exportRecipes( ids, i18n( "Export Recipes" ), i18n( "Recipes" ), database );
				break;
			default: break;
			}
		}
		else
			KMessageBox::sorry( kapp->mainWidget(), i18n("No recipes selected."), i18n("Export") );
	}
}

void RecipeActionsHandler::removeFromCategory()
{
	QList<Q3ListViewItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QListIterator<Q3ListViewItem *> it(items);
		Q3ListViewItem *item;
		while ( it.hasNext() ) {
			item = it.next();
			if ( item->parent() != 0 ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) item;
				int recipe_id = recipe_it->recipeID();
	
				CategoryListItem *cat_it = ( CategoryListItem* ) item->parent();
				database->removeRecipeFromCategory( recipe_id, cat_it->categoryId() );
			}
		}
	}
}

void RecipeActionsHandler::remove()
{
	QList<Q3ListViewItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QListIterator<Q3ListViewItem*> it(items);
		Q3ListViewItem *item;
		while ( it.hasNext() ) {
			item = it.next();
			if ( item->rtti() == RECIPELISTITEM_RTTI ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) item;
				emit recipeSelected( recipe_it->recipeID(), 2 );
			}
		}
	}
}

void RecipeActionsHandler::addToShoppingList()
{
	QList<Q3ListViewItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		QListIterator<Q3ListViewItem *> it(items);
		Q3ListViewItem *item;
		while ( it.hasNext() ) {
			item = it.next();
			if ( item->parent() != 0 ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) item;
				emit recipeSelected( recipe_it->recipeID(), 3 );
			}
		}
	}
}

void RecipeActionsHandler::expandAll()
{
	Q3ListViewItemIterator it( parentListView );
	while ( it.current() ) {
		Q3ListViewItem * item = it.current();
		item->setOpen( true );
		++it;
	}
}

void RecipeActionsHandler::collapseAll()
{
	Q3ListViewItemIterator it( parentListView );
	while ( it.current() ) {
		Q3ListViewItem * item = it.current();
		item->setOpen( false );
		++it;
	}
}

void RecipeActionsHandler::exportRecipe( int id, const QString & caption, const QString &selection, RecipeDB *db )
{
	Q3ValueList<int> ids;
	ids.append( id );

	exportRecipes( ids, caption, selection, db );
}

void RecipeActionsHandler::exportRecipes( const Q3ValueList<int> &ids, const QString & caption, const QString &selection, RecipeDB *database )
{
	KFileDialog * fd = new KFileDialog( KUrl(),
	                                    QString( "*.kre|%1 (*.kre)\n"
	                                             "*.kreml|Krecipes (*.kreml)\n"
	                                             "*.txt|%3 (*.txt)\n"
	                                             //"*.cml|CookML (*.cml)\n"
	                                             "*|Web Book\n"
	                                             "*.html|%2 (*.html)\n"
	                                             "*.mmf|Meal-Master (*.mmf)\n"
	                                             "*.xml|RecipeML (*.xml)\n"
	                                             "*.rk|Rezkonv (*.rk)"
	                                              ).arg( i18n( "Compressed Krecipes format" ) ).arg( i18n( "Web page" ) ).arg( i18n("Plain Text") ),
	                                    0 );
	fd->setObjectName( "export_dlg" );
	fd->setModal( true );
	fd->setCaption( caption );
	fd->setOperationMode( KFileDialog::Saving );
	fd->setSelection( selection );
	fd->setMode( KFile::File | KFile::Directory );
	if ( fd->exec() == KFileDialog::Accepted ) {
		QString fileName = fd->selectedFile();
		if ( !fileName.isNull() ) {
			BaseExporter * exporter;
			if ( fd->currentFilter() == "*.xml" )
				exporter = new RecipeMLExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.mmf" )
				exporter = new MMFExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*" ) {
				CategoryTree *cat_structure = new CategoryTree;
				database->loadCategories( cat_structure );
				exporter = new HTMLBookExporter( cat_structure, fd->baseUrl().path(), "*.html" );
			}
			else if ( fd->currentFilter() == "*.html" ) {
				exporter = new HTMLExporter( fileName, fd->currentFilter() );
				XSLTExporter exporter_junk( fileName, "*.html" ); // AGH, i don't get build systems...
			}
			else if ( fd->currentFilter() == "*.cml" )
				exporter = new CookMLExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.txt" )
				exporter = new PlainTextExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.rk" )
				exporter = new RezkonvExporter( fileName, fd->currentFilter() );
			else {
				CategoryTree *cat_structure = new CategoryTree;
				database->loadCategories( cat_structure );
				exporter = new KreExporter( cat_structure, fileName, fd->currentFilter() );
			}

			int overwrite = -1;
			if ( QFile::exists( exporter->fileName() ) ) {
				overwrite = KMessageBox::warningYesNo( 0, i18n( "File \"%1\" exists.  Are you sure you want to overwrite it?" , exporter->fileName()), i18n( "Saving recipe" ) );
			}

			if ( overwrite == KMessageBox::Yes || overwrite == -1 ) {
				KProgressDialog progress_dialog( 0, QString::null, i18n( "Saving recipes..." ) );
				progress_dialog.setObjectName("export_progress_dialog");
				exporter->exporter( ids, database, &progress_dialog );
			}
			delete exporter;
		}
	}
	delete fd;
}

void RecipeActionsHandler::recipesToClipboard( const Q3ValueList<int> &ids, RecipeDB *db )
{
	KConfigGroup config = KGlobal::config()->group("Export");
	QString formatFilter = config.readEntry("ClipboardFormat");

	BaseExporter * exporter;
	if ( formatFilter == "*.xml" )
		exporter = new RecipeMLExporter( QString::null, formatFilter );
	else if ( formatFilter == "*.mmf" )
		exporter = new MMFExporter( QString::null, formatFilter );
	else if ( formatFilter == "*.cml" )
		exporter = new CookMLExporter( QString::null, formatFilter );
	else if ( formatFilter == "*.rk" )
		exporter = new RezkonvExporter( QString::null, formatFilter );
	else if ( formatFilter == "*.kre" || formatFilter == "*.kreml" ) {
		CategoryTree *cat_structure = new CategoryTree;
		db->loadCategories( cat_structure );
		exporter = new KreExporter( cat_structure, QString::null, formatFilter );
	}
	else //default to plain text
		exporter = new PlainTextExporter( QString::null, "*.txt" );

	RecipeList recipeList;
	db->loadRecipes( &recipeList, exporter->supportedItems(), ids );

	QString buffer;
	Q3TextStream stream(buffer,QIODevice::WriteOnly);
	exporter->writeStream(stream,recipeList);

	delete exporter;

	QApplication::clipboard()->setText(buffer);
}

void RecipeActionsHandler::recipesToClipboard()
{
	QList<Q3ListViewItem *> items = parentListView->selectedItems();
	if ( items.count() > 0 ) {
		Q3ValueList<int> ids = recipeIDs( items );

		recipesToClipboard(ids,database);
	}
}

Q3ValueList<int> RecipeActionsHandler::getAllVisibleItems()
{
	Q3ValueList<int> ids;

	Q3ListViewItemIterator iterator( parentListView );
	while ( iterator.current() ) {
		if ( iterator.current() ->isVisible() ) {
			if ( iterator.current() ->rtti() == RECIPELISTITEM_RTTI ) {
				RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
				int recipe_id = recipe_it->recipeID();
	
				if ( ids.find( recipe_id ) == ids.end() )
					ids.append( recipe_id );
			}
			//it is a category item and isn't populated, so get the unpopulated data from the database
			else if ( iterator.current()->rtti() == CATEGORYLISTITEM_RTTI && !iterator.current()->firstChild() ) {
				int cat_id = (( CategoryListItem* ) iterator.current())->element().id;
				ElementList list;
				database->loadRecipeList( &list, cat_id, true );
		
				for ( ElementList::const_iterator it = list.begin(); it != list.end(); ++it ) {
					if ( ids.find( (*it).id ) == ids.end() )
						ids << (*it).id;
				}
			}
		}

		++iterator;
	}

	return ids;
}

#include "recipeactionshandler.moc"

