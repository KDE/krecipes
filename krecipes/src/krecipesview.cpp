
/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "krecipesview.h"

#include <qlayout.h>
#include <qimage.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kglobal.h>

#include "recipeactionshandler.h"
#include "setupwizard.h"
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

KrecipesView::KrecipesView( QWidget *parent )
		: DCOPObject( "KrecipesInterface" ), Q3VBox( parent )
{
	#ifndef NDEBUG
	QTime dbg_total_timer; dbg_total_timer.start();
	#endif

	kapp->dcopClient()->setDefaultObject( objId() );

	// Init the setup wizard if necessary
	kDebug() << "Beginning wizard" << endl;
	wizard();
	kDebug() << "Wizard finished correctly" << endl;

	// Show Splash Screen

	KStartupLogo* start_logo = 0L;
	start_logo = new KStartupLogo();
	start_logo -> setHideEnabled( true );
	start_logo->show();
	start_logo->raise();

	// Initialize Database

	// Read the database setup

	KConfig *config = KGlobal::config();
	config->sync();


	// Check if the database type is among those supported
	// and initialize the database in each case
	START_TIMER("Initializing database")
	initDatabase( config );
	END_TIMER()


	// Design the GUI
	splitter = new Q3HBox( this );

	// Create Left and Right Panels (splitter)


	KIconLoader *il = KIconLoader::global();
	leftPanel = new KreMenu( splitter, "leftPanel" );
	rightPanel = new PanelDeco( splitter, "rightPanel", i18n( "Find/Edit Recipes" ), "system-search" );

	// Design Left Panel

	START_TIMER("Setting up buttons")
	// Buttons
	buttonsList = new Q3PtrList<KreMenuButton>();
	buttonsList->setAutoDelete( TRUE );

	button0 = new KreMenuButton( leftPanel, SelectP );
	button0->setIconSet( il->loadIconSet( "system-search", KIconLoader::Panel, 32 ) );
	buttonsList->append( button0 );

	button1 = new KreMenuButton( leftPanel, ShoppingP );
	button1->setIconSet( il->loadIconSet( "trolley", KIconLoader::Panel, 32 ) );
	buttonsList->append( button1 );

	button7 = new KreMenuButton( leftPanel, DietP );
	button7->setIconSet( il->loadIconSet( "diet", KIconLoader::Panel, 32 ) );
	buttonsList->append( button7 );

	button8 = new KreMenuButton( leftPanel, MatcherP );
	button8->setIconSet( il->loadIconSet( "categories", KIconLoader::Panel, 32 ) );
	buttonsList->append( button8 );


	// Submenus
	dataMenu = leftPanel->createSubMenu( i18n( "Data..." ), "data" );

	button2 = new KreMenuButton( leftPanel, IngredientsP, dataMenu );
	button2->setIconSet( il->loadIconSet( "ingredients", KIconLoader::Panel, 32 ) );
	//buttonsList->append(button2);

	button3 = new KreMenuButton( leftPanel, PropertiesP, dataMenu );
	button3->setIconSet( il->loadIconSet( "properties", KIconLoader::Panel, 32 ) );
	buttonsList->append( button3 );

	button4 = new KreMenuButton( leftPanel, UnitsP, dataMenu );
	button4->setIconSet( il->loadIconSet( "units", KIconLoader::Panel, 32 ) );
	buttonsList->append( button4 );

	button9 = new KreMenuButton( leftPanel, PrepMethodsP, dataMenu );
	button9->setIconSet( il->loadIconSet( "methods", KIconLoader::Panel, 32 ) );
	buttonsList->append( button9 );

	button5 = new KreMenuButton( leftPanel, CategoriesP, dataMenu );
	button5->setIconSet( il->loadIconSet( "categories", KIconLoader::Panel, 32 ) );
	buttonsList->append( button5 );

	button6 = new KreMenuButton( leftPanel, AuthorsP, dataMenu );
	button6->setIconSet( il->loadIconSet( "authors", KIconLoader::Panel, 32 ) );
	buttonsList->append( button6 );

	contextButton = new QPushButton( leftPanel, "contextButton" );
	contextButton->setIconSet( il->loadIconSet( "krectip", KIconLoader::Panel, 32 ) );
	contextButton->setGeometry( leftPanel->width() - 42, leftPanel->height() - 42, 32, 32 );
	contextButton->setPaletteBackgroundColor( contextButton->paletteBackgroundColor().light( 140 ) );
	contextButton->setFlat( true );
	END_TIMER()

	config->setGroup( "Performance" );
	int limit = config->readNumEntry( "CategoryLimit", -1 );
	database->updateCategoryCache(limit);

	// Right Panel Widgets
	START_TIMER("Creating input dialog")
	inputPanel = new RecipeInputDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating recipe view")
	viewPanel = new RecipeViewDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating recipe selection dialog")
	selectPanel = new SelectRecipeDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating ingredients component")
	ingredientsPanel = new IngredientsDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating properties component")
	propertiesPanel = new PropertiesDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating units component")
	unitsPanel = new UnitsDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating shopping list dialog")
	shoppingListPanel = new ShoppingListDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating diet wizard dialog")
	dietPanel = new DietWizardDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating categories component")
	categoriesPanel = new CategoriesEditorDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating authors component")
	authorsPanel = new AuthorsDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating prep methods component")
	prepMethodsPanel = new PrepMethodsDialog( rightPanel, database );
	END_TIMER()

	START_TIMER("Creating ingredients matcher dialog")
	ingredientMatcherPanel = new IngredientMatcherDialog( rightPanel, database );
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

	m_activePanel = RecipeEdit;

	// i18n
	translate();

	// Initialize Variables
	recipeButton = 0;



	// Connect Signals from Left Panel to slotSetPanel()
	connect( leftPanel, SIGNAL( clicked( KrePanel ) ), this, SLOT( slotSetPanel( KrePanel ) ) );

	connect( contextButton, SIGNAL( clicked() ), SLOT( activateContextHelp() ) );

	connect( leftPanel, SIGNAL( resized( int, int ) ), this, SLOT( resizeRightPane( int, int ) ) );


	// Retransmit signal to parent to Enable/Disable the Save Button
	connect ( inputPanel, SIGNAL( enableSaveOption( bool ) ), this, SIGNAL( enableSaveOption( bool ) ) );

	// Create a new button when a recipe is unsaved
	connect ( inputPanel, SIGNAL( createButton( QWidget*, const QString & ) ), this, SLOT( addRecipeButton( QWidget*, const QString & ) ) );

	// Connect Signals from selectPanel (SelectRecipeDialog)

	connect ( selectPanel, SIGNAL( recipeSelected( int, int ) ), this, SLOT( actionRecipe( int, int ) ) );
	connect ( selectPanel, SIGNAL( recipesSelected( const Q3ValueList<int>&, int ) ), this, SLOT( actionRecipes( const Q3ValueList<int>&, int ) ) );

	// Connect Signals from ingredientMatcherPanel (IngredientMatcherDialog)

	connect ( ingredientMatcherPanel, SIGNAL( recipeSelected( int, int ) ), SLOT( actionRecipe( int, int ) ) );

	// Close a recipe when requested (just switch panels)
	connect( inputPanel, SIGNAL( closeRecipe() ), this, SLOT( closeRecipe() ) );

	// Show a recipe when requested (just switch panels)
	connect( inputPanel, SIGNAL( showRecipe( int ) ), this, SLOT( showRecipe( int ) ) );

	// Create a new shopping list when a new diet is generated and accepted
	connect( dietPanel, SIGNAL( dietReady() ), this, SLOT( createShoppingListFromDiet() ) );

	// Place the Tip Button in correct position when the left pane is resized
	connect( leftPanel, SIGNAL( resized( int, int ) ), this, SLOT( moveTipButton( int, int ) ) );

	connect( rightPanel, SIGNAL( panelRaised( QWidget*, QWidget* ) ), SLOT( panelRaised( QWidget*, QWidget* ) ) );

	connect( selectPanel, SIGNAL( recipeSelected(bool) ), SIGNAL( recipeSelected(bool) ) );

	// Close Splash Screen
	delete start_logo;

	#ifndef NDEBUG
	kDebug()<<"Total time elapsed: "<<dbg_total_timer.elapsed()/1000<<" sec"<<endl;
	#endif
}

KrecipesView::~KrecipesView()
{
	delete buttonsList;
	delete viewPanel; //manually delete viewPanel because we need to be sure it is deleted
	//before the database is because its destructor uses 'database'
	delete database;
}

bool KrecipesView::questionRerunWizard( const QString &message, const QString &error )
{
	QString yesNoMessage = message + " " + i18n( "\nWould you like to run the setup wizard again? Otherwise, the application will be closed." );
	int answer = KMessageBox::questionYesNo( this, yesNoMessage );

	if ( answer == KMessageBox::Yes )
		wizard( true );
	else {
		kError() << error << ". " << i18n( "Exiting" ) << endl;
		kapp->exit( 1 ); exit ( 1 ); //FIXME: why doesn't kapp->exit(1) do anything?
		return false;
	}

	return true;
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

void KrecipesView::print()
{
	viewPanel->print();
}


void KrecipesView::slotSetTitle( const QString& title )
{
	emit signalChangeCaption( title );
}

// Function to switch panels
void KrecipesView::slotSetPanel( KrePanel p )
{
	m_activePanel = p;

	switch ( m_activePanel ) {
	case SelectP:
		rightPanel->setHeader( i18n( "Find/Edit Recipes" ), "system-search" );
		rightPanel->raise( selectPanel );
		break;
	case ShoppingP:
		rightPanel->setHeader( i18n( "Shopping List" ), "trolley" );
		rightPanel->raise( shoppingListPanel );
		shoppingListPanel->reload( Load );
		break;
	case DietP:
		rightPanel->setHeader( i18n( "Diet Helper" ), "diet" );
		rightPanel->raise( dietPanel );
		dietPanel->reload( Load );
		break;
	case MatcherP:
		rightPanel->setHeader( i18n( "Ingredient Matcher" ), "categories" );
		rightPanel->raise( ingredientMatcherPanel );
		ingredientMatcherPanel->reload( Load );
		break;

	case IngredientsP:
		rightPanel->setHeader( i18n( "Ingredients" ), "ingredients" );
		rightPanel->raise( ingredientsPanel );
		ingredientsPanel->reload( Load );
		break;
	case PropertiesP:
		rightPanel->setHeader( i18n( "Properties" ), "properties" );
		rightPanel->raise( propertiesPanel );
		//propertiesPanel->reload();
		break;
	case UnitsP:
		rightPanel->setHeader( i18n( "Units" ), "units" );
		rightPanel->raise( unitsPanel );
		unitsPanel->reload( Load );
		break;
	case PrepMethodsP:
		rightPanel->setHeader( i18n( "Preparation Methods" ), "methods" );
		rightPanel->raise( prepMethodsPanel );
		prepMethodsPanel->reload( Load );
		break;
	case CategoriesP:
		rightPanel->setHeader( i18n( "Categories" ), "categories" );
		rightPanel->raise( categoriesPanel );
		categoriesPanel->reload( Load );
		break;
	case AuthorsP:
		rightPanel->setHeader( i18n( "Authors" ), "authors" );
		rightPanel->raise( authorsPanel );
		authorsPanel->reload( Load );
		break;
	case RecipeEdit:
		rightPanel->setHeader( i18n( "Edit Recipe" ), "edit" );
		rightPanel->raise( inputPanel );
		break;
	case RecipeView:
		rightPanel->setHeader( i18n( "View Recipe" ), "system-search" );
		rightPanel->raise( viewPanel );
		break;
	}
}

bool KrecipesView::save( void )
{
	return inputPanel->save();
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
}

void KrecipesView::exportToClipboard()
{
	QWidget * vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == viewPanel && viewPanel->recipesLoaded() > 0 ) {
		Q3ValueList<int> ids = viewPanel->currentRecipes();
		RecipeActionsHandler::recipesToClipboard( ids, database );
	}
	else if ( vis_panel == selectPanel ) {
		selectPanel->getActionsHandler()->recipesToClipboard();
	}
}

void KrecipesView::exportRecipes( const Q3ValueList<int> &ids )
{
	if ( ids.count() == 1 )
		RecipeActionsHandler::exportRecipes( ids, i18n( "Export Recipe" ), database->recipeTitle( ids[ 0 ] ), database );
	else
		RecipeActionsHandler::exportRecipes( ids, i18n( "Export Recipe" ), i18n( "Recipes" ), database );
}

void KrecipesView::actionRecipe( int recipeID, int action )
{
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
				         QString( i18n( "A recipe contains unsaved changes.\n"
				                        "Do you want to save changes made to this recipe before editing another recipe?" ) ),
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
	case 2:  //Remove
		{
			switch ( KMessageBox::questionYesNo( this,
			                                     QString( i18n( "Are you sure you want to permanently remove the recipe, %1?" ) ).arg(database->recipeTitle(recipeID)),
			                                     i18n( "Confirm remove" ) ) )
			{
			case KMessageBox::Yes:
				database->removeRecipe( recipeID );
				break;
			case KMessageBox::No:
				break;
			}
			break;
		}
	case 3:  //Add to shopping list
		{
			shoppingListPanel->addRecipeToShoppingList( recipeID );
			break;
		}
	}
}

void KrecipesView::actionRecipes( const Q3ValueList<int> &ids, int action )
{
	if ( action == 0 )  //show
	{
		showRecipes( ids );
	}
}


void KrecipesView::createNewRecipe( void )
{
	if ( !inputPanel->everythingSaved() ) {
		switch ( KMessageBox::questionYesNoCancel( this,
		         QString( i18n( "A recipe contains unsaved changes.\n"
		                        "Do you want to save changes made to this recipe before creating a new recipe?" ) ),
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

void KrecipesView::createNewElement( void )
{
	//this is inconstant as the program stands...
	/*if (rightPanel->visiblePanel())==4) //Properties Panel is the active one
	{
	propertiesPanel->createNewProperty();
	}
	else*/{
		createNewRecipe();
	}
}

void KrecipesView::wizard( bool force )
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Wizard" );
	bool setupDone = config->readBoolEntry( "SystemSetup", false );

	QString setupVersion = config->readEntry( "Version", "0.3" );  // By default assume it's 0.3. This parameter didn't exist in that version yet.

	if ( !setupDone || ( setupVersion.toDouble() < 0.5 ) || force )  // The config structure changed in version 0.4 to have DBType and Config Structure version
	{

		bool setupUser, initData, doUSDAImport, adminEnabled;
		QString adminUser, adminPass, user, pass, host, client, dbName;
		int port;
		bool isRemote;

		SetupWizard *setupWizard = new SetupWizard( this );
		if ( setupWizard->exec() == QDialog::Accepted )
		{
			config->sync();
			config->setGroup( "DBType" );
			dbType = config->readEntry( "Type", "SQLite" );

			kDebug() << "Setting up" << endl;
			setupWizard->getOptions( setupUser, initData, doUSDAImport );

			// Setup user if necessary
			if ( ( dbType == "MySQL" || dbType == "PostgreSQL" ) && setupUser )  // Don't setup user if checkbox of existing user... was set
			{
				kDebug() << "Setting up user\n";
				setupWizard->getAdminInfo( adminEnabled, adminUser, adminPass, dbType );
				setupWizard->getServerInfo( isRemote, host, client, dbName, user, pass, port );

				if ( !adminEnabled )  // Use root without password
				{
					kDebug() << "Using default admin\n";
					if ( dbType == "MySQL" )
						adminUser = "root";
					else if ( dbType == "PostgreSQL" )
						adminUser = "postgres";
					adminPass = QString::null;
				}
				if ( !isRemote )  // Use localhost
				{
					kDebug() << "Using localhost\n";
					host = "localhost";
					client = "localhost";
				}

				setupUserPermissions( host, client, dbName, user, pass, adminUser, adminPass, port );
			}

			// Initialize database with data if requested
			if ( initData ) {
				setupWizard->getServerInfo( isRemote, host, client, dbName, user, pass, port );
				initializeData( host, dbName, user, pass, port ); // Populate data as normal user
			}

			if ( doUSDAImport ) {
				// Open the DB first
				setupWizard->getServerInfo( isRemote, host, client, dbName, user, pass, port ); //only used if needed by backend
				RecipeDB *db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );

				// Import the data
				if ( db ) {
					db->connect();

					if ( db->ok() ) {
						ProgressInterface pi(this);
						pi.listenOn(db);
						db->importUSDADatabase();
					}

					//close the database whether ok() or not
					delete db;
				}
			}

			//we can do a faster usda import if this is done after it
			if ( initData ) {
				RecipeDB *db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );
				if ( db ) {
					db->connect();

					if ( db->ok() ) {
						db->importSamples();
					}

					//close the database whether ok() or not
					delete db;
				}
			}

		}
		delete setupWizard;
	}
}


void KrecipesView::setupUserPermissions( const QString &host, const QString &client, const QString &dbName, const QString &newUser, const QString &newPass, const QString &adminUser, const QString &adminPass, int port )
{
	QString user = adminUser;
	QString pass = adminPass;
	if ( user.isNull() ) {
		pass = QString::null;

		if ( dbType == "PostgreSQL" )
			user = "postgres";
		else if ( dbType == "MySQL" )
			user = "root";

		kDebug() << "Open db as " << user << ", with no password\n";
	}
	else
		kDebug() << "Open db as:" << user << ",*** with password ****\n";

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
	RecipeDB * db = RecipeDB::createDatabase( dbType, host, user, pass, dbName, port, dbName );
	if ( !db ) {
		kError() << i18n( "Code error. No DB support has been included. Exiting" ) << endl;
		kapp->exit( 1 );
	}

	db->connect();

	if ( db->ok() ) {
		db->emptyData();
		db->initializeData();
	}

	delete db;
}

void KrecipesView::addRecipeButton( QWidget *w, const QString &title )
{
	recipeWidget = w;
	KIconLoader *il = KIconLoader::global();
	if ( !recipeButton ) {
		recipeButton = new KreMenuButton( leftPanel, RecipeEdit );

		recipeButton->setIconSet( il->loadIconSet( "document-save", KIcon::Small ) );

		QString short_title = title.left( 20 );
		if ( title.length() > 20 )
			short_title.append( "..." );

		recipeButton->setTitle( short_title );

		buttonsList->append( recipeButton );
		leftPanel->highlightButton( recipeButton );

		connect( recipeButton, SIGNAL( clicked() ), this, SLOT( switchToRecipe() ) );
		connect( ( RecipeInputDialog * ) w, SIGNAL( titleChanged( const QString& ) ), recipeButton, SLOT( setTitle( const QString& ) ) );
	}

}

void KrecipesView::switchToRecipe( void )
{
	slotSetPanel( RecipeEdit );
}

void KrecipesView::closeRecipe( void )
{
	slotSetPanel( SelectP );
	buttonsList->removeLast();
	recipeButton = 0;
}

//Needed to make sure that the raise() is done after the construction of all the widgets, otherwise childEvent in the PanelDeco is called only _after_ the raise(), and can't be shown.

void KrecipesView::show ( void )
{
	slotSetPanel( SelectP );
	QWidget::show();
}

void KrecipesView::showRecipe( int recipeID )
{
	Q3ValueList<int> ids;
	ids << recipeID;
	showRecipes( ids );
}

void KrecipesView::showRecipes( const Q3ValueList<int> &recipeIDs )
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



void KrecipesView::initDatabase( KConfig *config )
{

	// Read the database type
	config->sync();
	config->setGroup( "DBType" );
	dbType = checkCorrectDBType( config );



	// Open the database
	database = RecipeDB::createDatabase( dbType );
	if ( !database ) {
		// No DB type has been enabled(should not happen at all, but just in case)

		kError() << i18n( "Code error. No DB support was built in. Exiting" ) << endl;
		kapp->exit( 1 );
	}

	database->connect();

	while ( !database->ok() ) {
		// Ask the user if he wants to rerun the wizard
		bool rerun = questionRerunWizard( database->err(), i18n( "Unable to open database" ) );
		if ( !rerun )
			break;

		// Reread the configuration file.
		// The user may have changed the data and/or DB type

		config->sync();
		config->setGroup( "DBType" );
		dbType = checkCorrectDBType( config );

		delete database;
		database = RecipeDB::createDatabase( dbType );
		if ( database )
			database->connect();
		else {
			// No DB type has been enabled (should not happen at all, but just in case)

			kError() << i18n( "Code error. No DB support was built in. Exiting" ) << endl;
			kapp->exit( 1 );
			break;
		}
	}
	kDebug() << i18n( "DB started correctly\n" ).toLatin1();
}

QString KrecipesView::checkCorrectDBType( KConfig *config )
{
	dbType = config->readEntry( "Type", "SQLite" );

	while ( ( dbType != "SQLite" ) && ( dbType != "MySQL" ) && ( dbType != "PostgreSQL" ) ) {
		questionRerunWizard( i18n( "The configured database type (%1) is unsupported." ).arg( dbType ), i18n( "Unsupported database type. Database must be either MySQL, SQLite, or PostgreSQL." ) );

		// Read the database setup again

		config = KGlobal::config();
		config->sync();
		config->setGroup( "DBType" );
		dbType = config->readEntry( "Type", "SQLite" );
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

DCOPRef KrecipesView::currentDatabase() const
{
	return DCOPRef(database);
}


#include "krecipesview.moc"
