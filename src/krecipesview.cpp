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

#include "krecipesview.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <qlayout.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpalette.h>

#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <krun.h>
#include <ktrader.h>
#include <kurl.h>
#include <kcursor.h>

#include "recipeactionshandler.h"
#include "setupwizard.h"
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
#include "gui/kstartuplogo.h"
#include "widgets/kremenu.h"
#include "widgets/paneldeco.h"

#if HAVE_MYSQL
#include "DBBackend/MySQL/mysqlrecipedb.h"
#endif

#if HAVE_SQLITE
#include "DBBackend/SQLite/literecipedb.h"
#endif



KrecipesView::KrecipesView(QWidget *parent)
    : QVBox(parent)
{
    

    // Init the setup wizard if necessary
    kdDebug()<<"Beginning wizard"<<endl;
    wizard();
    kdDebug()<<"Wizard finished correctly"<<endl;
    
    // Show Splash Screen

    KStartupLogo* start_logo = 0L;
    start_logo = new KStartupLogo();
    start_logo -> setHideEnabled( true );
    start_logo->show();
    start_logo->raise();

    // Initialize Database
    
    // Read the database setup

    KConfig *config; config=kapp->config(); config->sync(); 
    

    // Check if the database type is among those supported
    // and initialize the database in each case
    
   initDatabase(config);

    
    // Design the GUI
    splitter=new QHBox(this);

   // Create Left and Right Panels (splitter)


    KIconLoader il;
    leftPanel=new KreMenu(splitter,"leftPanel");
    rightPanel=new PanelDeco(splitter,"rightPanel",i18n("Find/Edit Recipes"),"filefind");
    leftPanel->setMinimumWidth(24);
    leftPanel->setMaximumWidth(200);
    leftPanel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding));
    //rightPanel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding));
    //rightPanel->setMaximumWidth(10000);

    // Design Left Panel
    
     
         // Buttons
    buttonsList = new QPtrList<KreMenuButton>();
    buttonsList->setAutoDelete( TRUE );

    button0=new KreMenuButton(leftPanel);
    button0->setIconSet(il.loadIconSet("filefind", KIcon::Panel, 32));
    buttonsList->append(button0);

    button1=new KreMenuButton(leftPanel);
    button1->setIconSet(il.loadIconSet( "trolley", KIcon::Panel,32 ));
    buttonsList->append(button1);
    
    button7=new KreMenuButton(leftPanel);
    button7->setIconSet(il.loadIconSet( "diet", KIcon::Panel,32));
    buttonsList->append(button7);
    
    button8=new KreMenuButton(leftPanel);
    button8->setIconSet(il.loadIconSet( "categories", KIcon::Panel,32));
    buttonsList->append(button8);
    
    
    // Submenus
    dataMenu=leftPanel->createSubMenu(i18n("Data"),"2rightarrow");
    
    button2=new KreMenuButton(leftPanel,dataMenu);
    button2->setIconSet(il.loadIconSet( "ingredients", KIcon::Panel,32 ));
    //buttonsList->append(button2);

    button3=new KreMenuButton(leftPanel,dataMenu);
    button3->setIconSet(il.loadIconSet( "properties", KIcon::Panel,32 ));
    buttonsList->append(button3);

    button4=new KreMenuButton(leftPanel,dataMenu);
    button4->setIconSet(il.loadIconSet( "units", KIcon::Panel,32 ));
    buttonsList->append(button4);
    
    button9=new KreMenuButton(leftPanel,dataMenu);
    button9->setIconSet(il.loadIconSet( "ICON PLEASE", KIcon::Panel,32 ));
    buttonsList->append(button9);

    button5=new KreMenuButton(leftPanel,dataMenu);
    button5->setIconSet(il.loadIconSet( "categories", KIcon::Panel,32 ));
    buttonsList->append(button5);

    button6=new KreMenuButton(leftPanel,dataMenu);
    button6->setIconSet(il.loadIconSet( "personal", KIcon::Panel,32 ));
    buttonsList->append(button6);

    contextButton = new QPushButton(leftPanel, "contextButton");
    contextButton->setIconSet(il.loadIconSet("krectip", KIcon::Panel, 32));
    contextButton->setGeometry(leftPanel->width()-42,leftPanel->height()-42,32,32);
    contextButton->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextButton->setFlat(true);

    contextHelp = new QWidget(leftPanel, "contextHelp");
    contextHelp->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextHelp->resize(leftPanel->size());
    contextHelp->hide();

    QGridLayout* contextLayout = new QGridLayout( contextHelp, 0, 0, 0, 0);
    contextTitle = new QLabel(contextHelp, "contextTitle");
    contextTitle->setTextFormat(Qt::RichText);
    contextLayout->addMultiCellWidget(contextTitle, 0, 0, 0, 2);
    contextClose = new QPushButton(contextHelp, "contextClose");
    contextClose->setFixedSize(QSize(16,16));
    contextClose->setIconSet(il.loadIconSet("fileclose", KIcon::Small, 16));
    contextClose->setPaletteBackgroundColor(QColor(238, 218, 156));

    contextLayout->addWidget(contextClose, 0, 2);
    contextText = new KTextBrowser(contextHelp, "contextText");
    contextText->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextText->setTextFormat(Qt::RichText);
    contextText->setReadOnly(true);
    contextText->setWordWrap(QTextEdit::WidgetWidth);
    contextLayout->addMultiCellWidget(contextText, 1, 4, 0, 2);

    // Right Panel Widgets
    inputPanel=new RecipeInputDialog(rightPanel,database);
    viewPanel=new RecipeViewDialog(rightPanel,database,-1);
    selectPanel=new SelectRecipeDialog(rightPanel,database);
    ingredientsPanel=new IngredientsDialog(rightPanel,database);
    propertiesPanel=new PropertiesDialog(rightPanel,database);
    unitsPanel=new UnitsDialog(rightPanel,database);
    shoppingListPanel=new ShoppingListDialog(rightPanel,database);
    dietPanel=new DietWizardDialog(rightPanel,database);
    categoriesPanel=new CategoriesEditorDialog(rightPanel,database);
    authorsPanel=new AuthorsDialog(rightPanel,database);
    prepMethodsPanel=new PrepMethodsDialog(rightPanel,database);
    ingredientMatcherPanel=new IngredientMatcherDialog(rightPanel,database);


    // i18n
    translate();

    // Initialize Variables
    recipeButton=0;



    // Connect Signals from Left Panel to slotSetPanel()
     connect( leftPanel, SIGNAL(clicked(int)),this, SLOT(slotSetPanel(int)) );

     connect( contextButton, SIGNAL(clicked()),contextHelp, SLOT(show()) );
     connect( contextButton, SIGNAL(clicked()),contextHelp, SLOT(raise()) );
     connect( contextClose, SIGNAL(clicked()),contextHelp, SLOT(close()) );
     connect( leftPanel, SIGNAL(clicked(int)),this, SLOT(setContextHelp(int)) );
     connect( leftPanel, SIGNAL(resized(int,int)),this, SLOT(resizeRightPane(int,int)));


    // Retransmit signal to parent to Enable/Disable the Save Button
    connect (inputPanel, SIGNAL(enableSaveOption(bool)), this, SIGNAL(enableSaveOption(bool)));

    // Create a new button when a recipe is unsaved
    connect (inputPanel, SIGNAL(createButton(QWidget*,QString)), this, SLOT(addRecipeButton(QWidget*,QString)));

    // Connect Signals from viewPanel (RecipeViewDialog)
    connect (viewPanel, SIGNAL(recipeSelected(int,int)),this, SLOT(actionRecipe(int,int)));

    // Connect Signals from selectPanel (SelectRecipeDialog)

    connect (selectPanel, SIGNAL(recipeSelected(int,int)),this, SLOT(actionRecipe(int,int)));
    connect (selectPanel, SIGNAL(recipesSelected(const QValueList<int>&,int)),this, SLOT(actionRecipes(const QValueList<int>&,int)));
    
    // Connect Signals from ingredientMatcherPanel (IngredientMatcherDialog)
    
    connect (ingredientMatcherPanel, SIGNAL(recipeSelected(int,int)),SLOT(actionRecipe(int,int)));

    // Close a recipe when requested (just switch panels)
    connect(inputPanel,SIGNAL(closeRecipe()),this,SLOT(closeRecipe()));

    // Show a recipe when requested (just switch panels)
    connect(inputPanel,SIGNAL(showRecipe(int)),this,SLOT(showRecipe(int)));

    // Create a new shopping list when a new diet is generated and accepted
    connect(dietPanel,SIGNAL(dietReady()),this,SLOT(createShoppingListFromDiet()));

    // Place the Tip Button in correct position when the left pane is resized
    connect(leftPanel,SIGNAL(resized(int,int)),this,SLOT(moveTipButton(int,int)));

    // Resize the Tip Viewer properly
    connect(leftPanel,SIGNAL(resized(int,int)),contextHelp,SLOT(resize(int,int)));



    // Close Splash Screen
    sleep(2);
    delete start_logo;
}

KrecipesView::~KrecipesView()
{
delete buttonsList;
delete viewPanel; //manually delete viewPanel because we need to be sure it is deleted
                  //before the database is because its destructor uses 'database'
delete database;
}

void KrecipesView::questionRerunWizard(const QString &message, const QString &error)
{
QString yesNoMessage=message+" "+i18n("\nWould you like to run the setup wizard again? Otherwise, the application will be closed.");
int answer=KMessageBox::questionYesNo(this,yesNoMessage);

	if (answer==KMessageBox::Yes) wizard(true);
	else
		{
		kdError()<<error<<". "<<i18n("Exiting")<<endl;
		exit(1);
		}
}

void KrecipesView::translate(){
  button0->setTitle(i18n("Find/Edit Recipes"));
  button1->setTitle(i18n("Shopping List"));
  button2->setTitle(i18n("Ingredients"));
  button3->setTitle(i18n("Properties"));
  button4->setTitle(i18n("Units"));
  button9->setTitle(i18n("Preparation Method"));
  button5->setTitle(i18n("Categories"));
  button6->setTitle(i18n("Authors"));
  button7->setTitle(i18n("Diet Helper"));
  button8->setTitle(i18n("Ingredient Matcher"));
}

void KrecipesView::print()
{
    // do the actual printing, here
    // p->drawText(etc..)

    viewPanel->print();
}


void KrecipesView::slotSetTitle(const QString& title)
{
    emit signalChangeCaption(title);
}

// Function to switch panels
void KrecipesView::slotSetPanel(int w)
{
if (leftPanel->currentMenu()==leftPanel->mainMenu())
	{
	switch (w)
		{
		case SelectP:
			rightPanel->setHeader(i18n("Find/Edit Recipes"),"filefind");
			rightPanel->raise(selectPanel);
			break;
		case ShoppingP:
			rightPanel->setHeader(i18n("Shopping List"),"trolley");
			rightPanel->raise(shoppingListPanel);
			break;
		case DietP:
			rightPanel->setHeader(i18n("Diet Helper"),"diet");
			rightPanel->raise(dietPanel);
			break;
		case MatcherP:
			rightPanel->setHeader(i18n("Ingredient Matcher"),"categories");
			rightPanel->raise(ingredientMatcherPanel);
			break;
		}
	}

else if (leftPanel->currentMenu()==dataMenu)
	{
	switch(w)
		{
		case IngredientsP:
			rightPanel->setHeader(i18n("Ingredients"),"ingredients");
			rightPanel->raise(ingredientsPanel);
			break;
		case PropertiesP:
			rightPanel->setHeader(i18n("Properties"),"properties");
			rightPanel->raise(propertiesPanel);
			break;
		case UnitsP:
			rightPanel->setHeader(i18n("Units"),"units");
			rightPanel->raise(unitsPanel);
			break;
		case PrepMethodsP:
			rightPanel->setHeader(i18n("Preparation Methods"),"GIVE ME AN ICON :p");
			rightPanel->raise(prepMethodsPanel);
			break;
		case CategoriesP:
			rightPanel->setHeader(i18n("Categories"),"categories");
			rightPanel->raise(categoriesPanel);
			break;
		case AuthorsP:
			rightPanel->setHeader(i18n("Authors"),"personal");
			rightPanel->raise(authorsPanel);
			break;
		case ContextHelp:
		break;
		}
	}
}

void KrecipesView::save(void)
{
inputPanel->save();
}

/*!
    \fn KrecipesView::exportRecipe()
 */
void KrecipesView::exportRecipe()
{/*
	if ( !inputPanel->everythingSaved() )
	{
		Recipe *recipe;  selectPanel->getCurrentRecipe( recipe );
		if ( recipe->recipeID == inputPanel->loadedRecipeID() )
		{
			switch( KMessageBox::questionYesNoCancel( this,
			  i18n("This recipe has unsaved changes.\n"
			  "In order to have these changes saved to a file, this recipe must first be saved to the database.\n"
			  "Do you want to save this recipe to a file with or without these changes?"),
			  i18n("Unsaved Changes") ) )
			{
			case KMessageBox::Yes: save();
			case KMessageBox::No: selectPanel->slotExportRecipe();
			case KMessageBox::Cancel: break;
			default: break;
			}
		}
	}
	else*/
	QWidget *vis_panel = rightPanel->visiblePanel();
	if ( vis_panel == viewPanel ) {
		if ( viewPanel->recipesLoaded() > 0 ) {
			if ( viewPanel->recipesLoaded() == 1 )
				RecipeActionsHandler::exportRecipes(viewPanel->currentRecipes(),i18n("Save Recipe"),database->recipeTitle(viewPanel->currentRecipes()[0]),database);
			else
				RecipeActionsHandler::exportRecipes(viewPanel->currentRecipes(),i18n("Save Recipe"),i18n("Recipes"),database);
		}
	}
	else if ( vis_panel == selectPanel ) {
		selectPanel->slotExportRecipe();
	}
}

void KrecipesView::actionRecipe(int recipeID, int action)
{
switch (action) {
case 0: //Show
{
	showRecipe(recipeID);
	break;
}
case 1: // Edit
{
	if ( !inputPanel->everythingSaved() )
	{
		switch( KMessageBox::questionYesNoCancel( this,
		  QString(i18n("Recipe \"%1\" contains unsaved changes.\n"
		  "Do you want to save changes made to this recipe before editing another recipe?")).arg(recipeButton->title()),
		   i18n("Unsaved changes") ) )
		{
			case KMessageBox::Yes: inputPanel->save(); break;
			case KMessageBox::No: break;
			case KMessageBox::Cancel: return;
		}
	}

	inputPanel->loadRecipe(recipeID);
	rightPanel->setHeader(i18n("Edit Recipe"),"edit");
	rightPanel->raise(inputPanel);
	setContextHelp(RecipeEdit);
	break;
}
case 2: //Remove
{
	switch( KMessageBox::questionYesNo( this,
	  QString(i18n("Are you sure you want to permanently remove the selected recipe?")),
	  i18n("Confirm remove") ) )
	{
		case KMessageBox::Yes:
			database->removeRecipe(recipeID);
			selectPanel->reload();
			break;
		case KMessageBox::No: break;
	}
	break;
}
case 3: //Add to shopping list
{
	shoppingListPanel->addRecipeToShoppingList(recipeID);
	break;
}
}
}

void KrecipesView::actionRecipes( const QValueList<int> &ids, int action )
{
	if ( action == 0 ) //show
	{
		showRecipes(ids);
	}
}


void KrecipesView::createNewRecipe(void)
{
if ( !inputPanel->everythingSaved() )
{
	switch( KMessageBox::questionYesNoCancel( this,
	  QString(i18n("Recipe \"%1\" contains unsaved changes.\n"
	  "Do you want to save changes made to this recipe before creating a new recipe?")).arg(recipeButton->title()),
	   i18n("Unsaved changes") ) )
	{
		case KMessageBox::Yes: inputPanel->save(); break;
		case KMessageBox::No: break;
		case KMessageBox::Cancel: return;
	}
}

inputPanel->newRecipe();
rightPanel->setHeader(i18n("Edit Recipe"),"edit");
rightPanel->raise(inputPanel);
setContextHelp(RecipeEdit);
}

void KrecipesView::createNewElement(void)
{
//this is inconstant as the program stands...
/*if (rightPanel->id(rightPanel->visiblePanel())==4) //Properties Panel is the active one
{
propertiesPanel->createNewProperty();
}
else*/{
  createNewRecipe();
  }
}

void KrecipesView::wizard(bool force)
{
KConfig *config=kapp->config();
config->setGroup("Wizard");
bool setupDone=config->readBoolEntry("SystemSetup",false);

QString setupVersion=config->readEntry("Version","0.3");  // By default assume it's 0.3. This parameter didn't exist in that version yet.

if (!setupDone || (setupVersion.toDouble()<0.5) || force) // The config structure changed in version 0.4 to have DBType and Config Structure version
{

bool setupUser,initData,doUSDAImport,adminEnabled; QString adminUser,adminPass,user,pass,host,client,dbName;
bool isRemote;

SetupWizard *setupWizard=new SetupWizard(this);
if(setupWizard->exec()== QDialog::Accepted)
{
KConfig *config; config=kapp->config(); config->sync(); config->setGroup("DBType");
dbType=config->readEntry("Type","SQLite");

kdDebug()<<"Setting up"<<endl;
setupWizard->getOptions(setupUser,initData,doUSDAImport);

// Setup user if necessary
#if HAVE_MYSQL
if (dbType=="MySQL" && setupUser) // Don't setup user if checkbox of existing user... was set
	{
	kdDebug()<<"Setting up user\n";
	setupWizard->getAdminInfo(adminEnabled,adminUser,adminPass);
	setupWizard->getServerInfo(isRemote,host,client,dbName,user,pass);

	if (!adminEnabled) // Use root without password
	{
	kdDebug()<<"Using default admin\n";
	adminUser="root";
	adminPass=QString::null;
	}
	if (!isRemote) // Use localhost
	{
	kdDebug()<<"Using localhost\n";
	host="localhost";
	client="localhost";
	}

	setupUserPermissions(host,client,dbName,user,pass,adminUser,adminPass);
	}
#endif //HAVE_MYSQL

#if HAVE_SQLITE
if (dbType=="SQLite")
	(void)LiteRecipeDB(QString::null,QString::null,QString::null,QString::null,true); //initialize database structure
#endif //HAVE_SQLITE

// Initialize database with data if requested
if (initData)
	{
	setupWizard->getServerInfo(isRemote,host,client,dbName,user,pass);
	initializeData(host,dbName,user,pass); // Populate data as normal user
	}
	
if (doUSDAImport)
	{
        // Open the DB first
	
	RecipeDB *db = 0;

	if ((dbType!="MySQL") && (dbType!="SQLite")) // Need it Just to have the else's properly. This should not happen anyway
	{
		kdError()<<i18n("Code error. Unrecognized database type. Exiting\n");
		exit(1);
	}
	#if HAVE_MYSQL
	else if (dbType=="MySQL")
	{
		setupWizard->getServerInfo(isRemote,host,client,dbName,user,pass);
		db = new MySQLRecipeDB(host,user,pass,dbName);
	}
	#endif //HAVE_MYSQL
	#if HAVE_SQLITE
	else if (dbType=="SQLite")
		db = new LiteRecipeDB(QString::null);
	#endif //HAVE_SQLITE
	
	// Import the data

	if (db)
	{
		if (db->ok())
		{
			KProgressDialog progress( this, "progress_dlg", i18n("Nutrient Import"), i18n("Importing USDA nutrient data"), true );
			db->importUSDADatabase( &progress );
		}
		
		//close the database whether ok() or not
		delete db;
	}
	}


}
delete setupWizard;
}
}


void KrecipesView::setupUserPermissions(const QString &host, const QString &client, const QString &dbName, const QString &newUser,const QString &newPass,const QString &adminUser,const QString &adminPass)
{
#if HAVE_MYSQL
if (dbType=="MySQL")
{
MySQLRecipeDB *db;

if (!adminPass.isNull())
	{ // Login as admin in the (remote) server and createDB if necessary
	kdDebug()<<"Open db as:"<< adminUser.latin1() <<",*** with password ****\n";
	db= new MySQLRecipeDB(host,adminUser,adminPass,dbName,true); // true means initialize db structure (It won't destroy data if exists)
	}
	else{ // Login as root with no password
	kdDebug()<<"Open db as root, with no password\n";
	db=new MySQLRecipeDB(host,"root",QString::null,dbName,true);
	}

db->givePermissions(dbName,newUser,newPass,client); // give permissions to the user

delete db; //it closes the db automatically
}
#endif // HAVE_MYSQL
}


void KrecipesView::initializeData(const QString &host,const QString &dbName, const QString &user,const QString &pass)
{
	if ((dbType!="MySQL")  && (dbType!="SQLite")) // Need it Just to have the else's properly. This should not happen anyway
	{
	kdError()<<i18n("Code error. Unrecognized database type. Exiting\n");
	exit(1);
	}

	#if HAVE_MYSQL
	else if (dbType=="MySQL")
	{
	MySQLRecipeDB *db;
	db= new MySQLRecipeDB(host,user,pass,dbName);
	db->emptyData();
	db->initializeData();
	delete db; //it closes the db automatically
	}
	#endif //HAVE_MYSQL
	
	#if HAVE_SQLITE
	else if(dbType=="SQLite")
	{
		LiteRecipeDB *db;
		db= new LiteRecipeDB(host,user,pass,dbName);
		db->emptyData();
		db->initializeData();
		delete db; //it closes the db automatically
	}
	#endif //HAVE_SQLITE
	else
	{
		kdError()<<i18n("Code error. No DB support has been included. Exiting\n");
		exit(1);
	}

}

void KrecipesView::addRecipeButton(QWidget *w,QString title)
{
recipeWidget=w;
KIconLoader il;
if (!recipeButton)
{
	recipeButton=new KreMenuButton(leftPanel,0,"recipeButton");
	recipeButton->setIconSet(il.loadIconSet("filesave",KIcon::Small));
	recipeButton->setTitle(title);
	if(contextHelp->isShown())
		{
		contextHelp->hide();
		}

	buttonsList->append(recipeButton);
	leftPanel->highlightButton(recipeButton);
	
	connect(recipeButton,SIGNAL(clicked()),this,SLOT(switchToRecipe()));
	connect((RecipeInputDialog *)w,SIGNAL(titleChanged(const QString&)),recipeButton,SLOT(setTitle(const QString&)));
}

}

void KrecipesView::switchToRecipe(void)
{
rightPanel->setHeader(i18n("View Recipe"),"filefind");
rightPanel->raise(recipeWidget);
setContextHelp(RecipeEdit);
}

void KrecipesView::closeRecipe(void)
{
selectPanel->reload();
rightPanel->setHeader(i18n("Find/Edit Recipes"),"filefind");
rightPanel->raise(selectPanel);
setContextHelp(SelectP);
buttonsList->removeLast(); recipeButton=0;
}

//Needed to make sure that the raise() is done after the construction of all the widgets, otherwise childEvent in the PanelDeco is called only _after_ the raise(), and can't be shown.

void KrecipesView::show (void)
{
	rightPanel->setHeader(i18n("Find/Edit Recipes"),"filefind");
	rightPanel->raise(this->selectPanel);
	setContextHelp(SelectP);
	QWidget::show();
}

void KrecipesView::showRecipe(int recipeID)
{
QValueList<int> ids;
ids << recipeID;
showRecipes( ids );
}

void KrecipesView::showRecipes( const QValueList<int> &recipeIDs)
{
if ( viewPanel->loadRecipes(recipeIDs) )
{
	rightPanel->setHeader(i18n("View Recipe"),"filefind");
	rightPanel->raise(viewPanel);
}
}

void KrecipesView::setContextHelp(int action){
if (leftPanel->currentMenu()==leftPanel->mainMenu())
	{
  	switch(action)
		{
		
		
		case SelectP:
		contextTitle->setText(i18n("<b>Recipes list</b>"));
		contextText->setText(i18n("<b>Search</b> for your favourite recipes easily: just type part of its name.<br><br>"
		"Set the <b>category filter</b> to use only the recipes in certain category: <i>desserts, chocolate, salads, vegetarian...</i>.<br><br>"
		"Right-click on a recipe to <b>save in Krecipes format</b> and <b>share your recipes</b> with your friends.<br><br>"
		"Oh, and do not forget you can search in <a href=\"http://www.google.com\">Google</a> for thousands of delicious recipes. Krecipes can import most famous formats on the net: <a href=\"http://www.formatdata.com/recipeml/\">RecipeML</a>, <a href=\"http://www.valu-soft.com/products/mastercook.html\">MasterCook</a>, and <a href=\"http://www.mealmaster.com/\">MealMaster</a>, apart from our excellent Krecipes format obviously.<br><br>"
		));
		break;
		
		case ShoppingP:
		contextTitle->setText(i18n("<b>Shopping list</b>"));
		contextText->setText(i18n("Need to do your shopping list? Just <b>add your recipes</b> for the week, and <b>press Ok</b>. Krecipes will generate a shopping list for you.<br><br>"
		"If you are willing to follow an adequate diet, or lazy enough to decide what to eat this week, just use the <b>Diet Helper</b> to autogenerate your diet, and then the shopping list.<br><br>"
		));
		break;
		
		case DietP:
		contextTitle->setText(i18n("<b>Diet Helper</b>"));
		contextText->setText(i18n("This dialog will help you in creating a diet for several weeks/days.<br><br>"
		"Choose how many days the diet will be for, how many meals per day you want, and how many dishes in each meal you want to have.<br><br>"
		"Oh, do not forget to specify the categories for your dishes, unless you want to have pizza for breakfast too....<br><br>"
		));
		break;
		
		}
	}
else if (leftPanel->currentMenu()==dataMenu)
	{
	
	switch(action)
			{
		case IngredientsP:
		contextTitle->setText(i18n("<b>Ingredients list</b>"));
		contextText->setText(i18n("Edit your ingredients: add/remove, double click to change their name, define the units used to measure them, and set their properties (<i>Energy, Fat, Calcium, Proteins...</i>)<br><br>"
		"Note that you can add more properties and units to the list from the <i>Properties List</i> menu"
		));
		break;
		
		case PropertiesP:
		contextTitle->setText(i18n("<b>Properties list</b>"));
			contextText->setText(i18n("What properties do you want to know from your recipes? <i>Fat, Energy, Vitamins, Cost,...</i>?<br><br>"
			"Add those here and later define the characteristics in the ingredients."
			));
		break;
		
		case UnitsP:
		contextTitle->setText(i18n("<b>Units list</b>"));
		contextText->setText(i18n("Double click to edit, or Add and Remove <b>new units</b> that you want to use to measure your ingredients. From a <i>gram</i>, to a <i>jar</i>, you can specify all kind of units you want. <br><br>Later, you can define in the <b>unit conversion table</b> how your units can be converted to others, so that Krecipes knows how to add up your ingredients when creating your shopping list, or calculate the properties of your recipes."
		));
		break;
		
		case PrepMethodsP:
		contextTitle->setText(i18n("<b>Preparation Methods list</b>"));
		contextText->setText(i18n("With the preparation method, you can give extra information about an ingredient. <i>sliced, cooked, optional,...</i> <br><br> Instead of adding this information to the ingredient itself, put this information here so that it is easier, for example, to create a shopping list or calculate nutrient information.<br><br>Just add and edit those here."));
		break;
		
		case CategoriesP:
		contextTitle->setText(i18n("<b>Categories list</b>"));
		contextText->setText(i18n("How do you want to classify your recipes? <i>Desserts, Main Course, Low Fat, Chocolate, Delicious, Vegetarian, ....</i> Just add and edit those here."
		));
		break;
		
		
		case AuthorsP:
		contextTitle->setText(i18n("<b>Authors list</b>"));
		contextText->setText(i18n("Keep track of the authors that created the recipes.<br><br>"
		"You can use this dialog to edit the details of the authors or add/remove them."
		));
		break;
		
		case RecipeEdit:
		contextTitle->setText(i18n("<b>Edit recipe</b>"));
		contextText->setText(i18n("Write your succulent recipes here. Set the title, authors and ingredients of your recipe, add a nice photo, and start typing. You can also use the <b>spellchecker</b> to correct your spelling mistakes.<br><br>"
		"If the <b>ingredient or unit</b> you are looking for is <b>missing</b>, do not worry. Just <b>type it</b>, and <b>new ones will be automatically created</b>. Remember to define the properties of your ingredients and fill in the units conversion table later.<br><br>"
		"Do you want your nice recipe to be included on the next release? Just save it in Krecipes format, and send it to us."
		));
		break;
		
		}
	}
}


void KrecipesView::createShoppingListFromDiet(void)
{
shoppingListPanel->createShopping(dietPanel->dietList());
rightPanel->setHeader(i18n("Shopping List"),"trolley");
rightPanel->raise(shoppingListPanel);
}

void KrecipesView::moveTipButton(int,int)
{
contextButton->setGeometry(leftPanel->width()-42,leftPanel->height()-42,32,32);
}

void KrecipesView::resizeRightPane(int lpw,int)
{
QSize rpsize=rightPanel->size();
QPoint rpplace=rightPanel->pos();
rpsize.setWidth(width()-lpw);
rpplace.setX(lpw);
rightPanel->move(rpplace);
rightPanel->resize(rpsize);

}



void KrecipesView::initDatabase(KConfig *config)
{   
	
	// Read the database type
	config->sync();
	config->setGroup("DBType");
   		dbType=checkCorrectDBType(config);
	
		
		
	// Open the database
	
	if (0) {}
	#if HAVE_MYSQL
	else if (dbType=="MySQL")
		{
		config->setGroup("Server");
		QString host=config->readEntry( "Host","localhost");
    		QString user=config->readEntry( "Username",QString::null);
    		QString pass=config->readEntry("Password",QString::null);
    		QString dbname=config->readEntry( "DBName", DEFAULT_DB_NAME);
		database=new MySQLRecipeDB(host,user,pass,dbname);
		}
			
	#endif // HAVE_MYSQL
	
	#if HAVE_SQLITE
	else if (dbType=="SQLite") database=new LiteRecipeDB(QString::null);
	#endif // HAVE_SQLITE
	else{	
		// No DB type has been enabled(should not happen at all, but just in case)
		
		kdError()<<i18n("Code error. No DB support was built in. Exiting")<<endl;
		exit(1);
	}
	
	while (!database->ok()) 
	{
		// Ask the user if he wants to rerun the wizard
		questionRerunWizard(database->err(), i18n("Unable to open database"));
		
		// Reread the configuration file.
		// The user may have changed the data and/or DB type
		
		config->sync();
		config->setGroup("DBType");
   		dbType=checkCorrectDBType(config);
		
		if (0) {}
		#if HAVE_MYSQL
		else if(dbType=="MySQL")  // First case, MySQL
			{
			config->setGroup("Server");
			QString host=config->readEntry( "Host","localhost");
    			QString user=config->readEntry( "Username",QString::null);
    			QString pass=config->readEntry("Password",QString::null);
    			QString dbname=config->readEntry( "DBName", DEFAULT_DB_NAME);
		
			kdDebug()<<i18n("Configured type... MySQL\n").latin1();
			
			// Try opening the database again with the new details
			delete database;
			database=new MySQLRecipeDB(host,user,pass,dbname);
			}
		#endif //HAVE_MYSQL
		
		#if HAVE_SQLITE
		else if (dbType=="SQLite")// The user chose SQLite this time
			{
			kdDebug()<<i18n("Configured type... SQLite\n").latin1();
			delete database;
			database=new LiteRecipeDB(QString::null); // server parameterss make no sense for SQLite
			}
		
		#endif //HAVE_SQLITE
		else{
		
		// No DB type has been enabled (should not happen at all, but just in case)
		
		kdError()<<i18n("Code error. No DB support was built in. Exiting")<<endl;
		exit(1);
		}
		
		
	}
	kdDebug()<<i18n("DB started correctly\n").latin1();
}

QString KrecipesView::checkCorrectDBType(KConfig *config)
{
	dbType=config->readEntry("Type","SQLite");
	
	while ((dbType!="SQLite") && (dbType!="MySQL"))
		{
		questionRerunWizard(i18n("The configured database type (%1) is unsupported.").arg(dbType),i18n("Unsupported database type. Database must be either MySQL or SQLite."));
			
		// Read the database setup again
		
		config=kapp->config(); config->sync(); config->setGroup("DBType");
		dbType=config->readEntry("Type","SQLite");
		}
	return (dbType);
}

#include "krecipesview.moc"

