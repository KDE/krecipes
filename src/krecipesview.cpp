/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
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

KrecipesView::KrecipesView(QWidget *parent)
    : QVBox(parent)
{
  splitter=new QSplitter(this);

    // Set Splitter Parameters
    splitter->setFrameShape( QSplitter::NoFrame );
    splitter->setFrameShadow( QSplitter::Plain );
    splitter->setOrientation( QSplitter::Horizontal );
    splitter->setOpaqueResize();

    // Initialize Database
    KConfig *config; config=kapp->config(); config->setGroup("Server");
    QString host=config->readEntry( "Host","localhost");
    QString user=config->readEntry( "Username",QString::null);
    QString pass=config->readEntry("Password",QString::null);
    QString dbname=config->readEntry( "DBName", DEFAULT_DB_NAME);
    std::cerr<<"Connecting to: "<<host<<"\n";

    database=new RecipeDB(host,user,pass,dbname);

// Create Left and Right Panels (splitter)


    leftPanel=new QButtonGroup(splitter);
    rightPanel=new QWidgetStack(splitter);

    // Design Resizing of the panels
   splitter->setResizeMode(leftPanel,QSplitter::FollowSizeHint);
   leftPanel->setMinimumWidth(130);


    // Design Left Panel

    boton1=new QPushButton(leftPanel); boton1->setFlat(true); boton1->setText("Select Recipe");
    	boton1->setGeometry(0,0,130,30);
    boton2=new QPushButton(leftPanel); boton2->setFlat(true); boton2->setText("View Recipe");
    	boton2->setGeometry(0,30,130,30);
    boton3=new QPushButton(leftPanel); boton3->setFlat(true); boton3->setText("Edit Recipe");
        boton3->setGeometry(0,60,130,30);
    boton4=new QPushButton(leftPanel); boton4->setFlat(true); boton4->setText("Edit Ingredients");
        boton4->setGeometry(0,90,130,30);
    boton5=new QPushButton(leftPanel); boton5->setFlat(true); boton5->setText("Edit Property List");
	boton5->setGeometry(0,120,130,30);
    boton6=new QPushButton(leftPanel); boton6->setFlat(true); boton6->setText("Edit Units");
	boton6->setGeometry(0,150,130,30);

    // Right Panel Widgets
    inputPanel=new RecipeInputDialog(rightPanel,database); rightPanel->addWidget(inputPanel);
    viewPanel=new RecipeViewDialog(rightPanel,database,1);rightPanel->addWidget(viewPanel);
    selectPanel=new SelectRecipeDialog(rightPanel,database);rightPanel->addWidget(selectPanel);
    ingredientsPanel=new IngredientsDialog(rightPanel,database);rightPanel->addWidget(ingredientsPanel);
    propertiesPanel=new PropertiesDialog(rightPanel,database);rightPanel->addWidget(propertiesPanel);
    unitsPanel=new UnitsDialog(rightPanel,database); rightPanel->addWidget(unitsPanel);

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
case 0: this->rightPanel->raiseWidget(selectPanel);
	break;
case 1: this->rightPanel->raiseWidget(viewPanel);
	break;
case 2: this->rightPanel->raiseWidget(inputPanel);
	break;
case 3: this->rightPanel->raiseWidget(ingredientsPanel);
	break;
case 4: this->rightPanel->raiseWidget(propertiesPanel);
	break;
case 5: this->rightPanel->raiseWidget(unitsPanel);
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
selectPanel->loadRecipeList();
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


#include "krecipesview.moc"
