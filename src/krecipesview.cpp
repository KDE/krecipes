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

#include <iostream>
#include <unistd.h>

#include <qpainter.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <kdebug.h>
#include <kurl.h>
#include <kapp.h>
#include <qpalette.h>
#include <ktrader.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kconfig.h>

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

#if HAVE_MYSQL
#include "DBBackend/mysqlrecipedb.h"
#endif

#if HAVE_SQLITE
#include "DBBackend/literecipedb.h"
#endif

#include "menugroup.h"

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

    KConfig *config; config=kapp->config();config->setGroup("DBType");
    dbtype=config->readEntry("Type","SQLite");


    // Check if the database type is among those supported

    if ((dbtype!="SQLite") && (dbtype!="MySQL"))
    {
    std::cerr<<"Unrecognized database type "<<dbtype<<"\n";
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

    #endif //USE_MYSQL_DATABASE


    #if HAVE_SQLITE

    else if (dbtype=="SQLite")// SQLite case
    	{
    	database=new LiteRecipeDB(QString::null); // server parameterss make no sense for SQLite
	}

    #endif //USE_SQLITE_DATABASE
    else{
    std::cerr<<"Unsupported database type. Exiting\n";
    exit(1);
    }

    splitter=new QSplitter(this);

    // Set Splitter Parameters
    splitter->setFrameShape( QSplitter::NoFrame );
    splitter->setFrameShadow( QSplitter::Plain );
    splitter->setOrientation( QSplitter::Horizontal );
    splitter->setOpaqueResize();


// Create Left and Right Panels (splitter)


    il=new KIconLoader;
    leftPanel=new MenuGroup(splitter,"leftPanel");
    rightPanel=new QWidgetStack(splitter,"rightPanel");
    leftPanel->setMinimumWidth(22);
    leftPanel->setPaletteBackgroundColor(QColor(238, 218, 156));

    logo = new QLabel(leftPanel, "logo");
    logo->setGeometry(1, 220, 144, 296);
    logo->setPixmap(QPixmap(locate("data", "krecipes/pics/menubg.png")));

    // Design Resizing of the panels
   splitter->setResizeMode(leftPanel,QSplitter::FollowSizeHint);


    // Design Left Panel
    buttonsList = new QPtrList<MenuButton>();
    buttonsList->setAutoDelete( TRUE );

    button0=new MenuButton(leftPanel); button0->setFlat(true);
    button0->setIconSet(il->loadIconSet("filefind", KIcon::Small));
    button0->setGeometry(2,2,146,30);
    buttonsList->append(button0);

    button1=new MenuButton(leftPanel); button1->setFlat(true);
    button1->setIconSet(il->loadIconSet( "trolley", KIcon::Small ));
	  button1->setGeometry(2,32,146,30);
    buttonsList->append(button1);

    button2=new MenuButton(leftPanel); button2->setFlat(true);
    button2->setIconSet(il->loadIconSet( "ingredients", KIcon::Small ));
    button2->setGeometry(2,62,146,30);
    buttonsList->append(button2);

    button3=new MenuButton(leftPanel); button3->setFlat(true);
    button3->setIconSet(il->loadIconSet( "properties", KIcon::Small ));
	  button3->setGeometry(2,92,146,30);
    buttonsList->append(button3);

    button4=new MenuButton(leftPanel); button4->setFlat(true);
    button4->setIconSet(il->loadIconSet( "units", KIcon::Small ));
	  button4->setGeometry(2,122,146,30);
    buttonsList->append(button4);

    button5=new MenuButton(leftPanel); button5->setFlat(true);
    button5->setIconSet(il->loadIconSet( "categories", KIcon::Small ));
	  button5->setGeometry(2,152,146,30);
    buttonsList->append(button5);

    button6=new MenuButton(leftPanel); button6->setFlat(true);
    button6->setIconSet(il->loadIconSet( "personal", KIcon::Small ));
	  button6->setGeometry(2,182,146,30);
    buttonsList->append(button6);

    contextButton = new QPushButton(leftPanel, "contextButton");
    contextButton->setIconSet(il->loadIconSet("krectip", KIcon::Small, 32));
    contextButton->setGeometry(112, 486, 32, 32);
    contextButton->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextButton->setFlat(true);

    contextHelp = new QWidget(leftPanel, "contextHelp");
    contextHelp->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextHelp->hide();
    QGridLayout* contextLayout = new QGridLayout( contextHelp, 0, 0, 0, 0);
    contextTitle = new QLabel(contextHelp, "contextTitle");
    contextTitle->setTextFormat(Qt::RichText);
    contextLayout->addMultiCellWidget(contextTitle, 0, 0, 0, 2);
    contextClose = new QPushButton(contextHelp, "contextClose");
    contextClose->setFixedSize(QSize(16,16));
    contextClose->setIconSet(il->loadIconSet("fileclose", KIcon::Small, 16));
    contextClose->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextClose->setFlat(true);
    contextLayout->addWidget(contextClose, 0, 2);
    contextText = new KTextBrowser(contextHelp, "contextText");
    contextText->setPaletteBackgroundColor(QColor(238, 218, 156));
    contextText->setTextFormat(Qt::RichText);
    contextText->setReadOnly(true);
    contextText->setWordWrap(QTextEdit::WidgetWidth);
    contextLayout->addMultiCellWidget(contextText, 1, 4, 0, 2);

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
    authorsPanel=new AuthorsDialog(rightPanel,database); rightPanel->addWidget(authorsPanel);

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

    rightPanel->raiseWidget(selectPanel);
    setContextHelp(SelectP);


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
    sleep(2);
    delete start_logo;
}

KrecipesView::~KrecipesView()
{
delete il; // remove iconloader
}

void KrecipesView::translate(){
  button0->setTitle(i18n("Find/Edit Recipes"));
  button1->setTitle(i18n("Shopping List"));
  button2->setTitle(i18n("Ingredients"));
  button3->setTitle(i18n("Properties"));
  button4->setTitle(i18n("Units"));
  button5->setTitle(i18n("Recipe Categories"));
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
{
  selectPanel->exportRecipe();
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
setContextHelp(RecipeEdit);
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
#endif // HAVE_MYSQL
}


void KrecipesView::initializeData(const QString &host,const QString &dbName, const QString &user,const QString &pass)
{
if ((dbtype!="MySQL")  && (dbtype!="SQLite")) // Need it Just to have the else's properly
{
std::cerr<<"Unrecognized database type. Exiting\n";
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
std::cerr<<"Unsupported database type. Exiting\n";
exit(1);
}

}

void KrecipesView::resizeButtons(){
  QPtrListIterator<MenuButton> it( *buttonsList);	// iterate over menu buttons
  MenuButton *bt;
  while ( (bt = it.current()) != 0 ){
    bt->resize((leftPanel->width())-4, 30);
    ++it;
  }
  logo->setGeometry(1, (leftPanel->height() - 298), 144, 296);
  contextButton->setGeometry(leftPanel->width() - 34, leftPanel->height() - 34, 32, 32);
  contextHelp->setGeometry(2, 2, leftPanel->width() - 4, leftPanel->height() - 4);
}

void KrecipesView::addRecipeButton(QWidget *w,QString title)
{
recipeWidget=w;

if (!recipeButton)
{
	recipeButton=new MenuButton(leftPanel,"recipeButton");
	recipeButton->setFlat(true);recipeButton->setIconSet(il->loadIconSet("filesave",KIcon::Small));
	recipeButton->setGeometry(2,252,146,30);
	recipeButton->setTitle(title);
	recipeButton->resize((leftPanel->width())-4,30);
  recipeButton->stackUnder(contextHelp);
  if(contextHelp->isShown()){
    contextHelp->hide();
  }
	recipeButton->show();
  buttonsList->append(recipeButton);
	connect(recipeButton,SIGNAL(clicked()),this,SLOT(switchToRecipe()));
	connect((RecipeInputDialog *)w,SIGNAL(titleChanged(const QString&)),recipeButton,SLOT(setTitle(const QString&)));
}

}

void KrecipesView::switchToRecipe(void)
{
rightPanel->raiseWidget(recipeWidget);
setContextHelp(RecipeEdit);
}

void KrecipesView::closeRecipe(void)
{
selectPanel->reload();
rightPanel->raiseWidget(selectPanel);
buttonsList->removeLast();
setContextHelp(SelectP);
recipeButton=0;
}

void KrecipesView::showRecipe(int recipeID)
{
viewPanel->loadRecipe(recipeID);
rightPanel->raiseWidget(viewPanel);
}

void KrecipesView::setContextHelp(int action){
  switch(action){
    case SelectP:
      contextTitle->setText(i18n("<b>Recipes list</b>"));
      contextText->setText(i18n("Now we can add text for helping users<br>"
      "<a href=\"http://krecipes.sourceforge.net\">http://krecipes.sourceforge.net</a><br>"
      "We can add text, links and images"));
    break;
    case ShoppingP:
      contextTitle->setText(i18n("<b>Shopping list</b>"));
    break;
  case IngredientsP:
      contextTitle->setText(i18n("<b>Ingredients list</b>"));
    break;
  case PropertiesP:
      contextTitle->setText(i18n("<b>Properties list</b>"));
    break;
  case UnitsP:
      contextTitle->setText(i18n("<b>Units list</b>"));
    break;
  case CategoriesP:
      contextTitle->setText(i18n("<b>Categories list</b>"));
    break;
  case DietWizardP:
      contextTitle->setText(i18n("<b>Diet Wizard</b>"));
    break;
  case AuthorsP:
      contextTitle->setText(i18n("<b>Authors list</b>"));
    break;
  case RecipeEdit:
      contextTitle->setText(i18n("<b>Edit recipe</b>"));
    break;
  }
}

////////////////// Class MenuButton Methods///////////////////

MenuButton::MenuButton(QWidget *parent,const char *name):QPushButton(parent,name)
{
  mouseOver = false;
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
  mouseOver = true;
  repaint();
}

void MenuButton::leaveEvent( QEvent * ){
  mouseOver = false;
  repaint();
}

void MenuButton::focusInEvent( QFocusEvent * ){
  repaint();
}

void MenuButton::focusOutEvent( QFocusEvent * ){
  repaint();
}

void MenuButton::setIconSet(QIconSet i){
  icon = new QPixmap(i.pixmap(QIconSet::Small,QIconSet::Normal,QIconSet::On));
}


void MenuButton::drawButtonLabel( QPainter *p ){

    QPixmap* pm( (QPixmap*)p->device() );
    KPixmap pn;
    QFont font( KGlobalSettings::generalFont() );

    // draw icon
    style().drawItem(p, QRect(5,0,width()-5,height()), Qt::AlignLeft | Qt::AlignVCenter, colorGroup(), true, icon, 0);

    // copy base button
    pn = *pm;

    // create gradient image
    QPixmap tmp(size());
    tmp.fill(Qt::white);
    QImage gradient = tmp.convertToImage();
    QImage grad = KImageEffect::gradient (QSize(20, height()), Qt::white, Qt::black, KImageEffect::HorizontalGradient, 65536);
    bitBlt(&gradient, width()-20, 0, &grad, 0, Qt::CopyROP);

    // draw button text
    style().drawItem(p, QRect(25, 0, width()-25, height()), Qt::AlignLeft | Qt::AlignVCenter, colorGroup(), true, 0, text());

    // draw focus border
    if(hasFocus()){
      style().drawPrimitive( QStyle::PE_FocusRect, p, style().subRect(QStyle::SR_PushButtonFocusRect, this), colorGroup() );
    }

    // blend button with text with button without text using gradient
    QImage src = pm->convertToImage();
    QImage blend = pn.convertToImage();
    QImage button = KImageEffect::blend (src, blend, gradient, KImageEffect::Red);

    // draw the button
    bitBlt(pm, 0, 0, &button, 0, Qt::CopyROP);
}

#include "krecipesview.moc"


