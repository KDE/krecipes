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
#include <krun.h>
#include <ktrader.h>
#include <kurl.h>

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
#include "gui/kstartuplogo.h"
#include "widgets/kremenu.h"
#include "widgets/paneldeco.h"

#if HAVE_MYSQL
#include "DBBackend/mysqlrecipedb.h"
#endif

#if HAVE_SQLITE
#include "DBBackend/literecipedb.h"
#endif



KrecipesView::KrecipesView(QWidget *parent)
    : QVBox(parent)
{
    // Initialize Database

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


    // Read the database setup

    KConfig *config; config=kapp->config(); config->sync(); config->setGroup("DBType");
    dbtype=config->readEntry("Type","SQLite");

    // Check if the database type is among those supported

    if ((dbtype!="SQLite") && (dbtype!="MySQL"))
    {
    kdDebug()<<"Unsupported database type.  Database must be either MySQL or SQLite. Exiting"<<endl;
    exit(1);
    }
    #if HAVE_MYSQL

    else if(dbtype=="MySQL")  // First case, MySQL
    	{
	// Read the server parameters
	config->setGroup("Server");
	QString host=config->readEntry( "Host","localhost");
    	QString user=config->readEntry( "Username",QString::null);
    	QString pass=config->readEntry("Password",QString::null);
    	QString dbname=config->readEntry( "DBName", DEFAULT_DB_NAME);

	// Open the database

	database=new MySQLRecipeDB(host,user,pass,dbname);

	}

    #endif //HAVE_MYSQL


    #if HAVE_SQLITE

    else if (dbtype=="SQLite")// SQLite case
    	{
    	database=new LiteRecipeDB(QString::null); // server parameterss make no sense for SQLite
	}

    #endif //HAVE_SQLITE
    else{
    kdDebug()<<"Unsupported database type.  Database must be either MySQL or SQLite. Exiting"<<endl;
    exit(1);
    }

    splitter=new QHBox(this);




// Create Left and Right Panels (splitter)


    KIconLoader il;
    leftPanel=new KreMenu(splitter,"leftPanel");
    rightPanel=new PanelDeco(splitter,"rightPanel",i18n("Find/Edit Recipes"),"filefind");
    leftPanel->setMinimumWidth(22);
    leftPanel->setMaximumWidth(200);
    leftPanel->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding));
    //rightPanel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding));
    //rightPanel->setMaximumWidth(10000);

    // Design Left Panel
    buttonsList = new QPtrList<KreMenuButton>();
    buttonsList->setAutoDelete( TRUE );

    button0=new KreMenuButton(leftPanel);
    button0->setIconSet(il.loadIconSet("filefind", KIcon::Small));
    buttonsList->append(button0);

    button1=new KreMenuButton(leftPanel);
    button1->setIconSet(il.loadIconSet( "trolley", KIcon::Small ));
    buttonsList->append(button1);

    button2=new KreMenuButton(leftPanel);
    button2->setIconSet(il.loadIconSet( "ingredients", KIcon::Small ));
    //buttonsList->append(button2);

    button3=new KreMenuButton(leftPanel);
    button3->setIconSet(il.loadIconSet( "properties", KIcon::Small ));
    buttonsList->append(button3);

    button4=new KreMenuButton(leftPanel);
    button4->setIconSet(il.loadIconSet( "units", KIcon::Small ));
    buttonsList->append(button4);

    button5=new KreMenuButton(leftPanel);
    button5->setIconSet(il.loadIconSet( "categories", KIcon::Small ));
    buttonsList->append(button5);

    button6=new KreMenuButton(leftPanel);
    button6->setIconSet(il.loadIconSet( "personal", KIcon::Small ));
    buttonsList->append(button6);

    contextButton = new QPushButton(leftPanel, "contextButton");
    contextButton->setIconSet(il.loadIconSet("krectip", KIcon::Small, 32));
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
    viewPanel=new RecipeViewDialog(rightPanel,database,1);
    selectPanel=new SelectRecipeDialog(rightPanel,database);
    ingredientsPanel=new IngredientsDialog(rightPanel,database);
    propertiesPanel=new PropertiesDialog(rightPanel,database);
    unitsPanel=new UnitsDialog(rightPanel,database);
    shoppingListPanel=new ShoppingListDialog(rightPanel,database);
    dietPanel=new DietWizardDialog(rightPanel,database);
    categoriesPanel=new CategoriesEditorDialog(rightPanel,database);
    authorsPanel=new AuthorsDialog(rightPanel,database);



    // i18n
    translate();

    // Initialize Variables
    recipeButton=0;



    // Connect Signals from Left Panel to slotSetPanel()
     connect( leftPanel, SIGNAL(clicked(int)),this, SLOT(slotSetPanel(int)) );
     connect(shoppingListPanel,SIGNAL(wizardClicked()),this,SLOT(slotSetDietWizardPanel()));

     connect( contextButton, SIGNAL(clicked()),contextHelp, SLOT(show()) );
     connect( contextClose, SIGNAL(clicked()),contextHelp, SLOT(close()) );
     connect( leftPanel, SIGNAL(clicked(int)),this, SLOT(setContextHelp(int)) );
     connect( leftPanel, SIGNAL(resized(int,int)),this, SLOT(resizeRightPane(int,int)));


    // Retransmit signal to parent to Enable/Disable the Save Button
    connect (inputPanel, SIGNAL(enableSaveOption(bool)), this, SIGNAL(enableSaveOption(bool)));

    // Create a new button when a recipe is unsaved
    connect (inputPanel, SIGNAL(createButton(QWidget*,QString)), this, SLOT(addRecipeButton(QWidget*,QString)));

    // Connect Signals from selectPanel (SelectRecipeDialog)

    connect (selectPanel, SIGNAL(recipeSelected(int,int)),this, SLOT(actionRecipe(int,int)));

    // Close a recipe when requested (just switch panels)
    connect(inputPanel,SIGNAL(closeRecipe()),this,SLOT(closeRecipe()));

    // Show a recipe when requested (just switch panels)
    connect(inputPanel,SIGNAL(showRecipe(int)),this,SLOT(showRecipe(int)));

    // Add recipe to the shopping list when requested
    connect(inputPanel,SIGNAL(addRecipeToShoppingList(int)),shoppingListPanel,SLOT(addRecipeToShoppingList(int)));

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
}

void KrecipesView::translate(){
  button0->setTitle(i18n("Find/Edit Recipes"));
  button1->setTitle(i18n("Shopping List"));
  button2->setTitle(i18n("Ingredients"));
  button3->setTitle(i18n("Properties"));
  button4->setTitle(i18n("Units"));
  button5->setTitle(i18n("Categories"));
  button6->setTitle(i18n("Authors"));
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
switch (w)
{
case SelectP: selectPanel->reload(); // Reload data
	rightPanel->setHeader(i18n("Find/Edit Recipes"),"filefind");
	rightPanel->raise(selectPanel);
	break;
case ShoppingP: shoppingListPanel->reload(); // Reload data
	rightPanel->setHeader(i18n("Shopping List"),"trolley");
	rightPanel->raise(shoppingListPanel);
	break;
case IngredientsP: ingredientsPanel->reload();// Reload data
	rightPanel->setHeader(i18n("Ingredients"),"ingredients");
	rightPanel->raise(ingredientsPanel);
	break;
case PropertiesP: propertiesPanel->reload();
	rightPanel->setHeader(i18n("Properties"),"properties");
	rightPanel->raise(propertiesPanel);

	break;
case UnitsP: unitsPanel->reload(); // Reload data
	rightPanel->setHeader(i18n("Units"),"units");
	rightPanel->raise(unitsPanel);
	break;
case CategoriesP:
	categoriesPanel->reload();
	rightPanel->setHeader(i18n("Categories"),"categories");
	rightPanel->raise(categoriesPanel);
	break;
case DietWizardP: dietPanel->reload();
	rightPanel->setHeader(i18n("Diet Wizard"),"diet");
	rightPanel->raise(dietPanel);
	break;
case AuthorsP: authorsPanel->reload();
	rightPanel->setHeader(i18n("Authors"),"personal");
	rightPanel->raise(authorsPanel);
	break;
case ContextHelp:
  break;
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
		selectPanel->slotExportRecipe();
}

void KrecipesView::actionRecipe(int recipeID, int action)
{
if (action==0) //Show
	{
	showRecipe(recipeID);
	}
else if (action==1) // Edit
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
}
else if (action==2) //Remove
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
if (rightPanel->id(rightPanel->visiblePanel())==4) //Properties Panel is the active one
{
propertiesPanel->createNewProperty();
}
else{
  createNewRecipe();
  }
}

void KrecipesView::wizard(void)
{
KConfig *config=kapp->config();
config->setGroup("Wizard");
bool setupDone=config->readBoolEntry("SystemSetup",false);

QString setupVersion=config->readEntry("Version","0.3");  // By default assume it's 0.3. This parameter didn't exist in that version yet.

if (!setupDone || (setupVersion.toDouble()<0.4)) // The config structure changed in version 0.4 to have DBType and Config Structure version
{

bool setupUser,initData,adminEnabled; QString adminUser,adminPass,user,pass,host,client,dbName;
bool isRemote;

SetupWizard *setupWizard=new SetupWizard(this);
if(setupWizard->exec()== QDialog::Accepted)
{
KConfig *config; config=kapp->config(); config->sync(); config->setGroup("DBType");
dbtype=config->readEntry("Type","SQLite");

kdDebug()<<"Setting up"<<endl;
setupWizard->getOptions(setupUser,initData);

// Setup user if necessary

if (setupUser) // Don't setup user if checkbox of existing user... was set
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

// Initialize database with data if requested

if (initData)
	{
	setupWizard->getServerInfo(isRemote,host,client,dbName,user,pass);
	initializeData(host,dbName,user,pass); // Populate data as normal user
	}

}
delete setupWizard;
}
}


void KrecipesView::slotSetDietWizardPanel(void)
{
dietPanel->reload();
rightPanel->setHeader(i18n("Diet Wizard"),"wizard");
rightPanel->raise(dietPanel);
    setContextHelp(DietWizardP);
}


void KrecipesView::setupUserPermissions(const QString &host, const QString &client, const QString &dbName, const QString &newUser,const QString &newPass,const QString &adminUser,const QString &adminPass)
{
#if HAVE_MYSQL
if (dbtype=="MySQL")
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
if ((dbtype!="MySQL")  && (dbtype!="SQLite")) // Need it Just to have the else's properly
{
kdDebug()<<"Unrecognized database type. Exiting\n";
exit(1);
}

#if HAVE_MYSQL
else if (dbtype=="MySQL")
{
MySQLRecipeDB *db;
db= new MySQLRecipeDB(host,user,pass,dbName);
db->emptyData();
db->initializeData();
delete db; //it closes the db automatically
}
#endif //HAVE_MYSQL

#if HAVE_SQLITE
else if(dbtype=="SQLite")
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
kdDebug()<<"Unsupported database type. Exiting\n";
exit(1);
}

}

void KrecipesView::addRecipeButton(QWidget *w,QString title)
{
recipeWidget=w;
KIconLoader il;
if (!recipeButton)
{
	recipeButton=new KreMenuButton(leftPanel,"recipeButton");
	recipeButton->setIconSet(il.loadIconSet("filesave",KIcon::Small));
	recipeButton->setTitle(title);
  if(contextHelp->isShown()){
    contextHelp->hide();
  }

  buttonsList->append(recipeButton);
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
rightPanel->setHeader(i18n("Find/Edit Recipe"),"filefind");
rightPanel->raise(selectPanel);
buttonsList->removeLast();
setContextHelp(SelectP);
recipeButton=0;
}

//Needed to make sure that the raise() is done after the construction of all the widgets, otherwise childEvent in the PanelDeco is called only _after_ the raise(), and can't be shown.

void KrecipesView::show (void)
{
	rightPanel->setHeader(i18n("Find/Edit Recipe"),"filefind");
	rightPanel->raise(this->selectPanel);
	setContextHelp(SelectP);
	QWidget::show();
}

void KrecipesView::showRecipe(int recipeID)
{
viewPanel->loadRecipe(recipeID);
rightPanel->setHeader(i18n("View Recipe"),"filefind");
rightPanel->raise(viewPanel);
}

void KrecipesView::setContextHelp(int action){
  switch(action){
    case SelectP:
      contextTitle->setText(i18n("<b>Recipes list</b>"));
      contextText->setText(i18n("<b>Search</b> for your favourite recipes easily! Just type part of its name.<br><br>"
      "Set the <b>category filter</b> to use only the recipes in certain category: <i>desserts, chocolate, salads, vegetarian...</i><br><br>"
      "Right click on a recipe to <b>save in Krecipes format</b> and <b>share your recipes</b> with your friends <br><br>"
      "Oh and do not forget you can search in <a href=\"http://www.google.com\">Google</a> for thousands of delicious recipes. Krecipes can import most famous formats on the net: <a href=\"http://www.formatdata.com/recipeml/\">RecipeML</a>, <a href=\"http://www.valu-soft.com/products/mastercook.html\">MasterCook</a> and <a href=\"http://www.mealmaster.com/\">MealMaster</a>, appart from our excellent Krecipes format obviously.<br><br>"
      ));
    break;
    case ShoppingP:
      contextTitle->setText(i18n("<b>Shopping list</b>"));
      contextText->setText(i18n("Need to do your shopping list? Just <b>add your recipes</b> for the week, and <b>press Ok</b>. Krecipes will generate a shopping list for you.<br><br>"
      "If you are willing to follow an adequate diet, or lazy enough to decide what to eat this week, just use the <b>Diet Wizard</b> to autogenerate your diet, and then the shopping list.<br><br>"
      ));
    break;
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
  case CategoriesP:
      contextTitle->setText(i18n("<b>Categories list</b>"));
      contextText->setText(i18n("How do you want to classify your recipes? <i>Desserts, Main Course, Low Fat, Chocolate, Delicious, Vegetarian, ....</i> Just add and edit those here."
      ));
    break;
  case DietWizardP:
      contextTitle->setText(i18n("<b>Diet Wizard</b>"));
      contextText->setText(i18n("This dialog will help you creating a diet for several weeks/days.<br><br>"
      "Choose how many days the diet will be, how many meals per day you want, and how many dishes in each meal you want to have.<br><br>"
      "Oh, do not forget to specify the categories for your dishes, unless you want to have pizza for breakfast too :) <br><br>"
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
      contextText->setText(i18n("Write your succulent recipes here. Set the title, authors and ingredients of your recipe, add a nice photo, and start typing. You can also use the <b>spellchecker</b> to correct your spelling mistakes!<br><br>"
      "If the <b>ingredient or unit</b> you are looking for is <b>missing</b>, do not worry. Just <b>type it</b>, and <b>new ones will be automatically created</b>. Remember to define the properties of your ingredients and fill in the units conversion table later.<br><br>"
      "Do you want your nice recipe to be included on the next release? Just save it in Krecipes format, and send it to us."
      ));
    break;
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

void KrecipesView::resizeRightPane(int lpw,int lph)
{
QSize rpsize=rightPanel->size();
QPoint rpplace=rightPanel->pos();
rpsize.setWidth(width()-lpw);
rpplace.setX(lpw);
rightPanel->move(rpplace);
rightPanel->resize(rpsize);

}

#include "krecipesview.moc"

