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

#include <qvaluevector.h>

#include "recipe.h"
#include "recipedb.h"
#include "dialogs/recipeimportdialog.h"

class CustomVector : public QValueVector<Element>
{
public:
	CustomVector( const ElementList &list ) : QValueVector<Element>()
	{
		reserve(list.count());
	
		for ( ElementList::const_iterator it = list.begin(); it != list.end(); ++it )
			push_back(Element((*it).name.lower(),(*it).id)); //lowercase to speed comparisons... this doesn't affect the way it is actually stored
	}
	
	/** Find the string of this list assuming it is sorted using the binary search algorithm */
	int bsearch( const QString &d ) const
	{
		int n1 = 0;
		int n2 = count() - 1;
		int mid = 0;
		bool found = false;
		while ( n1 <= n2 )
		{
			int  res;
			mid = (n1 + n2)/2;
			res = QString::compare( d, this->at(mid).name );
			if ( res < 0 )
				n2 = mid - 1;
			else if ( res > 0 )
				n1 = mid + 1;
			else
			{
				found = true;
				break;
			}
		}

		if ( !found )
			return -1;

		// search to first of equal items
		//while ( (mid - 1 >= 0) && !((QGVector*)this)->compareItems(d, vec[mid-1]) )
		//	mid--;
		return this->at(mid).id;
	}
	
	/** Insert the element into a sorted list, keeping the list sorted.
	  * This uses a modified binary search to determine the location to
	  * insert the element.
	  */
	void inSort( const Element &element )
	{
		if ( count() == 0 )
		{
			push_back(element);
			return;
		}

		int n1 = 0;
		int n2 = count() - 1;
		int mid = 0;

		int res;
		while ( n1 <= n2 )
		{
			mid = (n1 + n2)/2;
			res = QString::compare( element.name, this->at(mid).name );
			if ( res < 0 )
				n2 = mid - 1;
			else if ( res > 0 )
				n1 = mid + 1;
			else
				break;
		}

		if ( res > 0 )
			insert( begin()+mid+1, element );
		else
			insert( begin()+mid  , element );
	}
};



BaseImporter::BaseImporter() :
  m_recipe_list(new RecipeList),
  file_recipe_count(0)
{
}

BaseImporter::~BaseImporter()
{
	delete m_recipe_list;
}

void BaseImporter::parseFiles( const QStringList &filenames )
{
	for ( QStringList::const_iterator file_it = filenames.begin(); file_it != filenames.end(); ++file_it )
	{
		file_recipe_count = 0;
		parseFile(*file_it);
		
		if (  m_error_msgs.count() > 0 )
		{
			m_master_error += QString(i18n("Import of recipes from the file <b>%1</b> <b>failed</b> due to the following error(s):")).arg(*file_it);
			m_master_error += "<ul><li>" + m_error_msgs.join("</li><li>") + "</li></ul>";

			m_error_msgs.clear();
		}
		else if ( m_warning_msgs.count() > 0 )
		{
			m_master_warning += QString(i18n("The file <b>%1</b> generated the following warning(s):")).arg(*file_it);
			m_master_warning += "<ul><li>" + m_warning_msgs.join("</li><li>") + "</li></ul>";

			m_warning_msgs.clear();
		}
	}
}

void BaseImporter::import( RecipeDB *db )
{
	if ( m_recipe_list->count() == 0 )
		return;

	RecipeImportDialog import_dialog(*m_recipe_list);

	if ( import_dialog.exec() != QDialog::Accepted )
	{
		//clear errors and messages so they won't be displayed
		m_error_msgs.clear();
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


	//binary search these vectors instead of querying the database to see if elements exist
	ElementList prepMethodList; db->loadPrepMethods( &prepMethodList );
	CustomVector prepMethodVector( prepMethodList ); qHeapSort( prepMethodVector );
	
	ElementList ingList; db->loadIngredients( &ingList );
	CustomVector ingVector( ingList ); qHeapSort( ingVector );

	ElementList unitList; db->loadUnits( &unitList );
	CustomVector unitVector( unitList ); qHeapSort( unitVector );
	
	ElementList authorList; db->loadAuthors( &authorList );
	CustomVector authorVector( authorList ); qHeapSort( authorVector );
	
	ElementList catList; db->loadCategories( &catList );
	CustomVector catVector( catList ); qHeapSort( catVector );


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
			int new_ing_id = ingVector.bsearch((*ing_it).name.lower());
			if ( new_ing_id == -1 && (*ing_it).name != "" )
			{
				db->createNewIngredient( (*ing_it).name );
				new_ing_id = db->lastInsertID();
				ingVector.inSort( Element( (*ing_it).name.lower(), new_ing_id ) );
			}

			int new_unit_id = unitVector.bsearch((*ing_it).units.lower());
			if ( new_unit_id == -1 )
			{
				db->createNewUnit( (*ing_it).units );
				new_unit_id = db->lastInsertID();
				unitVector.inSort( Element( (*ing_it).units.lower(), new_unit_id ) );
			}
			
			int new_prep_id = -1;
			if ( !(*ing_it).prepMethod.isEmpty() )
			{
				new_prep_id = prepMethodVector.bsearch((*ing_it).prepMethod.lower());
				if ( new_prep_id == -1 )
				{
					db->createNewPrepMethod( (*ing_it).prepMethod );
					new_prep_id = db->lastInsertID();
					prepMethodVector.inSort( Element( (*ing_it).prepMethod.lower(), new_prep_id ) );
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
			int new_author_id = authorVector.bsearch( (*author_it).name.lower() );
			if ( new_author_id == -1 && (*author_it).name != "" )
			{
				db->createNewAuthor((*author_it).name);
				new_author_id = db->lastInsertID();
				authorVector.inSort( Element( (*author_it).name.lower(), new_author_id ) );
			}

			(*author_it).id = new_author_id;
		}

		for ( ElementList::iterator cat_it = (*recipe_it).categoryList.begin(); cat_it != (*recipe_it).categoryList.end(); ++cat_it )
		{
			int new_cat_id = catVector.bsearch( (*cat_it).name.lower() );
			if ( new_cat_id == -1 && (*cat_it).name != "" )
			{
				db->createNewCategory((*cat_it).name);
				new_cat_id = db->lastInsertID();
				catVector.inSort( Element( (*cat_it).name.lower(), new_cat_id ) );
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

