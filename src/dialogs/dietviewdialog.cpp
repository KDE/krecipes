/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dietviewdialog.h"

#include <klocale.h>

DietViewDialog::DietViewDialog(QWidget *parent, RecipeList &recipeList):QWidget(parent)
{

 // Design the dialog

 	// Border spacers
 layout = new QGridLayout( this, 1, 1, 0, 0);
 QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addItem( spacer_left, 1,0 );
 QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
 layout->addItem(spacer_top,0,1);
 QSpacerItem* spacer_right = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addItem(spacer_right,1,2);
 QSpacerItem* spacer_bottom = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed);
 layout->addItem(spacer_bottom,2,1);

 	// The html part
 htmlBox=new QVBox (this); layout->addWidget(htmlBox,1,1);
 dietView=new KHTMLPart(htmlBox);


 // Show the diet
 showDiet(recipeList);
}

DietViewDialog::~DietViewDialog()
{
}

void DietViewDialog::showDiet(RecipeList &recipeList)
{
// Header
QString htmlCode=QString("<html><head><title>%1</title></head><body>").arg(i18n("Diet"));

// Title
htmlCode+=QString("<center><div STYLE=\"width: 80%\">");
htmlCode+=QString("<h1>%1</h1></div></center>").arg(i18n("Diet"));

// Diet table
/*
recipeHTML+="<div STYLE=\"border:medium solid blue; width:90%\"><table cellspacing=0px width=100%><tbody>";
        int counter=0;
        for (Ingredient *i=ingredientList->getFirst();i;i=ingredientList->getNext())
        {

        QString color;
        if (counter) color="#CBCEFF";*/

// Display it
dietView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where photos and logos can be stored
dietView->write(htmlCode);
dietView->end();

}
