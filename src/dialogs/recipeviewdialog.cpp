/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "recipeviewdialog.h"
#include "image.h"
#include "propertycalculator.h"
#include <klocale.h>

#include "recipedb.h"

RecipeViewDialog::RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID):QVBox(parent)
{

// Initialize UI Elements
recipeView=new KHTMLPart(this);

// Store/Initialize local variables
database=db; // Store the database pointer.
loadedRecipe=new Recipe();
properties=new IngredientPropertyList;

//----------Load  the recipe --------
loadRecipe(recipeID);

//this->calculateProperties();


 }

RecipeViewDialog::~RecipeViewDialog()
{
}

void RecipeViewDialog::loadRecipe(int recipeID)
{

// Load specified Recipe ID
database->loadRecipe(loadedRecipe,recipeID);

// Calculate the property list
calculateProperties(loadedRecipe,database,properties);

// Display the recipe
showRecipe();
}

void RecipeViewDialog::showRecipe(void)
{
QString recipeHTML;



// Create HTML Code
if (loadedRecipe->recipeID<0)
{
// Show default (empty) recipe
recipeHTML=QString("<html><head><title>%1</title></head>").arg(i18n("Title of the Recipe"));
recipeHTML+=QString("<div STYLE=\"position: absolute; top: 30px; left:1%; width: 22%\"> <li>%1</li>").arg(i18n("Ingredient 1"));
recipeHTML+=QString("<li>%1</li> <li>%2</li> </div>").arg(i18n("Ingredient 2")).arg(i18n("Ingredient 3"));
recipeHTML+="<div STYLE=\"position: absolute; top: 30px; left:25%; width: 74%\">";
recipeHTML+=QString("<center><h1>%1</h1></center>").arg(i18n("Title of the Recipe"));
recipeHTML+=QString("<p>%1 </p></div></body></html>").arg(i18n("Recipe Instructions"));
}
else
{
// Format the loaded recipe as HTML code

// title (not shown)
recipeHTML= QString("<html><head><title>%1</title></head><body>").arg( loadedRecipe->title);

// Left Block (ingredients+properties)

// Ingredient Block
recipeHTML+="<div STYLE=\"position: absolute; top: 230px; left:1%; width: 220px; height: 240px; background-color:#D4A143 \">";
    //Ingredients
    Ingredient * ing;
    for ( ing = loadedRecipe->ingList.getFirst(); ing; ing = loadedRecipe->ingList.getNext() )
       {
       recipeHTML+=QString("<li>%1: %2 %3</li>")
			    .arg(ing->name)
			    .arg(ing->amount)
			    .arg(ing->units);
       }
recipeHTML+="</div>";

// Properties Block

recipeHTML+="<div STYLE=\"position: absolute; top: 480px; left:1%; width: 220px; background-color: #0071D3\">";
    //Properties
    IngredientProperty * prop;
    for ( prop = properties->getFirst(); prop; prop = properties->getNext() )
       {
       recipeHTML+=QString("<li>%1: %2  %3</li>")
			    .arg(prop->name)
			    .arg(prop->amount)
			    .arg(prop->units);
       }
recipeHTML+="</div>";

// Instructions Block
recipeHTML+="<div STYLE=\"margin-left: 240px;margin-right: 150;margin-top: 80px\">";
recipeHTML+=QString("<center><h1>%1</h1></center>").arg(loadedRecipe->title);
recipeHTML+=QString("<p>%1</p></div>").arg(loadedRecipe->instructions);

// Photo Block

  // Store Photo
  if (!loadedRecipe->photo.isNull()){
      if( (loadedRecipe->photo.width() > 220 || loadedRecipe->photo.height() > 165) || (loadedRecipe->photo.width() < 220 && loadedRecipe->photo.height() < 165) ){
        QImage pm = loadedRecipe->photo.convertToImage();
        (QPixmap(pm.smoothScale(220, 165, QImage::ScaleMin))).save("/tmp/krecipes_photo.png","PNG");
      }
      else{
        loadedRecipe->photo.save("/tmp/krecipes_photo.png","PNG");
      }
  }
  else {QPixmap dp(defaultPhoto); dp.save("/tmp/krecipes_photo.png","PNG");}
  }
recipeHTML+="<div STYLE=\"position: absolute; top: 50px; left:1%; width: 220px; height: 165px; border: solid #000000 1px;\" ALIGN=\"center\">";
recipeHTML+=QString("<img src=\"/tmp/krecipes_photo.png\" ");
if((QPixmap("/tmp/krecipes_photo.png")).height() < 165){
  int m = (165 - (QPixmap("/tmp/krecipes_photo.png")).height())/2;
  recipeHTML+=QString("STYLE=\"margin-top:"+QString::number(m)+"px;\"");
}
recipeHTML+=QString("> </div>");

// Header

recipeHTML+="<div STYLE=\"position: absolute; top: 5px; left:1%; width: 98%; height:30px; background-color: #EDD89E\">";
recipeHTML+=QString("<p align=right >Recipe: #%1</p></div>").arg(loadedRecipe->recipeID);

// Close HTML
recipeHTML+="</body></html>";

delete recipeView;              // Temporary workaround
recipeView=new KHTMLPart(this); // to avoid the problem of caching images of KHTMLPart

recipeView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where the photo was stored
recipeView->write(recipeHTML);
recipeView->end();
}

