/*
 * Copyright (C) 2003 krecipes.sourceforge.net developers
 */

#include "krecipesview.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include <qpainter.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <kurl.h>
#include <kapp.h>
#include <ktrader.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kconfig.h>

#include "setupwizard.h"
#include "recipeinputdialog.h"
#include "recipeviewdialog.h"
#include "selectrecipedialog.h"
#include "ingredientsdialog.h"
#include "propertiesdialog.h"
#include "shoppinglistdialog.h"
#include "dietwizarddialog.h"
#include "categorieseditordialog.h"
#include "authorsdialog.h"
#include "unitsdialog.h"
#include "gui/kstartuplogo.h"

#ifdef USE_MYSQL_DATABASE
#include "DBBackend/mysqlrecipedb.h"
#endif

#ifdef USE_SQLITE_DATABASE
#include "DBBackend/literecipedb.h"
#endif

#include "menugroup.h"

KrecipesView::KrecipesView(QWidget *parent)
    : QVBox(parent)
{
    // Init the setup wizard if necessary
    wizard();

    // Show Splash Screen

    KStartupLogo* start_logo = 0L;
    start_logo = new KStartupLogo();
    start_logo -> setHideEnabled( true );
    start_logo->show();
    start_logo->raise();

    // Initialize Database
    KConfig *config; config=kapp->config(); config->setGroup("DBType");

    QString dbtype=config->readEntry("Type","SQLite");

    // Check if the database type is among those supported

    if ((dbtype!="SQLite") && (dbtype!="MySQL"))
    {
    std::cerr<<"Unrecognized database type "<<dbtype<<"\n";
    exit(1);
    }
    #ifdef USE_MYSQL_DATABASE

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

    #endif //USE_MYSQL_DATABASE


    #ifdef USE_SQLITE_DATABASE

    else // SQLite case
    	{
    	database=new LiteRecipeDB(QString::null); // server parameterss make no sense for SQLite
	}

    #endif //USE_SQLITE_DATABASE

    splitter=new QSplitter(this);

    // Set Splitter Parameters
    splitter->setFrameShape( QSplitter::NoFrame );
    splitter->setFrameShadow( QSplitter::Plain );
    splitter->setOrientation( QSplitter::Horizontal );
    splitter->setOpaqueResize();


// Create Left and Right Panels (splitter)


    leftPanel=new MenuGroup(splitter,"leftPanel");
    rightPanel=new QWidgetStack(splitter,"rightPanel");
   leftPanel->setMinimumWidth(22);

    // Design Resizing of the panels
   splitter->setResizeMode(leftPanel,QSplitter::FollowSizeHint);


    // Design Left Panel
    il=new KIconLoader;
    buttonsList = new QPtrList<MenuButton>();
    buttonsList->setAutoDelete( TRUE );

    button0=new MenuButton(leftPanel); button0->setFlat(true);
    button0->setIconSet(il->loadIconSet("filefind", KIcon::Small));
    button0->setGeometry(0,0,150,30);
    buttonsList->append(button0);

    button1=new MenuButton(leftPanel); button1->setFlat(true);
    button1->setIconSet(il->loadIconSet( "trolley", KIcon::Small ));
	  button1->setGeometry(0,30,150,30);
    buttonsList->append(button1);

    button2=new MenuButton(leftPanel); button2->setFlat(true);
    button2->setIconSet(il->loadIconSet( "ingredients", KIcon::Small ));
    button2->setGeometry(0,60,150,30);
    buttonsList->append(button2);

    button3=new MenuButton(leftPanel); button3->setFlat(true);
    button3->setIconSet(il->loadIconSet( "properties", KIcon::Small ));
	  button3->setGeometry(0,90,150,30);
    buttonsList->append(button3);

    button4=new MenuButton(leftPanel); button4->setFlat(true);
    button4->setIconSet(il->loadIconSet( "units", KIcon::Small ));
	  button4->setGeometry(0,120,150,30);
    buttonsList->append(button4);

    button5=new MenuButton(leftPanel); button5->setFlat(true);
    button5->setIconSet(il->loadIconSet( "categories", KIcon::Small ));
	  button5->setGeometry(0,150,150,30);
    buttonsList->append(button5);

    button6=new MenuButton(leftPanel); button6->setFlat(true);
    button6->setIconSet(il->loadIconSet( "personal", KIcon::Small ));
	  button6->setGeometry(0,180,150,30);
    buttonsList->append(button6);

    // Right Panel Widgets
    inputPanel=new RecipeInputDialog(rightPanel,database); rightPanel->addWidget(inputPanel);
    viewPanel=new RecipeViewDialog(rightPanel,database,1);rightPanel->addWidget(viewPanel);
    selectPanel=new SelectRecipeDialog(rightPanel,database);rightPanel->addWidget(selectPanel);
    ingredientsPanel=new IngredientsDialog(rightPanel,database);rightPanel->addWidget(ingredientsPanel);
    propertiesPanel=new PropertiesDialog(rightPanel,database);rightPanel->addWidget(propertiesPanel);
    unitsPanel=new UnitsDialog(rightPanel,database); rightPanel->addWidget(unitsPanel);
    shoppingListPanel=new ShoppingListDialog(rightPanel,database); rightPanel->addWidget(shoppingListPanel);
    dietPanel=new DietWizardDialog(rightPanel,database);rightPanel->addWidget(dietPanel);
    categoriesPanel=new CategoriesEditorDialog(rightPanel,database); rightPanel->addWidget(categoriesPanel);
    authorsPanel=new AuthorsDialog(rightPanel,database); rightPanel->addWidget(inputPanel);
    rightPanel->addWidget(authorsPanel);

    // i18n
    translate();

    // Initialize Variables
    recipeButton=0;



    // Connect Signals from Left Panel to slotSetPanel()
     connect( leftPanel, SIGNAL(clicked(int)),this, SLOT(slotSetPanel(int)) );
     connect(shoppingListPanel,SIGNAL(wizardClicked()),this,SLOT(slotSetDietWizardPanel()));


    rightPanel->raiseWidget(selectPanel);


    // Retransmit signal to parent to Enable/Disable the Save Button
    connect (inputPanel, SIGNAL(enableSaveOption(bool)), this, SIGNAL(enableSaveOption(bool)));

    // Create a new button when a recipe is unsaved
    connect (inputPanel, SIGNAL(createButton(QWidget*,QString)), this, SLOT(addRecipeButton(QWidget*,QString)));

    // Connect Signals from selectPanel (SelectRecipeDialog)

    connect (selectPanel, SIGNAL(recipeSelected(int,int)),this, SLOT(actionRecipe(int,int)));

    // Resize signal
    connect (leftPanel, SIGNAL(resized(int,int)),this, SLOT(resizeButtons()));

    // Close a recipe when requested (just switch panels)
    connect(inputPanel,SIGNAL(closeRecipe()),this,SLOT(closeRecipe()));

    // Show a recipe when requested (just switch panels)
    connect(inputPanel,SIGNAL(showRecipe(int)),this,SLOT(showRecipe(int)));

    // Add recipe to the shopping list when requested
    connect(inputPanel,SIGNAL(addRecipeToShoppingList(int)),shoppingListPanel,SLOT(addRecipeToShoppingList(int)));

    // Close Splash Screen
    delete start_logo;
}

KrecipesView::~KrecipesView()
{
delete il; // remove iconloader
}

void KrecipesView::translate(){
  button0->setTitle(i18n("Find/Edit Recipes"));
  button1->setText(i18n("Shopping List"));
  button2->setText(i18n("Ingredients"));
  button3->setText(i18n("Properties"));
  button4->setText(i18n("Units"));
  button5->setText(i18n("Recipe Categories"));
  button6->setText(i18n("Authors"));
}

void KrecipesView::print(QPainter *p, int height, int width)
{
    // do the actual printing, here
    // p->drawText(etc..)
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
	this->rightPanel->raiseWidget(selectPanel);
	break;
case ShoppingP: shoppingListPanel->reload(); // Reload data
	this->rightPanel->raiseWidget(shoppingListPanel);
	break;
case IngredientsP: ingredientsPanel->reload();// Reload data
	this->rightPanel->raiseWidget(ingredientsPanel);
	break;
case PropertiesP: propertiesPanel->reload();
	this->rightPanel->raiseWidget(propertiesPanel);
	break;
case UnitsP: unitsPanel->reload(); // Reload data
	this->rightPanel->raiseWidget(unitsPanel);
	break;
case CategoriesP: this->rightPanel->raiseWidget(categoriesPanel);
	break;
case DietWizardP: dietPanel->reload();
	this->rightPanel->raiseWidget(dietPanel);
	break;
case AuthorsP: authorsPanel->reload();
	this->rightPanel->raiseWidget(authorsPanel);
	break;

}

}
void KrecipesView::save(void)
{
inputPanel->save();
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
		  "Do you want to save changes made to this recipe before editing another recipe?")).arg(recipeButton->text()),
		   i18n("Unsaved changes") ) )
		{
			case KMessageBox::Yes: inputPanel->save(); break;
			case KMessageBox::No: break;
			case KMessageBox::Cancel: return;
		}
	}

	inputPanel->loadRecipe(recipeID);
	rightPanel->raiseWidget(inputPanel);
}
else if (action==2) //Remove
{
database->removeRecipe(recipeID);
selectPanel->reload();
}
}


void KrecipesView::createNewRecipe(void)
{
if ( !inputPanel->everythingSaved() )
{
	switch( KMessageBox::questionYesNoCancel( this,
	  QString(i18n("Recipe \"%1\" contains unsaved changes.\n"
	  "Do you want to save changes made to this recipe before creating a new recipe?")).arg(recipeButton->text()),
	   i18n("Unsaved changes") ) )
	{
		case KMessageBox::Yes: inputPanel->save(); break;
		case KMessageBox::No: break;
		case KMessageBox::Cancel: return;
	}
}

inputPanel->newRecipe();
rightPanel->raiseWidget(inputPanel);
}

void KrecipesView::createNewElement(void)
{
if (rightPanel->id(rightPanel->visibleWidget())==4) //Properties Panel is the active one
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
std::cerr<<"Setting up\n";
setupWizard->getOptions(setupUser,initData);

// Setup user if necessary

if (setupUser) // Don't setup user if checkbox of existing user... was set
	{
	std::cerr<<"Setting up user\n";
	setupWizard->getAdminInfo(adminEnabled,adminUser,adminPass);
	setupWizard->getServerInfo(isRemote,host,client,dbName,user,pass);

	if (!adminEnabled) // Use root without password
	{
	std::cerr<<"Using default admin\n";
	adminUser="root";
	adminPass=QString::null;
	}
	if (!isRemote) // Use localhost
	{
	std::cerr<<"Using localhost\n";
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
rightPanel->raiseWidget(dietPanel);
}


void KrecipesView::setupUserPermissions(const QString &host, const QString &client, const QString &dbName, const QString &newUser,const QString &newPass,const QString &adminUser,const QString &adminPass)
{
MySQLRecipeDB *db;

if (!adminPass.isNull())
	{ // Login as admin in the (remote) server and createDB if necessary
	std::cerr<<"Open db as:"<< adminUser.latin1() <<",*** with password ****\n";
	db= new MySQLRecipeDB(host,adminUser,adminPass,dbName,true); // true means initialize db structure (It won't destroy data if exists)
	}
	else{ // Login as root with no password
	std::cerr<<"Open db as root, with no password\n";
	db=new MySQLRecipeDB(host,"root",QString::null,dbName,true);
	}

db->givePermissions(dbName,newUser,newPass,client); // give permissions to the user

delete db; //it closes the db automatically
}

void KrecipesView::initializeData(const QString &host,const QString &dbName, const QString &user,const QString &pass)
{
MySQLRecipeDB *db;
db= new MySQLRecipeDB(host,user,pass,dbName);
db->emptyData();
db->initializeData();
delete db; //it closes the db automatically
}

void KrecipesView::resizeButtons(){
  QPtrListIterator<MenuButton> it( *buttonsList);	// iterate over menu buttons
  MenuButton *bt;
  while ( (bt = it.current()) != 0 ){
    bt->resize(leftPanel->width(), 30);
    bt->repaint();
    bt->fade();
    ++it;
  }
}

void KrecipesView::addRecipeButton(QWidget *w,QString title)
{
recipeWidget=w;

if (!recipeButton)
{
	recipeButton=new MenuButton(leftPanel,"recipeButton");
	recipeButton->setFlat(true);recipeButton->setIconSet(il->loadIconSet("filesave",KIcon::Small));
	recipeButton->setGeometry(0,250,150,30);
	recipeButton->setTitle(title);
	recipeButton->resize(leftPanel->width(),30);
	recipeButton->show();
  buttonsList->append(recipeButton);
	connect(recipeButton,SIGNAL(clicked()),this,SLOT(switchToRecipe()));
	connect((RecipeInputDialog *)w,SIGNAL(titleChanged(const QString&)),recipeButton,SLOT(setTitle(const QString&)));
}

}

void KrecipesView::switchToRecipe(void)
{
rightPanel->raiseWidget(recipeWidget);
}

void KrecipesView::closeRecipe(void)
{
selectPanel->reload();
rightPanel->raiseWidget(selectPanel);
delete recipeButton; recipeButton=0;
}

void KrecipesView::showRecipe(int recipeID)
{
viewPanel->loadRecipe(recipeID);
rightPanel->raiseWidget(viewPanel);
}

////////////////// Class MenuButton Methods///////////////////

MenuButton::MenuButton(QWidget *parent,const char *name):QPushButton(parent,name)
{
}

MenuButton::~MenuButton()
{
}

void MenuButton::setTitle(const QString &title)
{
  setText(title);
  QToolTip::add(this, title);
}

void MenuButton::enterEvent( QEvent * ){
  fade();
}

void MenuButton::leaveEvent( QEvent * ){
  fade();
}

void MenuButton::focusInEvent( QFocusEvent * ){
  repaint();
  fade();
}

void MenuButton::focusOutEvent( QFocusEvent * ){
  repaint();
  fade();
}

void MenuButton::fade(){
    int w = width();
    int h = height();
    KPixmap img1 = KPixmap(QSize(w/4, h-4));
    QBitmap bm( QSize( w/4, h-4 ) );
    bm = KPixmapEffect::gradient( img1, Qt::white, Qt::black, KPixmapEffect::HorizontalGradient, 0 );
    img1.fill(this->paletteBackgroundColor());
    img1.setMask( bm );
    bitBlt(this,3*w/4,2,&img1,0,Qt::CopyROP);
}

#include "krecipesview.moc"
