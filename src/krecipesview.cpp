/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#include "krecipesview.h"

#include <qpainter.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <kurl.h>

#include <ktrader.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>

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
    database=new RecipeDB("localhost");

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


    boton4=new QPushButton(rightPanel); boton4->setFlat(true); boton4->setText("First");
    boton5=new QPushButton(rightPanel); boton5->setFlat(true); boton5->setText("Second");
    inputPanel=new RecipeInputDialog(rightPanel,database);
    viewPanel=new RecipeViewDialog(rightPanel,database,1);
    selectPanel=new SelectRecipeDialog(rightPanel,database);

    // Connect Signals from Left Panel to ChangePanel()
     connect( leftPanel, SIGNAL(clicked(int)), SLOT(slotSetPanel(int)) );

    rightPanel->raiseWidget(boton4);


    // Retransmit signal to parent to Enable/Disable the Save Button
    connect (inputPanel, SIGNAL(enableSaveOption(bool)), this, SIGNAL(enableSaveOption(bool)));

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
}

}
void KrecipesView::save(void)
{
inputPanel->save();
}
#include "krecipesview.moc"
