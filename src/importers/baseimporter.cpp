/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "baseimporter.h"

#include <kapplication.h>
#include <klocale.h>

#include "recipe.h"
#include "recipedb.h"
#include "dualprogressdialog.h"

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

void BaseImporter::import( RecipeDB *db, DualProgressDialog *progress_dialog )
{
	KProgress *sub_progress = 0;
	if (progress_dialog)
	{
		sub_progress = progress_dialog->subProgressBar();
		sub_progress->setTotalSteps( m_recipe_list->count() );
	}

	for ( Recipe *new_recipe = m_recipe_list->first(); new_recipe; new_recipe = m_recipe_list->next() )
	{
		if (progress_dialog)
		{
			if ( progress_dialog->wasCancelled() )
				return;

			//progress_dialog->setSubLabel( QString( i18n("Importing recipe: %1") ).arg(new_recipe->title) );
			sub_progress->advance( 1 );
			kapp->processEvents();
		}

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
			bool duplicate = false;
			for ( Element *unit = unitsWithIng.getFirst(); unit; unit = unitsWithIng.getNext() )
				if ( unit->id == new_unit_id ){ duplicate = true; break;}

			if ( !duplicate )
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
				db->createNewCategory(category->name);

			category->id = new_cat_id;
		}

		//TODO: add a preference to overwrite recipes with same title, or to rename
		new_recipe->title = db->getUniqueRecipeTitle( new_recipe->title ); //rename
		//new_recipe->recipeID = db->findExistingRecipeByName( new_recipe->title ); //overwrite existing

		//save into the database
		db->saveRecipe( new_recipe );
	}
}

