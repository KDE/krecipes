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
#include "datablocks/categorytree.h"
#include "datablocks/unit.h"

/** @brief A vector designed for fast searches and sorted insertions.
  *
  * This is accomplished by keeping a sorted list that is searched
  * using the binary search algorithm.  When items are inserted into
  * the vector the list remains sorted;  the position that new items
  * will be inserted is determined by a fast binary search.
  *
  * @author Jason Kivlighn
  */
class CustomVector : public QValueVector<Element>
{
public:
	CustomVector( const ElementList &list ) : QValueVector<Element>()
	{
		reserve(list.count());
	
		for ( ElementList::const_iterator it = list.begin(); it != list.end(); ++it )
		{
			QString name = (*it).name;
			if ( name.isNull() ) name = "";
				
			push_back(Element(name.lower(),(*it).id)); //lowercase to speed comparisons... this doesn't affect the way it is actually stored
		}
	}
	
	/** Find the string of this list assuming it is sorted using the binary search algorithm */
	int bsearch( const QString &d ) const
	{
		QString test_str = d;
		if ( test_str.isNull() )
			test_str = "";
 
		int n1 = 0;
		int n2 = count() - 1;
		int mid = 0;
		bool found = false;
		while ( n1 <= n2 )
		{
			int  res;
			mid = (n1 + n2)/2;
			res = QString::compare( test_str, at(mid).name );
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
		return at(mid).id;
	}
	
	/** Insert the element into a sorted list, keeping the list sorted.
	  * This uses a modified binary search to determine the location to
	  * insert the element.
	  */
	void inSort( const Element &element )
	{
		Element new_el = element;
		if ( new_el.name.isNull() ) new_el.name = "";

		if ( count() == 0 )
		{
			push_back(new_el);
			return;
		}

		int n1 = 0;
		int n2 = count() - 1;
		int mid = 0;

		int res;
		while ( n1 <= n2 )
		{
			mid = (n1 + n2)/2;
			res = QString::compare( new_el.name, at(mid).name );
			if ( res < 0 )
				n2 = mid - 1;
			else if ( res > 0 )
				n1 = mid + 1;
			else
				break;
		}

		if ( res > 0 )
			insert( begin()+mid+1, new_el );
		else
			insert( begin()+mid  , new_el );
	}
};



BaseImporter::BaseImporter() :
  m_recipe_list(new RecipeList),
  m_cat_structure(0),
  file_recipe_count(0)
{
}

BaseImporter::~BaseImporter()
{
	delete m_recipe_list;
	delete m_cat_structure;
}

void BaseImporter::add( const RecipeList &recipe_list )
{
	file_recipe_count += recipe_list.count();

	for ( RecipeList::const_iterator it = recipe_list.begin(); it != recipe_list.end(); ++it )
		m_recipe_list->append( *it );
}

void BaseImporter::parseFiles( const QStringList &filenames )
{
	for ( QStringList::const_iterator file_it = filenames.begin(); file_it != filenames.end(); ++file_it )
	{
		file_recipe_count = 0;
		parseFile(*file_it);
		
		if (  m_error_msgs.count() > 0 )
		{
			//<!doc> ensures it is detected as RichText
			m_master_error += QString(i18n("<!doc>Import of recipes from the file <b>\"%1\"</b> <b>failed</b> due to the following error(s):")).arg(*file_it);
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

void BaseImporter::import( RecipeDB *db, bool automatic )
{
	if ( m_recipe_list->count() == 0 )
		return;

	RecipeList selected_recipes;
	if ( !automatic ) {
		RecipeImportDialog import_dialog(*m_recipe_list);
	
		if ( import_dialog.exec() != QDialog::Accepted )
		{
			//clear errors and messages so they won't be displayed
			m_error_msgs.clear();
			m_warning_msgs.clear();
			return;
		}

		selected_recipes = import_dialog.getSelectedRecipes();
	}
	else
		selected_recipes = *m_recipe_list;

	//cache some data we'll need
	int max_author_length = db->maxAuthorNameLength();
	int max_category_length = db->maxCategoryNameLength();
	int max_ing_length = db->maxIngredientNameLength();
	int max_prepmethod_length = db->maxPrepMethodNameLength();
	int max_units_length = db->maxUnitNameLength();
	int max_group_length = db->maxIngGroupNameLength();

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

	UnitList unitList; db->loadUnits( &unitList );
	/*CustomVector unitVector( unitList ); qHeapSort( unitVector );*/
	
	ElementList authorList; db->loadAuthors( &authorList );
	CustomVector authorVector( authorList ); qHeapSort( authorVector );
	
	ElementList catList; db->loadCategories( &catList );
	CustomVector catVector( catList ); qHeapSort( catVector );

	if ( m_cat_structure )
		importCategoryStructure(db,catVector,m_cat_structure);

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

		ElementList ingGroupList;

		//add all recipe items (authors, ingredients, etc. to the database if they aren't already
		for ( IngredientList::iterator ing_it = (*recipe_it).ingList.begin(); ing_it != (*recipe_it).ingList.end(); ++ing_it )
		{
			//create ingredient groups
			Element el = ingGroupList.findByName( (*ing_it).group );
			if ( el.id != -1 )
				(*ing_it).groupID = el.id;
			else if ( !(*ing_it).group.isEmpty() ) {
				QString real_group_name = (*ing_it).group.left(max_group_length);
				db->createNewIngGroup( real_group_name );
				(*ing_it).groupID = db->lastInsertID();

				ingGroupList.append( Element((*ing_it).group,(*ing_it).groupID) );
			}

			QString real_ing_name = (*ing_it).name.left(max_ing_length);
			int new_ing_id = ingVector.bsearch(real_ing_name.lower());
			if ( new_ing_id == -1 && !real_ing_name.isEmpty() )
			{
				db->createNewIngredient( real_ing_name );
				new_ing_id = db->lastInsertID();
				ingVector.inSort( Element( real_ing_name.lower(), new_ing_id ) );
			}

			Unit real_unit((*ing_it).units.name.left(max_units_length),(*ing_it).units.plural.left(max_units_length));
			UnitList::const_iterator unit_result = unitList.find(real_unit);
			int new_unit_id;
			if ( unit_result == unitList.end() )
			{
				db->createNewUnit( real_unit.name, real_unit.plural );
				new_unit_id = db->lastInsertID();
				unitList.append( Unit( real_unit.name, real_unit.plural, new_unit_id ) );
			}
			else
				new_unit_id = (*unit_result).id;
			
			int new_prep_id = -1;
			if ( !(*ing_it).prepMethod.isEmpty() )
			{
				QString real_prep_name = (*ing_it).prepMethod.left(max_prepmethod_length);

				new_prep_id = prepMethodVector.bsearch(real_prep_name.lower());
				if ( new_prep_id == -1 )
				{
					db->createNewPrepMethod( real_prep_name );
					new_prep_id = db->lastInsertID();
					prepMethodVector.inSort( Element( real_prep_name.lower(), new_prep_id ) );
				}
			}

			(*ing_it).prepMethodID = new_prep_id;
			(*ing_it).unitID = new_unit_id;
			(*ing_it).ingredientID = new_ing_id;

			ElementList unitsWithIng;
			db->findExistingUnitsByName( (*ing_it).units.name, new_ing_id, &unitsWithIng );
			db->findExistingUnitsByName( (*ing_it).units.plural, new_ing_id, &unitsWithIng );

			if ( !unitsWithIng.containsId(new_unit_id) )
				db->addUnitToIngredient( new_ing_id, new_unit_id );
		}

		for ( ElementList::iterator author_it = (*recipe_it).authorList.begin(); author_it != (*recipe_it).authorList.end(); ++author_it )
		{
			QString real_author_name = (*author_it).name.left(max_author_length);
			int new_author_id = authorVector.bsearch( real_author_name.lower() );
			if ( new_author_id == -1 && !real_author_name.isEmpty() )
			{
				db->createNewAuthor(real_author_name);
				new_author_id = db->lastInsertID();
				authorVector.inSort( Element( real_author_name.lower(), new_author_id ) );
			}

			(*author_it).id = new_author_id;
		}

		for ( ElementList::iterator cat_it = (*recipe_it).categoryList.begin(); cat_it != (*recipe_it).categoryList.end(); ++cat_it )
		{
			QString real_category_name = (*cat_it).name.left(max_category_length);
			int new_cat_id = catVector.bsearch( real_category_name.lower() );
			if ( new_cat_id == -1 && !real_category_name.isEmpty() )
			{
				db->createNewCategory(real_category_name);
				new_cat_id = db->lastInsertID();
				catVector.inSort( Element( real_category_name.lower(), new_cat_id ) );
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

	importUnitRatios( db );

	delete progress_dialog;
}

void BaseImporter::setCategoryStructure( CategoryTree *cat_structure )
{
	delete m_cat_structure;
	m_cat_structure = cat_structure;
}

void BaseImporter::importCategoryStructure( RecipeDB *db, CustomVector &catVector, const CategoryTree *cat_tree )
{
	const CategoryTreeChildren *children = cat_tree->children();
	for ( CategoryTreeChildren::const_iterator child_it = children->begin(); child_it != children->end(); ++child_it ) {
		int new_cat_id = catVector.bsearch( (*child_it)->category.name.lower() );
		if ( new_cat_id == -1 ) {
			db->createNewCategory( (*child_it)->category.name, cat_tree->category.id );
			new_cat_id = db->lastInsertID();
			catVector.inSort( Element( (*child_it)->category.name.lower(), new_cat_id) );
		}

		(*child_it)->category.id = new_cat_id;

		importCategoryStructure( db, catVector, *child_it );
	}
}

void BaseImporter::setUnitRatioInfo( UnitRatioList &ratioList, UnitList &unitList )
{
	m_ratioList = ratioList;
	m_unitList = unitList;
}

void BaseImporter::importUnitRatios( RecipeDB *db )
{
	for ( UnitRatioList::const_iterator it = m_ratioList.begin(); it != m_ratioList.end(); ++it ) {
		QString unitName1, unitName2;
		for ( UnitList::const_iterator unit_it = m_unitList.begin(); unit_it != m_unitList.end(); ++unit_it ) {
			if ( (*it).uID1 == (*unit_it).id ) {
			 	unitName1 = (*unit_it).name;
				if ( !unitName2.isNull() )
					break;
			}
			else if ( (*it).uID2 == (*unit_it).id ) {
				unitName2 = (*unit_it).name;
				if ( !unitName1.isNull() )
					break;
			}
		}

		int unitId1 = db->findExistingUnitByName(unitName1);
		int unitId2 = db->findExistingUnitByName(unitName2);

		//the unit needed for the ratio may not have been added, because the
		//recipes chosen did not include the unit
		if ( unitId1 != -1 && unitId2 != -1 ) {
			UnitRatio ratio;
			ratio.uID1 = unitId1; ratio.uID2 = unitId2;
			ratio.ratio = (*it).ratio;
			db->saveUnitRatio( &ratio );
		}
	}
}
