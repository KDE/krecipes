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
#include <klocale.h>
#include <kprogress.h>
#include <kmessagebox.h>

#include "recipe.h"
#include "recipedb.h"
#include "dialogs/recipeimportdialog.h"

BaseImporter::BaseImporter() :
  m_recipe_list(new QPtrList<Recipe>),
  m_error_msg(QString::null)
{
	m_recipe_list->setAutoDelete( true );
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

	QPtrList<Recipe> *selected_recipes = import_dialog.getSelectedRecipes(); //no need to delete recipe pointers this contains; contains the same pointers as m_recipe_list

	// Load Current Settings
	KConfig *config=kapp->config();
	config->setGroup("Import");
	bool overwrite = config->readBoolEntry( "OverwriteExisting", false );

	KProgressDialog *progress_dialog = new KProgressDialog( 0,0,i18n("Importing selected recipes"), QString::null, true );
	KProgress *progress = progress_dialog->progressBar();
	progress->setTotalSteps( selected_recipes->count() );
	progress->setFormat(i18n("%v/%m Recipes"));

	for ( Recipe *new_recipe = selected_recipes->first(); new_recipe; new_recipe = selected_recipes->next() )
	{
		if ( progress_dialog->wasCancelled() )
		{
			KMessageBox::information( 0, i18n("All recipes up unto this point have been successfully imported.") );
			return;
		}

		progress_dialog->setLabel( QString(i18n("Importing recipe: %1")).arg(new_recipe->title) );
		progress->advance( 1 );
		kapp->processEvents();

		//add all recipe items (authors, ingredients, etc. to the database if they aren't already
		for ( Ingredient *ing = new_recipe->ingList.getFirst(); ing; ing = new_recipe->ingList.getNext() )
		{
			int new_ing_id = db->findExistingIngredientByName(ing->name);
			if ( new_ing_id == -1 && ing->name != "" )
			{
				db->createNewIngredient( ing->name );
				new_ing_id = db->lastInsertID();
			}

			int new_unit_id = db->findExistingUnitByName(ing->units);
			if ( new_unit_id == -1 )
			{
				db->createNewUnit( ing->units );
				new_unit_id = db->lastInsertID();
			}

			ing->unitID = new_unit_id;
			ing->ingredientID = new_ing_id;

			ElementList unitsWithIng;
			db->findExistingUnitsByName( ing->units, new_ing_id, &unitsWithIng );

			Element find_unit; find_unit.id = new_unit_id;
			if ( unitsWithIng.find(&find_unit) == -1 )
				db->addUnitToIngredient( new_ing_id, new_unit_id );
		}

		for ( Element *author = new_recipe->authorList.getFirst(); author; author = new_recipe->authorList.getNext() )
		{
			int new_author_id = db->findExistingAuthorByName( author->name );
			if ( new_author_id == -1 && author->name != "" )
			{
				db->createNewAuthor(author->name);
				new_author_id = db->lastInsertID();
			}

			author->id = new_author_id;
		}

		for ( Element *category = new_recipe->categoryList.getFirst(); category; category = new_recipe->categoryList.getNext() )
		{
			int new_cat_id = db->findExistingCategoryByName( category->name );
			if ( new_cat_id == -1 && category->name != "" )
			{
				db->createNewCategory(category->name);
				new_cat_id = db->lastInsertID();
			}

			category->id = new_cat_id;
		}

		if ( overwrite ) //overwrite existing
			new_recipe->recipeID = db->findExistingRecipeByName( new_recipe->title );
		else //rename
			new_recipe->title = db->getUniqueRecipeTitle( new_recipe->title );


		//save into the database
		db->saveRecipe( new_recipe );
	}

	delete selected_recipes;
	delete progress_dialog;
}

