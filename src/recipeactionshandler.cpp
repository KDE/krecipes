
#include "recipeactionshandler.h"

#include <qwidget.h>

#include <kfiledialog.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kprogress.h>

#include "exporters/cookmlexporter.h"
#include "exporters/htmlexporter.h"
#include "exporters/kreexport.h"
#include "exporters/mmfexporter.h"
#include "exporters/recipemlexporter.h"

#include "widgets/recipelistview.h"
#include "widgets/categorylistview.h"
#include "recipedb.h"

RecipeActionsHandler::RecipeActionsHandler( KListView *_parentListView, RecipeDB *db, int actions ) : QObject(_parentListView),
  parentListView(_parentListView),
  database(db)
{
	KIconLoader *il = new KIconLoader;
	
	kpop = new KPopupMenu( parentListView );
	if ( actions & Open )
		kpop->insertItem( il->loadIcon("ok", KIcon::NoGroup,16),i18n("&Open"), this, SLOT(open()), CTRL+Key_L );
	if ( actions & Edit )
		kpop->insertItem( il->loadIcon("edit", KIcon::NoGroup,16),i18n("&Edit"), this, SLOT(edit()), CTRL+Key_E );
	if ( actions & SaveAs )
		kpop->insertItem( il->loadIcon("filesaveas", KIcon::NoGroup,16),i18n("&Save as"), this, SLOT(saveAs()), CTRL+Key_S );
	if ( actions & RemoveFromCategory )
		remove_from_cat_item = kpop->insertItem( il->loadIcon("editshred", KIcon::NoGroup,16),i18n("Remove from &Category"), this, SLOT(removeFromCategory()), CTRL+Key_C );
	if ( actions & Remove )
		kpop->insertItem( il->loadIcon("editshred", KIcon::NoGroup,16),i18n("&Remove"), this, SLOT(remove()), CTRL+Key_R );
	kpop->polish();
	
	catPop = new KPopupMenu( parentListView );
	if ( actions & ExpandAll )
		catPop->insertItem( i18n("&Expand All"), this, SLOT(expandAll()), CTRL+Key_Plus );
	if ( actions & CollapseAll )
		catPop->insertItem( i18n("&Collapse All"), this, SLOT(collapseAll()), CTRL+Key_Minus );
	if ( actions & SaveCategoryAs )
		catPop->insertItem( il->loadIcon("filesaveas", KIcon::NoGroup,16),i18n("&Save as"), this, SLOT(saveCategoryAs()), CTRL+Key_S );
	catPop->polish();

	delete il;

	connect(parentListView,SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)), SLOT(showPopup(KListView *, QListViewItem *, const QPoint &)));
	connect(parentListView,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )), SLOT(open()));
}

void RecipeActionsHandler::exec( ItemType type, const QPoint &p )
{
	if ( type == Recipe )
	{
		if ( kpop->idAt(0) != -1 )
			kpop->exec(p);
	}
	else if ( type == Category )
	{
		if ( catPop->idAt(0) != -1 )
			catPop->exec(p);
	}
}

void RecipeActionsHandler::showPopup( KListView */*l*/, QListViewItem *i, const QPoint &p )
{
	if (i) { // Check if the QListViewItem actually exists
		if ( i->rtti() == 1000 ) {
			kpop->setItemVisible( remove_from_cat_item, i->parent() != 0 );
			exec(Recipe,p);
		}
		else if ( i->rtti() == 1001 && i->firstChild() ) //is a category... don't pop-up for an empty category though
			exec(Category,p);
	}
}

void RecipeActionsHandler::open()
{
	QListViewItem *it = parentListView->selectedItem();
	if ( it )
	{
		if ( it->rtti() == 1000 ) { //RecipeListItem
			RecipeListItem *recipe_it = (RecipeListItem*)it;
			emit recipeSelected(recipe_it->recipeID(),0);
		}
		else if ( it->rtti() == 1001 && it->firstChild() ) //CategoryListItem and not empty
		{
			QValueList<int> ids;
			
			//do this to only iterate over children of 'it'
			QListViewItem *pEndItem = NULL;
			QListViewItem *pStartItem = it;
			do
			{
				if(pStartItem->nextSibling())
					pEndItem = pStartItem->nextSibling();
				else
					pStartItem = pStartItem->parent();
			}
			while(pStartItem && !pEndItem);
			
			QListViewItemIterator iterator(it);
			while(iterator.current() != pEndItem)
			{
				if ( iterator.current()->rtti() == 1000 ) {
					RecipeListItem *recipe_it = (RecipeListItem*)iterator.current();
					if ( ids.find(recipe_it->recipeID()) == ids.end() ) {
						ids.append(recipe_it->recipeID());
					}
				}
				++iterator;
			}
			emit recipesSelected( ids, 0 );
		}
	}
}

void RecipeActionsHandler::edit()
{
	QListViewItem *it = parentListView->selectedItem();
	if ( it ) {
		if ( it->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*)it;
			emit recipeSelected(recipe_it->recipeID(),1);
		}
	}
}

void RecipeActionsHandler::saveAs()
{
	if ( parentListView->selectedItem() )
	{
		if ( parentListView->selectedItem()->firstChild() )
			saveCategoryAs();
		else
		{
			if ( parentListView->selectedItem()->rtti() == 1000 ) {
				RecipeListItem *recipe_it = (RecipeListItem*)parentListView->selectedItem();
				int recipe_id = recipe_it->recipeID();

				QValueList<int> id;
				id.append( recipe_id );
		
				exportRecipes( id, i18n("Save Recipe"), recipe_it->title() );
			}
		}
	}
	else //if nothing selected, export all visible recipes
	{
		QValueList<int> ids = getAllVisibleItems();

		if ( ids.count() > 0 )
			exportRecipes( ids, i18n("Save Recipes"), "Recipes" );
		//else TODO: give notice
	}
}

void RecipeActionsHandler::removeFromCategory()
{
	QListViewItem *it = parentListView->selectedItem();
	if ( it && it->rtti() == 1000 ) {
		if (it->parent() != 0) {
			RecipeListItem *recipe_it = (RecipeListItem*)it;
			int recipe_id = recipe_it->recipeID();

			CategoryListItem *cat_it = (CategoryListItem*)it->parent();
			database->removeRecipeFromCategory(recipe_id, cat_it->categoryId());
		}
	}
}

void RecipeActionsHandler::remove()
{
	QListViewItem *it = parentListView->selectedItem();
	if ( it && it->rtti() == 1000 ) {
		RecipeListItem *recipe_it = (RecipeListItem*)it;
		emit recipeSelected(recipe_it->recipeID(),2);
	}
}

void RecipeActionsHandler::expandAll()
{
	QListViewItemIterator it( parentListView );
	while ( it.current() ) {
		QListViewItem *item = it.current();
		item->setOpen(true);
		++it;
	}
}

void RecipeActionsHandler::collapseAll()
{
	QListViewItemIterator it( parentListView );
	while ( it.current() ) {
		QListViewItem *item = it.current();
		item->setOpen(false);
		++it;
	}
}

void RecipeActionsHandler::saveCategoryAs()
{
	if (parentListView->selectedItem() )
	{
		CategoryListItem *cat_it = (CategoryListItem*)parentListView->selectedItem();
		QValueList<int> ids;

		//do this to only iterate over children of 'cat_it'
		QListViewItem *pEndItem = NULL;
		QListViewItem *pStartItem = cat_it;
		do
		{
			if(pStartItem->nextSibling())
				pEndItem = pStartItem->nextSibling();
			else
				pStartItem = pStartItem->parent();
		}
		while(pStartItem && !pEndItem);
		
		QListViewItemIterator iterator(cat_it);
		while(iterator.current() != pEndItem)
		{
			if ( iterator.current()->rtti() == 1000 )
			{
				RecipeListItem *recipe_it = (RecipeListItem*)iterator.current();
				int recipe_id = recipe_it->recipeID();
	
				if ( ids.find(recipe_id) == ids.end() )
					ids.append(recipe_id);
			}
			++iterator;
		}

		exportRecipes( ids, i18n("Save Recipes"), cat_it->categoryName() );
	}
}

void RecipeActionsHandler::exportRecipes( const QValueList<int> &ids, const QString & caption, const QString &selection )
{
	KFileDialog* fd = new KFileDialog( QString::null,
	"*.kre|Gzip Krecipes file (*.kre)\n"
	"*.kreml|Krecipes xml file (*.kreml)\n"
	"*.cml|CookML file (*.cml)\n"
	"*.html|HTML file (*.html)\n"
	"*.mmf|Meal-Master file (*.mmf)\n"
	"*.xml|RecipeML file (*.xml)",
	parentListView, "export_dlg", true);
	fd->setCaption( caption );
	fd->setOperationMode( KFileDialog::Saving );
	fd->setSelection( selection );
	if ( fd->exec() == KFileDialog::Accepted )
	{
		QString fileName = fd->selectedFile();
		if( !fileName.isNull() )
		{
			BaseExporter *exporter;
			if ( fd->currentFilter() == "*.xml" )
				exporter = new RecipeMLExporter(fileName, fd->currentFilter());
			else if ( fd->currentFilter() == "*.mmf" )
				exporter = new MMFExporter(fileName, fd->currentFilter());
			else if ( fd->currentFilter() == "*.html" )
				exporter = new HTMLExporter(database, fileName, fd->currentFilter(), 650);
			else if ( fd->currentFilter() == "*.cml" )
				exporter = new CookMLExporter(fileName, fd->currentFilter());
			else
				exporter = new KreExporter(fileName, fd->currentFilter());

			int overwrite = -1;
			if ( QFile::exists( exporter->fileName() ) )
			{
				overwrite = KMessageBox::warningYesNo( 0,QString(i18n("File \"%1\" exists.  Are you sure you want to overwrite it?")).arg(exporter->fileName()),i18n("Saving recipe") );
			}

			if ( overwrite == KMessageBox::Yes || overwrite == -1 )
			{
				KProgressDialog progress_dialog(parentListView, "export_progress_dialog", QString::null, i18n("Preparing to save recipes...") );
				progress_dialog.setAutoClose(false); progress_dialog.setAutoReset(true);
				RecipeList recipes; database->loadRecipes( &recipes, ids, &progress_dialog );
				progress_dialog.setAutoReset(false);

				progress_dialog.setLabel( i18n("Saving recipes...") );
				exporter->exporter( recipes, &progress_dialog );
			}
			delete exporter;
		}
	}
	delete fd;
}

QValueList<int> RecipeActionsHandler::getAllVisibleItems()
{
	QValueList<int> ids;

	QListViewItemIterator iterator(parentListView,QListViewItemIterator::Visible);
	while(iterator.current())
	{
		if ( iterator.current()->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*)iterator.current();
			int recipe_id = recipe_it->recipeID();

			if ( ids.find(recipe_id) == ids.end() )
				ids.append(recipe_id);
		}

		++iterator;
	}

	return ids;
}

#include "recipeactionshandler.moc"

