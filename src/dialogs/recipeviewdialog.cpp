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

tmp_filename = locateLocal("tmp", "krecipes_recipe_view");
kdDebug()<<tmp_filename<<endl;

//----------Load  the recipe --------
loadRecipe(recipeID);

//this->calculateProperties();
}

RecipeViewDialog::~RecipeViewDialog()
{
	if ( recipe_loaded )
		removeOldFiles();
}

void RecipeViewDialog::loadRecipe(int recipeID)
{
	QValueList<int> ids; ids.append(recipeID);
	loadRecipes(ids);
}

void RecipeViewDialog::loadRecipes( const QValueList<int> &ids )
{
// Remove any files created by the last recipe loaded
removeOldFiles();

ids_loaded = ids; //need to save these ids in order to delete the html files later...make sure this comes after the call to removeOldFiles()

recipe_loaded = ( ids.count() > 0 && ids[0] >= 0 );

showRecipes( ids );
}

void RecipeViewDialog::showRecipes( const QValueList<int> &ids )
{
HTMLExporter html_generator( database, tmp_filename+".html", "html", parentWidget()->width() );

RecipeList recipe_list; database->loadRecipes( &recipe_list, ids );
html_generator.exporter( recipe_list ); //writes the generated HTML to 'tmp_filename+".html"'

delete recipeView;              // Temporary workaround
recipeView=new KHTMLPart(this); // to avoid the problem of caching images of KHTMLPart

KURL url;
url.setPath( tmp_filename+".html" );
recipeView->openURL( url );
}

void RecipeViewDialog::print(void)
{
	if ( recipeView && recipe_loaded )
		recipeView->view()->print();
}

void RecipeViewDialog::removeOldFiles()
{
	RecipeList recipe_list; database->loadRecipes( &recipe_list, ids_loaded );

	QStringList recipe_titles;
	for ( RecipeList::const_iterator it = recipe_list.begin(); it != recipe_list.end(); ++it )
		recipe_titles << (*it).title;

	HTMLExporter::removeHTMLFiles(tmp_filename,recipe_titles);
}
