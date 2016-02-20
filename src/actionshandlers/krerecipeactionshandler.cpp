/***************************************************************************
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                    *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>          *
*   Copyright © 2015 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "krerecipeactionshandler.h"

//#include "kdebug.h"

#include "dialogs/recipeinput/selectcategoriesdialog.h"
#include "widgets/krerecipeslistwidget.h"
#include "exporters/cookmlexporter.h"
#include "exporters/xsltexporter.h"
#include "exporters/htmlexporter.h"
#include "exporters/htmlbookexporter.h"
#include "exporters/kreexporter.h"
#include "exporters/mmfexporter.h"
#include "exporters/recipemlexporter.h"
#include "exporters/mx2exporter.h"
#include "exporters/plaintextexporter.h"
#include "exporters/rezkonvexporter.h"
#include "backends/recipedb.h"

#include <KAction>
#include <KLocale>
#include <KFileDialog>
#include <KMessageBox>
#include <KTempDir>
#include <KStandardDirs>
#include <KWebPage>
#include <KMenu>

#include <QClipboard>
#include <QPointer>
#include <QModelIndex>
#include <QWebFrame>
#include <QPrintPreviewDialog>
#include <QPrinter>



KreRecipeActionsHandler::KreRecipeActionsHandler( KreRecipesListWidget * listWidget, RecipeDB * db ):
	KreGenericActionsHandler( listWidget, db ),
	m_categorizeAction( 0 ),
	m_removeFromCategoryAction( 0 )
{

	m_recipesMenu = m_contextMenu; //Re-use the menu from the base class
	m_categoriesMenu = new KMenu( listWidget );

	connect( listWidget, SIGNAL( selectionChanged() ),
		this, SLOT( selectionChangedSlot() ) );

	connect( listWidget, SIGNAL( doubleClicked(const QModelIndex &) ),
		this, SLOT( open() ) );

	connect( this, SIGNAL( printDone() ), 
		this, SLOT( printDoneSlot() ), Qt::QueuedConnection );
}

void KreRecipeActionsHandler::addRecipeAction( KAction * action )
{
	m_recipesMenu->addAction( action );
}

void KreRecipeActionsHandler::addCategoryAction( KAction * action )
{
	m_categoriesMenu->addAction( action );
}

void KreRecipeActionsHandler::setCategorizeAction( KAction * action )
{
	m_categorizeAction = action;
	m_recipesMenu->addAction( action );
}

void KreRecipeActionsHandler::setRemoveFromCategoryAction( KAction * action )
{
	m_removeFromCategoryAction = action;
	m_recipesMenu->addAction( action );
}

void KreRecipeActionsHandler::showPopup( const QModelIndex & index, const QPoint & point )
{
        if (index.isValid() ) {
		if ( index.data(KreRecipesListWidget::ItemTypeRole) 
		== KreRecipesListWidget::RecipeItem ) {
			if ( index.data(KreRecipesListWidget::CategorizedRole)
			== KreRecipesListWidget::Uncategorized ) {
				if ( m_categorizeAction )
					m_categorizeAction->setVisible( true );
				if ( m_removeFromCategoryAction )
					m_removeFromCategoryAction->setVisible( false );
			} else {
				if ( m_categorizeAction )
					m_categorizeAction->setVisible( false );
				if ( m_removeFromCategoryAction )
					m_removeFromCategoryAction->setVisible( true );
			}
                	m_recipesMenu->exec( point );
		} else if ( index.data(KreRecipesListWidget::ItemTypeRole) 
		== KreRecipesListWidget::CategoryItem ) {
                	m_categoriesMenu->exec( point );
		} 
	}
}

void KreRecipeActionsHandler::createNew()
{
	//Reimplementing because the base class demands it,
	//but right now this is not needed to create a new recipe
}

void KreRecipeActionsHandler::open()
{
	KreRecipesListWidget* listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	
	int numberOfRecipes = ids.count();
	if ( numberOfRecipes == 1 ) {
		emit recipeSelected( ids.first(), 0 );
	} else if ( numberOfRecipes > 0 ) {
		emit recipesSelected( ids, 0 );
	}

}

void KreRecipeActionsHandler::categorize()
{
	KreRecipesListWidget* listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	if ( !ids.isEmpty() ) {
		ElementList categoryList;
		QPointer<SelectCategoriesDialog> editCategoriesDialog = 
			new SelectCategoriesDialog( m_listWidget, categoryList, m_database );
	
		if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
			editCategoriesDialog->getSelectedCategories( &categoryList ); // get the category list chosen
			
			for ( QList<int>::const_iterator it = ids.constBegin(); it != ids.constEnd(); it++ ) {
				m_database->categorizeRecipe( *it, categoryList );
			}
		}

		delete editCategoriesDialog;
	}
}

void KreRecipeActionsHandler::edit()
{
	KreRecipesListWidget* listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	
	int numberOfRecipes = ids.count();
	if ( numberOfRecipes > 1 ) {
		KMessageBox::sorry( listWidget, i18n("Please select only one recipe."), i18n("Edit Recipe") );
	} else if ( numberOfRecipes == 1 ) {
		emit recipeSelected( ids.first(), 1 );
	} else { 
		//either nothing was selected or a category was selected
		KMessageBox::sorry( listWidget, i18n("No recipes selected."), i18n("Edit Recipe") );
	}
}

void KreRecipeActionsHandler::recipeExport()
{
	KreRecipesListWidget * listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	if ( !ids.isEmpty() ) {
		QString title;
		if ( ids.count() == 1 ) {
			title = listWidget->getRecipeName( ids.first() );
		} else {			
			title = i18n( "Recipes" );
		}
		exportRecipes( ids, i18n( "Export Recipe" ), title, m_database );
	} else { //if nothing selected, export all visible recipes
		ids = listWidget->getAllVisibleRecipes();
		if ( !ids.isEmpty() ) {
			if ( KMessageBox::questionYesNo( listWidget, 
			i18n("No recipes are currently selected.\n"
			"Would you like to export all recipes in the current view?") )
			== KMessageBox::Yes ) {
				exportRecipes( ids, i18n( "Export Recipes" ), i18n( "Recipes" ), m_database );
			}
		} else {
			KMessageBox::sorry( listWidget, i18n("No recipes selected."), i18n("Export") );
		}
	}
}

void KreRecipeActionsHandler::recipePrint()
{
	KreRecipesListWidget * listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	if ( !ids.isEmpty() ) {
		printRecipes( ids, m_database );
	} else { //if nothing selected, print all visible recipes
		QList<int> ids = listWidget->getAllVisibleRecipes();
		if ( !ids.isEmpty() ) {
			if ( KMessageBox::questionYesNo( listWidget,
			i18n("No recipes are currently selected.\n"
			"Would you like to print all recipes in the current view?")  )
			 == KMessageBox::Yes )
				printRecipes( ids, m_database );
		} else {
			KMessageBox::sorry( listWidget, 
				i18n("No recipes selected."), i18n("Print") );
		}
	}
}

void KreRecipeActionsHandler::removeFromCategory()
{
	KreRecipesListWidget* listWidget = (KreRecipesListWidget*)m_listWidget;
	QList< QPair<int,int> > ids = listWidget->selectedRecipesWithCategories();

	if ( !ids.isEmpty() ) {
		QList< QPair<int,int> >::const_iterator it;
		for ( it = ids.constBegin(); it != ids.constEnd(); it++ ) {
			m_database->removeRecipeFromCategory( it->first, it->second );
		}
	}
}

void KreRecipeActionsHandler::remove()
{
	KreRecipesListWidget* listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	
	emit recipesSelected( ids, 2 );
}

void KreRecipeActionsHandler::addToShoppingList()
{
	KreRecipesListWidget * listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	for ( QList<int>::const_iterator it = ids.constBegin(); it != ids.constEnd(); it++ ) {
		emit recipeSelected( *it , 3 );
	}
}

void KreRecipeActionsHandler::expandAll()
{
	KreRecipesListWidget * listWidget = (KreRecipesListWidget*)m_listWidget;
	listWidget->expandAll();
}

void KreRecipeActionsHandler::collapseAll()
{
	KreRecipesListWidget * listWidget = (KreRecipesListWidget*)m_listWidget;
	listWidget->collapseAll();
}

void KreRecipeActionsHandler::exportRecipes( const QList<int> &ids, const QString & caption, const QString &selection, RecipeDB *database )
{
	KFileDialog * fd = new KFileDialog( KUrl(),
		QString( "*.kre|%1 (*.kre)\n"
		"*.kreml|Krecipes (*.kreml)\n"
		"*.txt|%3 (*.txt)\n"
		//"*.cml|CookML (*.cml)\n"
		"*|%4\n"
		"*.html|%2 (*.html)\n"
		"*.mmf|Meal-Master (*.mmf)\n"
		"*.xml|RecipeML (*.xml)\n"
		"*.mx2|MasterCook (*.mx2)\n"
		"*.rk|Rezkonv (*.rk)"
		).arg( i18n( "Compressed Krecipes format" ) )
		.arg( i18n( "Web page" ) )
		.arg( i18n("Plain Text") )
		.arg( i18n("Web Book") ),
	0 );
	fd->setObjectName( "export_dlg" );
	fd->setModal( true );
	fd->setCaption( caption );
	fd->setOperationMode( KFileDialog::Saving );
	fd->setSelection( selection );
	fd->setMode( KFile::File | KFile::Directory );
	if ( fd->exec() == KFileDialog::Accepted ) {
		QString fileName = fd->selectedFile();
		if ( !fileName.isEmpty() ) {
			BaseExporter * exporter;
			if ( fd->currentFilter() == "*.xml" )
				exporter = new RecipeMLExporter( fileName, fd->currentFilter() );
			else if ( fd->currentFilter() == "*.mx2" )
			    exporter = new MX2Exporter( fileName, fd->currentFilter() );
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
				overwrite = KMessageBox::warningYesNo( 0, i18n( "File \"%1\" exists.  Are you sure you want to overwrite it?" , exporter->fileName()), i18nc( "@title:window", "Saving recipe" ) );
			}

			if ( overwrite == KMessageBox::Yes || overwrite == -1 ) {
				KProgressDialog progress_dialog( 0, QString(), i18nc( "@info:progress", "Saving recipes..." ) );
				progress_dialog.setObjectName("export_progress_dialog");
				exporter->exporter( ids, database, &progress_dialog );
			}
			delete exporter;
		}
	}
	delete fd;
}

void KreRecipeActionsHandler::printRecipes( const QList<int> &ids, RecipeDB *database )
{
	//Create the temporary directory.
	m_tempdir = new KTempDir(KStandardDirs::locateLocal("tmp", "krecipes-data-print"));
	QString tmp_filename = m_tempdir->name() + "krecipes_recipe_view.html";
	//Export to HTML in the temporary directory.
	XSLTExporter html_generator( tmp_filename, "html" );
	KConfigGroup config(KGlobal::config(), "Page Setup" );
	QString styleFile = config.readEntry( "PrintLayout", KStandardDirs::locate( "appdata", "layouts/None.klo" ) );
	if ( !styleFile.isEmpty() && QFile::exists( styleFile ) )
		html_generator.setStyle( styleFile );

	QString templateFile = config.readEntry( "PrintTemplate", KStandardDirs::locate( "appdata", "layouts/Default.xsl" ) );
	if ( !templateFile.isEmpty() && QFile::exists( templateFile ) )
		html_generator.setTemplate( templateFile );
	html_generator.exporter( ids, database );
	//Load the generated HTML. When loaded, RecipeActionsHandlerView::print(...) will be called.
	m_printPage = new KWebPage;
	connect(m_printPage, SIGNAL(loadFinished(bool)), SLOT(print(bool)));
	m_printPage->mainFrame()->load( KUrl(tmp_filename) );
}

void KreRecipeActionsHandler::print(bool ok)
{
	Q_UNUSED(ok)
	QPrinter printer;
	QPointer<QPrintPreviewDialog> previewdlg = new QPrintPreviewDialog(&printer);
	//Show the print preview dialog.
	connect(previewdlg, SIGNAL(paintRequested(QPrinter *)),
		m_printPage->mainFrame(), SLOT(print(QPrinter *)));
	previewdlg->exec();
	delete previewdlg;
	//Remove the temporary directory which stores the HTML and free memory.
	m_tempdir->unlink();
	delete m_tempdir;
	emit( printDone() );
}

void KreRecipeActionsHandler::printDoneSlot()
{
	delete m_printPage;
}

void KreRecipeActionsHandler::recipesToClipboard()
{
	KreRecipesListWidget * listWidget = (KreRecipesListWidget*)m_listWidget;

	QList<int> ids = listWidget->selectedRecipes();

	if ( ids.isEmpty() ) {
		return;
	}

	KConfigGroup config = KGlobal::config()->group("Export");
	QString formatFilter = config.readEntry("ClipboardFormat");

	BaseExporter * exporter;
	if ( formatFilter == "*.xml" ) {
		exporter = new RecipeMLExporter( QString(), formatFilter );
	} else if ( formatFilter == "*.mx2" ) {
		exporter = new MX2Exporter( QString(), formatFilter );
	} else if ( formatFilter == "*.mmf" ) {
		exporter = new MMFExporter( QString(), formatFilter );
	} else if ( formatFilter == "*.cml" ) {
		exporter = new CookMLExporter( QString(), formatFilter );
	} else if ( formatFilter == "*.rk" ) {
		exporter = new RezkonvExporter( QString(), formatFilter );
	} else if ( formatFilter == "*.kre" || formatFilter == "*.kreml" ) {
		CategoryTree *cat_structure = new CategoryTree;
		m_database->loadCategories( cat_structure );
		exporter = new KreExporter( cat_structure, QString::null, formatFilter );
	} else { //default to plain text
		exporter = new PlainTextExporter( QString(), "*.txt" );
	}

	RecipeList recipeList;
	m_database->loadRecipes( &recipeList, exporter->supportedItems(), ids );

	QString buffer;
	QTextStream stream(&buffer,QIODevice::WriteOnly);
	exporter->writeStream(stream,recipeList);

	delete exporter;

	QApplication::clipboard()->setText(buffer);
}

void KreRecipeActionsHandler::selectionChangedSlot()
{
	KreRecipesListWidget* listWidget = (KreRecipesListWidget*)m_listWidget;
	QList<int> ids = listWidget->selectedRecipes();
	if ( ids.count() == 1 ) {
		// We have a single recipe as our selection
		emit recipeSelected( ids.first(), 4 );
		emit recipeSelected( true );
	} else {
		emit recipeSelected( 0, 5 ); //id doesn't matter here
		emit recipeSelected( false );
	}
}

void KreRecipeActionsHandler::saveElement( const QModelIndex& topLeft, 
	const QModelIndex& bottomRight )
{
	Q_UNUSED(topLeft)
	Q_UNUSED(bottomRight)
	//Right now, we don't need to do anything here, since renaming items
	//is disabled for KreRecipesListWidget.
}

