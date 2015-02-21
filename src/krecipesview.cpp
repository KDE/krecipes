/*****************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                      *
*   Copyright © 2003-2004 Cyril Bosselut <bosselut@b1project.com>            *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>               *
*   Copyright © 2009-2012 José Manuel Santamaría Lema <panfaust@gmail.com>   *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "krecipesview.h"

//Added by qt3to4:

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kglobal.h>
#include <kvbox.h>
#include <QFrame>

#include "actionshandlers/kreauthoractionshandler.h"
#include "actionshandlers/recipeactionshandler.h"
#include "actionshandlers/unitactionshandler.h"
#include "actionshandlers/krecategoryactionshandler.h"
#include "setupassistant.h"
#include "convert_sqlite3.h"
#include "kstartuplogo.h"

#include "dialogs/recipeinputdialog.h"
#include "dialogs/recipeviewdialog.h"
#include "dialogs/selectrecipedialog.h"
#include "dialogs/ingredientsdialog.h"
#include "dialogs/propertiesdialog.h"
#include "dialogs/shoppinglistdialog.h"
#include "dialogs/dietwizarddialog.h"
#include "dialogs/categorieseditordialog.h"
#include "dialogs/authorsdialog.h"
#include "dialogs/unitsdialog.h"
#include "dialogs/prepmethodsdialog.h"
#include "dialogs/ingredientmatcherdialog.h"

#include "widgets/kremenu.h"
#include "widgets/paneldeco.h"

#include "backends/progressinterface.h"

#include "profiling.h"
#include "krecipesadaptor.h"

KrecipesView::KrecipesView( QWidget *parent )
	: QWidget( parent ), m_actionshandler( 0, 0 )
{
	new KrecipesAdaptor(this );
	QDBusConnection::sessionBus().registerObject("/Krecipes", this);

#ifndef NDEBUG
	QTime dbg_total_timer; dbg_total_timer.start();
#endif
	// Init the setup wizard if necessary
	kDebug() << "Beginning wizard" ;
	wizard();
	kDebug() << "Wizard finished correctly" ;

	// Show Splash Screen

	KStartupLogo* start_logo = 0L;
	start_logo = new KStartupLogo();
	start_logo -> setHideEnabled( true );
	start_logo->show();
	start_logo->raise();

	// Initialize Database

	// Check if the database type is among those supported
	// and initialize the database in each case
	START_TIMER("Initializing database")
	initDatabase();
	END_TIMER()

	// Design the GUI
	QHBoxLayout *layout = new QHBoxLayout;
	setLayout( layout );
	splitter = new KHBox( this );
	layout->addWidget( splitter );
	// Create Left and Right Panels (splitter)

	leftPanelFrame = new QFrame( splitter );
	leftPanel = new KreMenu;
	QHBoxLayout *leftPanelFrameLayout = new QHBoxLayout;
	leftPanelFrame->setLayout( leftPanelFrameLayout );
	leftPanelFrameLayout->addWidget( leftPanel );
	leftPanelFrameLayout->setMargin( 0 );
	leftPanelFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );
	leftPanelFrame->setFrameRect( QRect( 0, 0, 0, 0 ) );
	rightPanel = new PanelDeco( splitter, i18n( "Find/Edit Recipes" ), "system-search" );

	// Design Left Panel

	START_TIMER("Setting up buttons")
	// Buttons

	button0 = new KreMenuButton( leftPanel, SelectP );
	button0->setIconSet( KIcon( "system-search" ) );
	buttonsList.append( button0 );

	button1 = new KreMenuButton( leftPanel, ShoppingP );
	button1->setIconSet( KIcon( "view-pim-tasks" ) );
	buttonsList.append( button1 );

	button7 = new KreMenuButton( leftPanel, DietP );
	button7->setIconSet( KIcon( "diet" ) );
	buttonsList.append( button7 );

	button8 = new KreMenuButton( leftPanel, MatcherP );
	button8->setIconSet( KIcon( "view-filter" ) );
	buttonsList.append( button8 );


	// Submenus
	dataMenu = leftPanel->createSubMenu( i18n( "Data..." ), "server-database" );
	
	recipeButton = new KreMenuButton( leftPanel, RecipeEdit );
	recipeButton->setIconSet( KIcon( "document-save" ) );
	buttonsList.append( recipeButton );
	recipeButton->setEnabled( false );
	recipeButton->hide();

	button2 = new KreMenuButton( leftPanel, IngredientsP, dataMenu );
	button2->setIconSet( KIcon( "ingredients" ) );
	buttonsList.append(button2);

	button3 = new KreMenuButton( leftPanel, PropertiesP, dataMenu );
	button3->setIconSet( KIcon( "properties" ) );
	buttonsList.append( button3 );

	button4 = new KreMenuButton( leftPanel, UnitsP, dataMenu );
	button4->setIconSet( KIcon( "units" ) );
	buttonsList.append( button4 );

	button9 = new KreMenuButton( leftPanel, PrepMethodsP, dataMenu );
	button9->setIconSet( KIcon( "methods" ) );
	buttonsList.append( button9 );

	button5 = new KreMenuButton( leftPanel, CategoriesP, dataMenu );
	button5->setIconSet( KIcon( "folder-yellow" ) );
	buttonsList.append( button5 );

	button6 = new KreMenuButton( leftPanel, AuthorsP, dataMenu );
	button6->setIconSet( KIcon( "authors" ) );
	buttonsList.append( button6 );	

	contextButton = new QPushButton( leftPanel );
	contextButton->setObjectName( "contextButton" );
	contextButton->setIcon( KIcon( "system-help" ) );
	contextButton->setGeometry( leftPanel->width() - 42, leftPanel->height() - 42, 32, 32 );

	QPalette p = palette();
	p.setColor(backgroundRole(), contextButton->palette().color(backgroundRole()).light( 140 ) );
	contextButton->setPalette(p);
	contextButton->setFlat( true );
	END_TIMER()

	KConfigGroup config(KGlobal::config(), "Performance" );
	int limit = config.readEntry( "CategoryLimit", -1 );
	database->updateCategoryCache(limit);

	// Right Panel Widgets
	START_TIMER("Creating input dialog")
	inputPanel = new RecipeInputDialog( rightPanel, database );
	rightPanel->addStackWidget( inputPanel );
	END_TIMER()

	START_TIMER("Creating recipe view")
	viewPanel = new RecipeViewDialog( rightPanel, database );
	rightPanel->addStackWidget( viewPanel );
	END_TIMER()

	START_TIMER("Creating recipe selection dialog")
	selectPanel = new SelectRecipeDialog( rightPanel, database );
	rightPanel->addStackWidget( selectPanel );

	END_TIMER()

	START_TIMER("Creating ingredients component")
	ingredientsPanel = new IngredientsDialog( rightPanel, database );
	rightPanel->addStackWidget( ingredientsPanel );
	END_TIMER()

	START_TIMER("Creating properties component")
	propertiesPanel = new PropertiesDialog( rightPanel, database );
	rightPanel->addStackWidget( propertiesPanel );

	END_TIMER()

	START_TIMER("Creating units component")
	unitsPanel = new UnitsDialog( rightPanel, database );
	rightPanel->addStackWidget( unitsPanel );

	END_TIMER()

	START_TIMER("Creating shopping list dialog")
	shoppingListPanel = new ShoppingListDialog( rightPanel, database );
	rightPanel->addStackWidget( shoppingListPanel );

	END_TIMER()

	START_TIMER("Creating diet wizard dialog")
	dietPanel = new DietWizardDialog( rightPanel, database );
	rightPanel->addStackWidget( dietPanel );

	END_TIMER()

	START_TIMER("Creating categories component")
	categoriesPanel = new CategoriesEditorDialog( rightPanel, database );
	rightPanel->addStackWidget( categoriesPanel );

	END_TIMER()

	START_TIMER("Creating authors component")
	authorsPanel = new AuthorsDialog( rightPanel, database );
	rightPanel->addStackWidget( authorsPanel );

	END_TIMER()

	START_TIMER("Creating prep methods component")
	prepMethodsPanel = new PrepMethodsDialog( rightPanel, database );
	rightPanel->addStackWidget( prepMethodsPanel );

	END_TIMER()

	START_TIMER("Creating ingredients matcher dialog")
	ingredientMatcherPanel = new IngredientMatcherDialog( rightPanel, database );
	rightPanel->addStackWidget( ingredientMatcherPanel );

	END_TIMER()

	database->clearCategoryCache();

	// Use to keep track of the panels
	panelMap.insert( inputPanel, RecipeEdit );
	panelMap.insert( viewPanel, RecipeView );
	panelMap.insert( selectPanel, SelectP );
	panelMap.insert( ingredientsPanel, IngredientsP );
	panelMap.insert( propertiesPanel, PropertiesP );
	panelMap.insert( unitsPanel, UnitsP );
	panelMap.insert( shoppingListPanel, ShoppingP );
	panelMap.insert( dietPanel, DietP );
	panelMap.insert( categoriesPanel, CategoriesP );
	panelMap.insert( authorsPanel, AuthorsP );
	panelMap.insert( prepMethodsPanel, PrepMethodsP );
	panelMap.insert( ingredientMatcherPanel, MatcherP );

	m_activePanel = SelectP;
	m_previousActivePanel = SelectP;
	slotSetPanel( SelectP );

	// i18n
	translate();


	// Connect Signals from Left Panel to slotSetPanel()
	connect( leftPanel, SIGNAL( clicked( KrePanel ) ), this, SLOT( slotSetPanel( KrePanel ) ) );

	connect( contextButton, SIGNAL( clicked() ), SLOT( activateContextHelp() ) );

	connect( leftPanel, SIGNAL( resized( int, int ) ), this, SLOT( resizeRightPane( int, int ) ) );


	// Retransmit signal to parent to Enable/Disable the Save Button
	connect ( inputPanel, SIGNAL( enableSaveOption( bool ) ), this, SLOT( enableSaveOptionSlot( bool ) ) );

	// Create a new button when a recipe is unsaved
	connect ( inputPanel, SIGNAL( createButton( QWidget*, const QString & ) ), this, SLOT( addRecipeButton( QWidget*, const QString & ) ) );

	// Connect Signals from selectPanel (SelectRecipeDialog)

	connect ( selectPanel, SIGNAL( recipeSelected( int, int ) ), this, SLOT( actionRecipe( int, int ) ) );
	connect ( selectPanel, SIGNAL( recipesSelected( const QList<int>&, int ) ), this, SLOT( actionRecipes( const QList<int>&, int ) ) );

	// Connect Signals from ingredientMatcherPanel (IngredientMatcherDialog)

	connect ( ingredientMatcherPanel, SIGNAL( recipeSelected( int, int ) ), SLOT( actionRecipe( int, int ) ) );

	// Close a recipe when requested (just switch panels)
	connect( inputPanel, SIGNAL( closeRecipe() ), this, SLOT( closeRecipe() ) );

	// Show a recipe when requested (just switch panels)
	connect( inputPanel, SIGNAL( showRecipe( int ) ), this, SLOT( showRecipe( int ) ) );

	// Close the recipe view when requested (just switch panels)
	connect( viewPanel, SIGNAL( closeRecipeView() ), this, SLOT( closeRecipe() ) );

	// Create a new shopping list when a new diet is generated and accepted
	connect( dietPanel, SIGNAL( dietReady() ), this, SLOT( createShoppingListFromDiet() ) );

	// Place the Tip Button in correct position when the left pane is resized
	connect( leftPanel, SIGNAL( resized( int, int ) ), this, SLOT( moveTipButton( int, int ) ) );

	connect( rightPanel, SIGNAL( panelRaised( QWidget*, QWidget* ) ), SLOT( panelRaised( QWidget*, QWidget* ) ) );

	connect( selectPanel, SIGNAL( recipeSelected(bool) ), SIGNAL( recipeSelected(bool) ) );
	
	connect( ingredientMatcherPanel, SIGNAL( recipeSelected(bool) ), SIGNAL( recipeSelected(bool) ) );

	// Close Splash Screen
	delete start_logo;

#ifndef NDEBUG
	kDebug()<<"Total time elapsed: "<<dbg_total_timer.elapsed()/1000<<" sec";
#endif
}

KrecipesView::~KrecipesView()
{
	qDeleteAll(buttonsList);
	delete viewPanel; //manually delete viewPanel because we need to be sure it is deleted
	//before the database is because its destructor uses 'database'
	delete database;
}

bool KrecipesView::questionRerunWizard( const QString &message, const QString &errormsg,
	RecipeDB::Error error )
{
	if ( (error == RecipeDB::FixDbFailed) && (dbType == "SQLite") ) {
		QString finalMessage = message + ' ' +
		i18n( "\nYou are using SQLite; this error is often caused by using an SQLite 2 "
		"database with Krecipes supporting SQLite 3, if this is the case you could run "
		"the SQLite converter.\n"
		"What do you want to do?" );
		int answer = KMessageBox::questionYesNoCancel( this, finalMessage, "",
			KGuiItem( i18n("Run the setup assistant"), KIcon("plasmagik") ),
			KGuiItem( i18n("Run the SQLite converter"), KIcon("document-revert") ),
			KStandardGuiItem::quit()
		);
		if ( answer == KMessageBox::Yes ) {
			wizard( true );
		} else if ( answer == KMessageBox::No ) {
			ConvertSQLite3 converter;
			converter.convert();
		} else {
			kError() << errormsg << ". Exiting" ;
			kapp->exit( 1 ); exit ( 1 ); //FIXME: why doesn't kapp->exit(1) do anything?
			return false;
		}
		return true;
	} else {
		QString finalMessage = message + ' ' +
		i18n( "\nWould you like to run the setup wizard again? "
		"Otherwise, the application will be closed." );

		int answer = KMessageBox::questionYesNo( this, finalMessage );

		if ( answer == KMessageBox::Yes )
			wizard( true );
		else {
			kError() << errormsg << ". Exiting" ;
			kapp->exit( 1 ); exit ( 1 ); //FIXME: why doesn't kapp->exit(1) do anything?
			return false;
		}

		return true;
	}
}

void KrecipesView::translate()
{
	button0->setTitle( i18n( "Find/Edit Recipes" ) );
	button1->setTitle( i18n( "Shopping List" ) );
	button2->setTitle( i18n( "Ingredients" ) );
	button3->setTitle( i18n( "Properties" ) );
	button4->setTitle( i18n( "Units" ) );
	button9->setTitle( i18n( "Preparation Methods" ) );
	button5->setTitle( i18n( "Categories" ) );
	button6->setTitle( i18n( "Authors" ) );
	button7->setTitle( i18n( "Diet Helper" ) );
	button8->setTitle( i18n( "Ingredient Matcher" ) );
}

void KrecipesView::printRequested()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == viewPanel ) {
		m_actionshandler.printRecipes( viewPanel->currentRecipes(), database );
	}
	else if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->recipePrint();
	}
	else if ( vis_panel == ingredientMatcherPanel ) {
		ingredientMatcherPanel->getActionsHandler()->recipePrint();
	}

}

void KrecipesView::cut()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == categoriesPanel)
		categoriesPanel->getActionsHandler()->cut();
}

void KrecipesView::paste()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == categoriesPanel)
		categoriesPanel->getActionsHandler()->paste();
}

void KrecipesView::pasteAsSubcategory()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == categoriesPanel)
		categoriesPanel->getActionsHandler()->pasteAsSub();
}

void KrecipesView::slotSetTitle( const QString& title )
{
	emit signalChangeCaption( title );
}

// Function to switch panels
void KrecipesView::slotSetPanel( KrePanel p, bool highlightLeftButton )
{
	if (p != -1) {
		if ( (m_activePanel != RecipeEdit) && (m_activePanel != RecipeView) )
			m_previousActivePanel = m_activePanel;
		m_activePanel = p;
	}
	kDebug() << "current:" << m_activePanel << "previous" << m_previousActivePanel;

	switch ( p ) {
	case SelectP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button0 );
		rightPanel->setHeader( i18n( "Find/Edit Recipes" ), "system-search" );
		rightPanel->raise( selectPanel );
		break;
	case ShoppingP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button1 );
		rightPanel->setHeader( i18n( "Shopping List" ), "view-pim-tasks" );
		rightPanel->raise( shoppingListPanel );
		shoppingListPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case DietP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button7 );
		rightPanel->setHeader( i18n( "Diet Helper" ), "diet" );
		rightPanel->raise( dietPanel );
		dietPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case MatcherP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button8 );
		rightPanel->setHeader( i18n( "Ingredient Matcher" ), "view-filter" );
		rightPanel->raise( ingredientMatcherPanel );
		ingredientMatcherPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;

	case IngredientsP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button2 );
		rightPanel->setHeader( i18n( "Ingredients" ), "ingredients" );
		rightPanel->raise( ingredientsPanel );
		ingredientsPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case PropertiesP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button3 );
		rightPanel->setHeader( i18n( "Properties" ), "properties" );
		rightPanel->raise( propertiesPanel );
		//propertiesPanel->reload();
		emit signalChangeStatusbar( QString("") );
		break;
	case UnitsP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button4 );
		rightPanel->setHeader( i18n( "Units" ), "units" );
		rightPanel->raise( unitsPanel );
		unitsPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case PrepMethodsP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button9 );
		rightPanel->setHeader( i18n( "Preparation Methods" ), "methods" );
		rightPanel->raise( prepMethodsPanel );
		prepMethodsPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case CategoriesP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button5 );
		rightPanel->setHeader( i18n( "Categories" ), "folder-yellow" );
		rightPanel->raise( categoriesPanel );
		categoriesPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case AuthorsP:
		if ( highlightLeftButton )
			leftPanel->highlightButton( button6 );
		rightPanel->setHeader( i18n( "Authors" ), "authors" );
		rightPanel->raise( authorsPanel );
		authorsPanel->reload( Load );
		emit signalChangeStatusbar( QString("") );
		break;
	case RecipeEdit:
		rightPanel->setHeader( i18n( "Edit Recipe" ), "document-edit" );
		rightPanel->raise( inputPanel );
		emit signalChangeStatusbar( QString("") );
		break;
	case RecipeView:
		rightPanel->setHeader( i18n( "View Recipe" ), "system-search" );
		rightPanel->raise( viewPanel );
		emit signalChangeStatusbar( QString("") );
		break;
	}
}

void KrecipesView::showCurrentRecipes()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->open();
	}
	else if (vis_panel == inputPanel ) {
		inputPanel->showRecipe();
	}
	else if ( vis_panel == ingredientMatcherPanel ) {
		ingredientMatcherPanel->getActionsHandler()->open();
	}
}

bool KrecipesView::save( void )
{
	return inputPanel->save();
}

void KrecipesView::enableSaveOptionSlot( bool enabled )
{
	recipeButton->setEnabled( enabled );

	if ( enabled )
		recipeButton->show();
	else 
		recipeButton->hide();

	emit enableSaveOption( enabled );
}

/*!
	\fn KrecipesView::exportRecipe()
 */
void KrecipesView::exportRecipe()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == viewPanel && viewPanel->recipesLoaded() > 0 ) {
		exportRecipes( viewPanel->currentRecipes() );
	}
	else if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->recipeExport();
	}
	else if ( vis_panel == ingredientMatcherPanel ) {
		ingredientMatcherPanel->getActionsHandler()->recipeExport();
	}
}

void KrecipesView::exportToClipboard()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == viewPanel && viewPanel->recipesLoaded() > 0 ) {
		QList<int> ids = viewPanel->currentRecipes();
		RecipeActionsHandler::recipesToClipboard( ids, database );
	}
	else if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->recipesToClipboard();
	}
}

void KrecipesView::addToShoppingList()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->addToShoppingList();
	}
}

void KrecipesView::categorizeCurrentRecipe()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->categorize();
	}
	else if (vis_panel == inputPanel ) {
		inputPanel->addCategory();
	}
}

void KrecipesView::removeFromCategory()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->removeFromCategory();
	}
}

void KrecipesView::createNewElement()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == ingredientsPanel ) {
		ingredientsPanel->getActionsHandler()->createNew();
	} else if ( vis_panel == propertiesPanel ) {
		propertiesPanel->getActionsHandler()->createNew();
	} else if ( vis_panel == unitsPanel ) {
		if ( unitsPanel->getActionsHandler() )
			unitsPanel->getActionsHandler()->createNew();
	} else if ( vis_panel == prepMethodsPanel ) {
		prepMethodsPanel->getActionsHandler()->createNew();
	} else if ( vis_panel == categoriesPanel) {
		categoriesPanel->getActionsHandler()->createNew();
	} else if ( vis_panel == authorsPanel ) {
		authorsPanel->getActionsHandler()->createNew();
	}	
}

void KrecipesView::renameCurrentElement()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == ingredientsPanel ) {
		ingredientsPanel->getActionsHandler()->rename();
	} else if ( vis_panel == propertiesPanel ) {
		propertiesPanel->getActionsHandler()->rename();
	} else if ( vis_panel == unitsPanel ) {
		if ( unitsPanel->getActionsHandler() )
			unitsPanel->getActionsHandler()->rename();
	} else if ( vis_panel == prepMethodsPanel ) {
		prepMethodsPanel->getActionsHandler()->rename();
	} else if ( vis_panel == categoriesPanel) {
		categoriesPanel->getActionsHandler()->rename();
	} else if ( vis_panel == authorsPanel ) {
		authorsPanel->getActionsHandler()->rename();
	}	
}

void KrecipesView::deleteCurrentElements()
{	
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->remove();
	} else if ( vis_panel == ingredientsPanel ) {
		ingredientsPanel->getActionsHandler()->remove();
	} else if ( vis_panel == propertiesPanel ) {
		propertiesPanel->getActionsHandler()->remove();
	} else if ( vis_panel == unitsPanel ) {
		if ( unitsPanel->getActionsHandler() )
			unitsPanel->getActionsHandler()->remove();
	} else if ( vis_panel == prepMethodsPanel ) {
		prepMethodsPanel->getActionsHandler()->remove();
	} else if ( vis_panel == categoriesPanel) {
		categoriesPanel->getActionsHandler()->remove();
	} else if ( vis_panel == authorsPanel ) {
		authorsPanel->getActionsHandler()->remove();
	}	
}

void KrecipesView::expandAll()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->expandAll();
	}
}

void KrecipesView::collapseAll()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->collapseAll();
	}
}

void KrecipesView::exportRecipes( const QList<int> &ids )
{
	if ( ids.count() == 1 )
		RecipeActionsHandler::exportRecipes( ids, i18n( "Export Recipe" ), database->recipeTitle( ids[ 0 ] ), database );
	else
		RecipeActionsHandler::exportRecipes( ids, i18n( "Export Recipe" ), i18n( "Recipes" ), database );
}

void KrecipesView::addSelectRecipeAction( KAction * action )
{
	selectPanel->addSelectRecipeAction( action );
}

void KrecipesView::addFindRecipeAction( KAction * action )
{
	selectPanel->addFindRecipeAction( action );
}

void KrecipesView::addCategoryAction( KAction * action )
{
	selectPanel->addCategoryAction( action );
}

void KrecipesView::addIngredientMatcherAction( KAction * action )
{
	ingredientMatcherPanel->addAction( action );
}

void KrecipesView::setCategorizeAction( KAction * action )
{
	selectPanel->setCategorizeAction( action );
}

void KrecipesView::setRemoveFromCategoryAction( KAction * action )
{
	selectPanel->setRemoveFromCategoryAction( action );
}

void KrecipesView::addElementAction( KAction * action )
{
	ingredientsPanel->addAction( action );
	propertiesPanel->addAction( action );
	unitsPanel->addAction( action );
	prepMethodsPanel->addAction( action );
	categoriesPanel->addAction( action );
	authorsPanel->addAction( action ); 
}

void KrecipesView::addCategoriesPanelAction( KAction * action )
{
	categoriesPanel->addAction( action );
}

void KrecipesView::setCategoryPasteAction( KAction * action )
{
	categoriesPanel->setCategoryPasteAction( action );
}

void KrecipesView::setPasteAsSubcategoryAction( KAction * action )
{
	categoriesPanel->setPasteAsSubcategoryAction( action );
}

void KrecipesView::actionRecipe( int recipeID, int action )
{
	//FIXME: Don't use magic numbers, use enums instead
	switch ( action ) {
	case 0:  //Show
		{
			showRecipe( recipeID );
			break;
		}
	case 1:  // Edit
		{
			if ( !inputPanel->everythingSaved() )
			{
				switch ( KMessageBox::questionYesNoCancel( this,
					i18n( "A recipe contains unsaved changes.\n"
					"Do you want to save changes made to this recipe before editing another recipe?" ),
					i18n( "Unsaved changes" ) ) ) {
				case KMessageBox::Yes:
					inputPanel->save();
					break;
				case KMessageBox::No:
					break;
				case KMessageBox::Cancel:
					return ;
				}
			}

			inputPanel->loadRecipe( recipeID );
			slotSetPanel( RecipeEdit );
			break;
		}
	case 2:  //Remove (not used at the moment)
		{
			switch ( KMessageBox::questionYesNo( this,
				i18n( "Are you sure you want to permanently remove the recipe, %1?"  ,database->recipeTitle(recipeID)),
				i18n( "Confirm remove" ) ) )
			{
			case KMessageBox::Yes:
				database->removeRecipe( recipeID );
				break;
			case KMessageBox::No:
				break;
			}
			QWidget * vis_panel = rightPanel->visiblePanel();
			if ( vis_panel == selectPanel )
				selectPanel->getActionsHandler()->selectionChangedSlot();
			break;
		}
	case 3:  //Add to shopping list
		{
			shoppingListPanel->addRecipeToShoppingList( recipeID );
			break;
		}
	case 4: //Show text in status bar
		{
			Recipe r;
 			database->loadRecipe(&r,RecipeDB::Meta|RecipeDB::Noatime,recipeID );

			KLocale *locale = KGlobal::locale();

			QString statusText = QString("<b>%1</b> %2 <b>%3</b> %4 <b>%5</b> %6")
			.arg(i18nc("Recipe created", "Created:")).arg(locale->formatDateTime(r.ctime))
			.arg(i18nc("Recipe modified", "Modified:")).arg(locale->formatDateTime(r.mtime))
			.arg(i18nc("Recipe last accessed", "Last Accessed:")).arg(locale->formatDateTime(r.atime));

			emit signalChangeStatusbar( statusText );
			break;
		}
	case 5: //Clear text in status bar
		{
			emit signalChangeStatusbar( QString("") );
			break;
		}
	}
}

void KrecipesView::actionRecipes( const QList<int> &ids, int action )
{
	if ( action == 0 ) { //show
		showRecipes( ids );
	} else if ( action == 2 ) {
		switch ( KMessageBox::questionYesNo( this,
		i18n( "Are you sure you want to permanently remove the selected recipes?" ),
		i18n( "Confirm remove" ) ) )
		{
		case KMessageBox::Yes:
			for ( QList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it ) {
				database->removeRecipe( *it );
			}
			break;
		case KMessageBox::No:
			break;
		}
	}
}


void KrecipesView::createNewRecipe( void )
{
	if ( !inputPanel->everythingSaved() ) {
		switch ( KMessageBox::questionYesNoCancel( this,
			i18n( "A recipe contains unsaved changes.\n"
			"Do you want to save changes made to this recipe before creating a new recipe?" ),
			i18n( "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			inputPanel->save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return ;
		}
	}

	inputPanel->newRecipe();
	slotSetPanel( RecipeEdit );
}

void KrecipesView::wizard( bool force )
{
	KConfigGroup config = KGlobal::config()->group( "Wizard" );
	bool setupDone = config.readEntry( "SystemSetup", false );

	QString setupVersion = config.readEntry( "Version", "0.3" );  // By default assume it's 0.3. This parameter didn't exist in that version yet.

	if ( !setupDone || ( setupVersion.toDouble() < 0.5 ) || force )  // The config structure changed in version 0.4 to have DBType and Config Structure version
	{

		bool setupUser, initData, doUSDAImport, adminEnabled;
		QString adminUser, adminPass, user, pass, host, client, dbName;
		int port;
		bool isRemote;

		QPointer<SetupAssistant> setupAssistant = new SetupAssistant( this );
		if ( setupAssistant->exec() == QDialog::Accepted )
		{
			config.sync();
			config = KGlobal::config()->group( "DBType" );
			dbType = config.readEntry( "Type", "SQLite" );

			kDebug() << "Setting up" ;
			setupAssistant->getOptions( setupUser, initData, doUSDAImport );
			kDebug()<<" setupUser :"<<setupUser<<" initData :"<<initData<<" doUSDAImport :"<<doUSDAImport;
			// Setup user if necessary
			if ( ( dbType == "MySQL" || dbType == "PostgreSQL" ) && setupUser )  // Don't setup user if checkbox of existing user... was set
			{
				kDebug() << "Setting up user";
				setupAssistant->getAdminInfo( adminEnabled, adminUser, adminPass, dbType );
				setupAssistant->getServerInfo( isRemote, host, client, dbName, user, pass, port );

				if ( !adminEnabled )  // Use root without password
				{
					kDebug() << "Using default admin";
					if ( dbType == "MySQL" )
						adminUser = "root";
					else if ( dbType == "PostgreSQL" )
						adminUser = "postgres";
					adminPass.clear();
				}
				if ( !isRemote )  // Use localhost
				{
					kDebug() << "Using localhost";
					host = "localhost";
					client = "localhost";
				}

				setupUserPermissions( host, client, dbName, user, pass, adminUser, adminPass, port );
			}

			// Initialize database with data if requested
			if ( initData ) {
				kDebug();
				setupAssistant->getServerInfo( isRemote, host, client, dbName, user, pass, port );
				initializeData( host, dbName, user, pass, port ); // Populate data as normal user
			}

			if ( doUSDAImport ) {
				kDebug()<<" import USDA";
				// Open the DB first
				setupAssistant->getServerInfo( isRemote, host, client, dbName, user, pass, port ); //only used if needed by backend
				kDebug()<<" dbName :"<<dbName<<" user :"<<user<<" pass :"<<pass<<" port :"<<port;
				RecipeDB *db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );
				kDebug()<<" database created :"<<db;
				// Import the data
				if ( db ) {
					kDebug()<<" try to connect";
					db->connect();

					if ( db->ok() ) {
						ProgressInterface pi(this);
						pi.listenOn(db);
						db->importUSDADatabase();
					}
					kDebug()<<" close";
					//close the database whether ok() or not
					delete db;
				}
			}

			//we can do a faster usda import if this is done after it
			if ( initData ) {
				kDebug()<<" initData :"<<initData;
				RecipeDB *db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );
				if ( db ) {
					kDebug()<<" dbName :"<<dbName<<" user :"<<user<<" pass :"<<pass<<" port :"<<port;
					db->connect();

					if ( db->ok() ) {
						kDebug()<<" import sample";
						db->importSamples();
					}

					//close the database whether ok() or not
					kDebug()<<" close db";
					delete db;
				}
			}

		}
		delete setupAssistant;
	}
}


void KrecipesView::setupUserPermissions( const QString &host, const QString &client, const QString &dbName, const QString &newUser, const QString &newPass, const QString &adminUser, const QString &adminPass, int port )
{
	QString user = adminUser;
	QString pass = adminPass;
	if ( user.isEmpty() ) {
		pass.clear();

		if ( dbType == "PostgreSQL" )
			user = "postgres";
		else if ( dbType == "MySQL" )
			user = "root";

		kDebug() << "Open db as " << user << ", with no password";
	}
	else
		kDebug() << "Open db as:" << user << ",*** with password ****";

	RecipeDB *db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );
	if ( db ) {
		db->connect(true,false);//create the database, but no tables (do that when connected as the user)
		if ( db->ok() )
			db->givePermissions( dbName, newUser, newPass, client ); // give permissions to the user
		else
			questionRerunWizard( db->err(), i18n( "Unable to setup database" ) );
	}

	delete db; //it closes the db automatically
}


void KrecipesView::initializeData( const QString &host, const QString &dbName, const QString &user, const QString &pass, int port )
{
	kDebug();
	RecipeDB * db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );
	if ( !db ) {
		kError() << "Code error. No DB support has been included. Exiting" ;
		kapp->exit( 1 );
	}
	kDebug()<<" connect it";
	db->connect();
	kDebug()<<" connected ok";
	if ( db->ok() ) {
		kDebug()<<" ok";
		db->emptyData();
		kDebug()<<" db empty data";
		db->initializeData();
		kDebug()<<" initializeData";
	}

	delete db;
}

void KrecipesView::addRecipeButton( QWidget *w, const QString &title )
{
	recipeWidget = w;

	QString short_title = title.left( 20 );
	if ( title.length() > 20 )
		short_title.append( "..." );

	recipeButton->setTitle( short_title );

	leftPanel->highlightButton( recipeButton );

	connect( recipeButton, SIGNAL( clicked() ), this, SLOT( switchToRecipe() ) );
	connect( ( RecipeInputDialog * ) w, SIGNAL( titleChanged( const QString& ) ), recipeButton, SLOT( setTitle( const QString& ) ) );

}

void KrecipesView::switchToRecipe( void )
{
	slotSetPanel( RecipeEdit );
}

void KrecipesView::closeRecipe( void )
{
	slotSetPanel( m_previousActivePanel, true );
}

//Needed to make sure that the raise() is done after the construction of all the widgets, otherwise childEvent in the PanelDeco is called only _after_ the raise(), and can't be shown.

void KrecipesView::show ( void )
{
	slotSetPanel( SelectP );
	QWidget::show();
}

void KrecipesView::showRecipe( int recipeID )
{
	QList<int> ids;
	ids << recipeID;
	showRecipes( ids );
}

void KrecipesView::showRecipes( const QList<int> &recipeIDs )
{
	if ( viewPanel->loadRecipes( recipeIDs ) )
		slotSetPanel( RecipeView );
}

void KrecipesView::activateContextHelp()
{
	switch ( m_activePanel ) {
	case RecipeView:
		KToolInvocation::invokeHelp();
		break;

	case SelectP:
		KToolInvocation::invokeHelp("find-edit");
		break;

	case ShoppingP:
		KToolInvocation::invokeHelp("shopping-list");
		break;

	case DietP:
		KToolInvocation::invokeHelp("diet-helper");
		break;

	case MatcherP:
		KToolInvocation::invokeHelp("ingredient-matcher");
		break;

	case RecipeEdit:
		KToolInvocation::invokeHelp("enter-edit-recipes");
		break;

	case IngredientsP:
		KToolInvocation::invokeHelp("ingredients-component");
		break;

	case PropertiesP:
		KToolInvocation::invokeHelp("properties-component");
		break;

	case UnitsP:
		KToolInvocation::invokeHelp("units-component");
		break;

	case PrepMethodsP:
		KToolInvocation::invokeHelp("prep-methods");
		break;

	case CategoriesP:
		KToolInvocation::invokeHelp("categories-component");
		break;

	case AuthorsP:
		KToolInvocation::invokeHelp("authors-component");
		break;
	}
}

void KrecipesView::panelRaised( QWidget *w, QWidget *old_w )
{
	emit panelShown( panelMap[ old_w ], false );
	emit panelShown( panelMap[ w ], true );
}


void KrecipesView::createShoppingListFromDiet( void )
{
	shoppingListPanel->createShopping( dietPanel->dietList() );
	slotSetPanel( ShoppingP );
}

void KrecipesView::moveTipButton( int, int )
{
	contextButton->setGeometry( leftPanel->width() - 42, leftPanel->height() - 42, 32, 32 );
}

void KrecipesView::resizeRightPane( int lpw, int )
{
	QSize rpsize = rightPanel->size();
	QPoint rpplace = rightPanel->pos();
	rpsize.setWidth( width() - lpw );
	rpplace.setX( lpw );
	rightPanel->move( rpplace );
	rightPanel->resize( rpsize );

}



void KrecipesView::initDatabase()
{

	KConfigGroup config = KGlobal::config()->group( "DBType" );
	dbType = checkCorrectDBType( config );



	// Open the database
	database = RecipeDB::createDatabase( dbType );
	if ( !database ) {
		// No DB type has been enabled(should not happen at all, but just in case)

		kError() << "Code error. No DB support was built in. Exiting" ;
		kapp->exit( 1 );
	}

	RecipeDB::Error connectError = database->connect();

	while ( !database->ok() ) {
		// Ask the user if he wants to rerun the wizard
		bool rerun = questionRerunWizard( database->err(),
			i18n( "Unable to open database" ), connectError);
		if ( !rerun )
			break;

		// Reread the configuration file.
		// The user may have changed the data and/or DB type

		KConfigGroup grp = KGlobal::config()->group( "DBType" );
		dbType = checkCorrectDBType( grp );

		delete database;
		database = RecipeDB::createDatabase( dbType );
		if ( database )
			connectError = database->connect();
		else {
			// No DB type has been enabled (should not happen at all, but just in case)

			kError() << "Code error. No DB support was built in. Exiting" ;
			kapp->exit( 1 );
			break;
		}
	}
	kDebug() << "DB started correctly" ;
}

QString KrecipesView::checkCorrectDBType( KConfigGroup &config )
{
	dbType = config.readEntry( "Type", "SQLite" );

	while ( ( dbType != "SQLite" ) && ( dbType != "MySQL" ) && ( dbType != "PostgreSQL" ) ) {
		questionRerunWizard( i18n( "The configured database type (%1) is unsupported." , dbType ), i18n( "Unsupported database type. Database must be either MySQL, SQLite, or PostgreSQL." ) );

		// Read the database setup again

		config = KGlobal::config()->group( "DBType" );
		config.sync();
		dbType = config.readEntry( "Type", "SQLite" );
	}
	return ( dbType );
}

void KrecipesView::reloadDisplay()
{
	viewPanel->reload();
}

void KrecipesView::editRecipe()
{
	KrePanel vis_panel = panelMap[ rightPanel->visiblePanel() ];

	switch ( vis_panel ) {
	case RecipeView:
		actionRecipe( viewPanel->currentRecipes() [ 0 ], 1 );
		break;
	case SelectP:
		selectPanel->getActionsHandler()->edit();
		break;
	case MatcherP:
		ingredientMatcherPanel->getActionsHandler()->edit();
	default:
		break;
	}
}

void KrecipesView::reload()
{
	viewPanel->reload();
	selectPanel->reload( ForceReload );
	shoppingListPanel->reload( ReloadIfPopulated );
	ingredientsPanel->reload( ReloadIfPopulated );
	propertiesPanel->reload();
	unitsPanel->reload( ReloadIfPopulated );
	dietPanel->reload( ReloadIfPopulated );
	authorsPanel->reload( ReloadIfPopulated );
	categoriesPanel->reload( ReloadIfPopulated );
	ingredientMatcherPanel->reload( ReloadIfPopulated );
	prepMethodsPanel->reload( ReloadIfPopulated );
}

QString KrecipesView::currentDatabase() const
{
	return "";
	// QDbus to be done
	//return DCOPRef(database);
}


#include "krecipesview.moc"
