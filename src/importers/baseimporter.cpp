/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "baseimporter.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kprogress.h>
#include <kmessagebox.h>

#include "recipe.h"
#include "recipedb.h"
#include "dialogs/recipeimportdialog.h"

BaseImporter::BaseImporter() :
  m_recipe_list(new RecipeList),
  m_error_msg(QString::null)
{
}

BaseImporter::~BaseImporter()
{
	delete m_recipe_list;
}

void BaseImporter::import( RecipeDB *db )
{
	RecipeImportDialog import_dialog(*m_recipe_list);

	if ( import_dialog.exec() != QDialog::Accepted )
	{
		//clear errors and messages so they won't be displayed
		m_error_msg = QString::null;
		m_warning_msgs.clear();
		return;
	}

	RecipeList selected_recipes = import_dialog.getSelectedRecipes();

	// Load Current Settings
	KConfig *config=kapp->config();
	config->setGroup("Import");
	bool overwrite = config->readBoolEntry( "OverwriteExisting", false );

	KProgressDialog *progress_dialog = new KProgressDialog( kapp->mainWidget(), 0, i18n("Importing selected recipes"), QString::null, true );
	KProgress *progress = progress_dialog->progressBar();
	progress->setTotalSteps( selected_recipes.count() );
	progress->setFormat(i18n("%v/%m Recipes"));

	ElementList prepMethodList; db->loadPrepMethods( &prepMethodList );
	
	RecipeList::iterator recipe_it;
	for ( recipe_it = selected_recipes.begin(); recipe_it != selected_recipes.end(); ++recipe_it )
	{
		if ( progress_dialog->wasCancelled() )
		{
			KMessageBox::information( kapp->mainWidget(), i18n("All recipes up unto this point have been successfully imported.") );
			return;
		}

		progress_dialog->setLabel( QString(i18n("Importing recipe: %1")).arg((*recipe_it).title) );
		progress->advance( 1 );
		kapp->processEvents();

		//add all recipe items (authors, ingredients, etc. to the database if they aren't already
		for ( IngredientList::iterator ing_it = (*recipe_it).ingList.begin(); ing_it != (*recipe_it).ingList.end(); ++ing_it )
		{
			int new_ing_id = db->findExistingIngredientByName((*ing_it).name);
			if ( new_ing_id == -1 && (*ing_it).name != "" )
			{
				db->createNewIngredient( (*ing_it).name );
				new_ing_id = db->lastInsertID();
			}

			int new_unit_id = db->findExistingUnitByName((*ing_it).units);
			if ( new_unit_id == -1 )
			{
				db->createNewUnit( (*ing_it).units );
				new_unit_id = db->lastInsertID();
			}
			
			int new_prep_id = 1; //1 is the null preparation method
			if ( !(*ing_it).prepMethod.isEmpty() )
			{
				Element prepMethodFound = prepMethodList.findByName( (*ing_it).prepMethod );
				new_prep_id = prepMethodFound.id;
				if ( new_prep_id == -1 )
				{
					db->createNewPrepMethod( (*ing_it).prepMethod );
					new_prep_id = db->lastInsertID();
					prepMethodList.append( Element( (*ing_it).prepMethod, new_prep_id ) );
				}
			}

			(*ing_it).prepMethodID = new_prep_id;
			(*ing_it).unitID = new_unit_id;
			(*ing_it).ingredientID = new_ing_id;

			ElementList unitsWithIng;
			db->findExistingUnitsByName( (*ing_it).units, new_ing_id, &unitsWithIng );

			if ( !unitsWithIng.containsId(new_unit_id) )
				db->addUnitToIngredient( new_ing_id, new_unit_id );
		}

		for ( ElementList::iterator author_it = (*recipe_it).authorList.begin(); author_it != (*recipe_it).authorList.end(); ++author_it )
		{
			int new_author_id = db->findExistingAuthorByName( (*author_it).name );
			if ( new_author_id == -1 && (*author_it).name != "" )
			{
				db->createNewAuthor((*author_it).name);
				new_author_id = db->lastInsertID();
			}

			(*author_it).id = new_author_id;
		}

		for ( ElementList::iterator cat_it = (*recipe_it).categoryList.begin(); cat_it != (*recipe_it).categoryList.end(); ++cat_it )
		{
			int new_cat_id = db->findExistingCategoryByName( (*cat_it).name );
			if ( new_cat_id == -1 && (*cat_it).name != "" )
			{
				db->createNewCategory((*cat_it).name);
				new_cat_id = db->lastInsertID();
			}

			(*cat_it).id = new_cat_id;
		}

		if ( overwrite ) //overwrite existing
			(*recipe_it).recipeID = db->findExistingRecipeByName( (*recipe_it).title );
		else //rename
			(*recipe_it).title = db->getUniqueRecipeTitle( (*recipe_it).title );


		//save into the database
		db->saveRecipe( &(*recipe_it) );
	}

	delete progress_dialog;
}

