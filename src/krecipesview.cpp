/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#include "krecipesview.h"

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

KrecipesView::KrecipesView(QWidget *parent)
    : QVBox(parent)
{
    // Init the setup wizard if necessary
    wizard();

    // Initialize Database
    KConfig *config; config=kapp->config(); config->setGroup("Server");
    QString host=config->readEntry( "Host","localhost");
    QString user=config->readEntry( "Username",QString::null);
    QString pass=config->readEntry("Password",QString::null);
    QString dbname=config->readEntry( "DBName", DEFAULT_DB_NAME);
    std::cerr<<"Connecting to: "<<host<<"\n";
    database=new RecipeDB(host,user,pass,dbname);

    splitter=new QSplitter(this);

    // Set Splitter Parameters
    splitter->setFrameShape( QSplitter::NoFrame );
    splitter->setFrameShadow( QSplitter::Plain );
    splitter->setOrientation( QSplitter::Horizontal );
    splitter->setOpaqueResize();


// Create Left and Right Panels (splitter)


    leftPanel=new QButtonGroup(splitter);
    rightPanel=new QWidgetStack(splitter);

    // Design Resizing of the panels
   splitter->setResizeMode(leftPanel,QSplitter::FollowSizeHint);
   leftPanel->setMinimumWidth(150);


    // Design Left Panel
    il=new KIconLoader;
    button1=new QPushButton(leftPanel); button1->setFlat(true); button1->setText("Find/Edit Recipes");
    	QPixmap pm=il->loadIcon("filefind", KIcon::NoGroup,16); button1->setIconSet(pm);
    	button1->setGeometry(0,0,150,30);
    button7=new QPushButton(leftPanel); button7->setFlat(true); button7->setText("Shopping List");
	pm=il->loadIcon("table", KIcon::NoGroup,16); button7->setIconSet(pm);
	button7->setGeometry(0,30,150,30);
    button4=new QPushButton(leftPanel); button4->setFlat(true); button4->setText("Edit Ingredients");
    	pm=il->loadIcon("ingredients", KIcon::NoGroup,16); button4->setIconSet(pm);
        button4->setGeometry(0,60,150,30);
    button5=new QPushButton(leftPanel); button5->setFlat(true); button5->setText("Edit Property List");
        pm=il->loadIcon("properties", KIcon::NoGroup,16); button5->setIconSet(pm);
	button5->setGeometry(0,90,150,30);
    button6=new QPushButton(leftPanel); button6->setFlat(true); button6->setText("Edit Units");
	button6->setGeometry(0,120,150,30);


    // Right Panel Widgets
    inputPanel=new RecipeInputDialog(rightPanel,database); rightPanel->addWidget(inputPanel);
    viewPanel=new RecipeViewDialog(rightPanel,database,1);rightPanel->addWidget(viewPanel);
    selectPanel=new SelectRecipeDialog(rightPanel,database);rightPanel->addWidget(selectPanel);
    ingredientsPanel=new IngredientsDialog(rightPanel,database);rightPanel->addWidget(ingredientsPanel);
    propertiesPanel=new PropertiesDialog(rightPanel,database);rightPanel->addWidget(propertiesPanel);
    unitsPanel=new UnitsDialog(rightPanel,database); rightPanel->addWidget(unitsPanel);
    shoppingListPanel=new ShoppingListDialog(rightPanel,database); rightPanel->addWidget(shoppingListPanel);
    // Connect Signals from Left Panel to slotSetPanel()
     connect( leftPanel, SIGNAL(clicked(int)),this, SLOT(slotSetPanel(int)) );

    rightPanel->raiseWidget(viewPanel);


    // Retransmit signal to parent to Enable/Disable the Save Button
    connect (inputPanel, SIGNAL(enableSaveOption(bool)), this, SIGNAL(enableSaveOption(bool)));

    // Connect Signals from selectPanel (SelectRecipeDialog)

    connect (selectPanel, SIGNAL(recipeSelected(int,int)),this, SLOT(actionRecipe(int,int)));


    inputPanel->loadRecipe(1);

}

KrecipesView::~KrecipesView()
{
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
case 0: selectPanel->reload(); // Reload data
	this->rightPanel->raiseWidget(selectPanel);
	break;
case 1: shoppingListPanel->reload(); // Reload data
	this->rightPanel->raiseWidget(shoppingListPanel);
	break;
case 2: ingredientsPanel->reload();// Reload data
	this->rightPanel->raiseWidget(ingredientsPanel);
	break;
case 3: propertiesPanel->reload();
	this->rightPanel->raiseWidget(propertiesPanel);
	break;
case 4: unitsPanel->reload(); // Reload data
	this->rightPanel->raiseWidget(unitsPanel);
	break;

}

}
void KrecipesView::save(void)
{
inputPanel->save();
}

void KrecipesView::actionRecipe(int recipeID, int action)
{

if (action==0) // Open
  {
  viewPanel->loadRecipe(recipeID);
  rightPanel->raiseWidget(viewPanel);
  }
else if (action==1) // Edit
 {
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

bool setupDone=config->readBoolEntry( "SystemSetup",false);
if (!setupDone)
{
SetupWizard* setupWizard=new SetupWizard();
setupWizard->exec();
}

}

#include "krecipesview.moc"
