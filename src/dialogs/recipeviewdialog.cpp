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

#include "recipeviewdialog.h"

#include <kconfig.h>
#include <kdebug.h>
#include <khtmlview.h>
#include <khtml_part.h>
#include <kstandarddirs.h>

#include "mixednumber.h"
#include "DBBackend/recipedb.h"
#include "exporters/htmlexporter.h"

RecipeViewDialog::RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID):QVBox(parent)
{

// Initialize UI Elements
recipeView=new KHTMLPart(this);

// Store/Initialize local variables
database=db; // Store the database pointer.
loadedRecipe=new Recipe();

tmp_filename = locateLocal("tmp", "krecipes_recipe_view");
kdDebug()<<tmp_filename<<endl;

//----------Load  the recipe --------
loadRecipe(recipeID);

//this->calculateProperties();
}

RecipeViewDialog::~RecipeViewDialog()
{
HTMLExporter::removeHTMLFiles(tmp_filename,loadedRecipe->title);
}

void RecipeViewDialog::loadRecipe(int recipeID)
{
// Remove any files created by the last recipe loaded
HTMLExporter::removeHTMLFiles(tmp_filename,loadedRecipe->title);

// Load specified Recipe ID
database->loadRecipe(loadedRecipe,recipeID);

// Display the recipe
showRecipe();
}

void RecipeViewDialog::showRecipe(void)
{
HTMLExporter html_generator( database, tmp_filename+".html", "html", ((QWidget*)parent())->width() );
html_generator.exporter( loadedRecipe->recipeID );

delete recipeView;              // Temporary workaround
recipeView=new KHTMLPart(this); // to avoid the problem of caching images of KHTMLPart

KURL url;
url.setPath( tmp_filename+".html" );
recipeView->openURL( url );
}

void RecipeViewDialog::print(void)
{
	if ( recipeView && loadedRecipe->recipeID >= 0 )
		recipeView->view()->print();
}
