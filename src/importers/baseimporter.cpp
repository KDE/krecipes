/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
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
#include <kprogressdialog.h>
#include <kmessagebox.h>
#include <kglobal.h>

#include "datablocks/recipe.h"
#include "backends/recipedb.h"
#include "datablocks/categorytree.h"
#include "datablocks/unit.h"

BaseImporter::BaseImporter() :
		m_recipe_list( new RecipeList ),
		m_cat_structure( 0 ),
		file_recipe_count( 0 )
{
	KConfigGroup config = KGlobal::config()->group( "Import" );

	direct = config.readEntry( "DirectImport", false );
}

BaseImporter::~BaseImporter()
{
	delete m_recipe_list;
	delete m_cat_structure;
}

void BaseImporter::add( const RecipeList &recipe_list )
{
	file_recipe_count += recipe_list.count();

	for ( RecipeList::const_iterator it = recipe_list.begin(); it != recipe_list.end(); ++it ) {
		Recipe copy = *it;
		copy.recipeID = -1; //make sure an importer didn't give this a value
		for ( RatingList::iterator rating_it = copy.ratingList.begin(); rating_it != copy.ratingList.end(); ++rating_it ) {
			(*rating_it).setId(-1);
		}
		m_recipe_list->append( copy );
	}

	if ( direct ) {
		if ( !m_progress_dialog->wasCancelled() )
			importRecipes( *m_recipe_list, m_database, m_progress_dialog );
	}
}

void BaseImporter::add( const Recipe &recipe )
{
	file_recipe_count++;
	Recipe copy = recipe;
	copy.recipeID = -1; //make sure an importer didn't give this a value

	if ( direct ) {
		if ( !m_progress_dialog->wasCancelled() ) {
			RecipeList list;
			list.append( recipe );
			importRecipes( list, m_database, m_progress_dialog );
		}
	}
	else
		m_recipe_list->append( copy );
}

void BaseImporter::parseFiles( const QStringList &filenames )
{
	if ( direct )
		m_filenames = filenames;
	else {
		for ( QStringList::const_iterator file_it = filenames.begin(); file_it != filenames.end(); ++file_it ) {
			file_recipe_count = 0;
			parseFile( *file_it );
			processMessages( *file_it );
		}
	}
}

void BaseImporter::import( RecipeDB *db, bool /*automatic*/ )
{
	if ( direct ) {
		m_database = db;

		m_progress_dialog = new KProgressDialog( kapp->mainWidget(), i18n( "Importing selected recipes" ), QString(), Qt::Dialog );
		m_progress_dialog->setModal( true );
		m_progress_dialog->progressBar()->setRange( 0, 0 );
	
		for ( QStringList::const_iterator file_it = m_filenames.constBegin(); file_it != m_filenames.constEnd(); ++file_it ) {
			file_recipe_count = 0;
			parseFile( *file_it );
			processMessages( *file_it );
	
			if ( m_progress_dialog->wasCancelled() )
				break;
		}
		
		importUnitRatios( db );
		delete m_progress_dialog;
	}
	else {
		if ( m_recipe_list->count() == 0 )
			return;
	
		m_recipe_list->empty();
		//db->blockSignals(true);

		m_progress_dialog = new KProgressDialog( kapp->mainWidget(), i18n( "Importing selected recipes" ), QString(), Qt::Dialog );
		m_progress_dialog->setModal( true );
		m_progress_dialog->progressBar()->setRange( 0, m_recipe_list->count() );
		m_progress_dialog->progressBar()->setFormat( i18n( "%v/%m Recipes" ) );

		if ( m_cat_structure ) {
			importCategoryStructure( db, m_cat_structure );
			delete m_cat_structure;
			m_cat_structure = 0;
		}
		importRecipes( *m_recipe_list, db, m_progress_dialog );
		importUnitRatios( db );
	
		//db->blockSignals(false);
		delete m_progress_dialog; m_progress_dialog = 0;
	}
}

void BaseImporter::importIngredient( IngredientData &ing, RecipeDB *db, KProgressDialog *progress_dialog )
{
	//cache some data we'll need
	int max_units_length = db->maxUnitNameLength();
	int max_group_length = db->maxIngGroupNameLength();

	if ( direct ) {
		progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
		kapp->processEvents();
	}

	//create ingredient groups
	QString real_group_name = ing.group.left( max_group_length );
	int new_group_id = db->findExistingIngredientGroupByName(real_group_name);
	if ( new_group_id == -1 ) {
		new_group_id = db->createNewIngGroup( real_group_name );
	}
	ing.groupID = new_group_id;

	int new_ing_id = db->findExistingIngredientByName(ing.name);
	if ( new_ing_id == -1 && !ing.name.isEmpty() )
	{
		new_ing_id = db->createNewIngredient( ing.name );
	}

	if ( direct ) {
		progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
		kapp->processEvents();
	}

	Unit real_unit( ing.units.name().left( max_units_length ), ing.units.plural().left( max_units_length ) );
	if ( real_unit.name().isEmpty() )
		real_unit.setName(real_unit.plural());
	else if ( real_unit.plural().isEmpty() )
		real_unit.setPlural(real_unit.name());

	int new_unit_id = db->findExistingUnitByName(real_unit.name());
	if ( new_unit_id == -1 ) {
		new_unit_id = db->createNewUnit( Unit(real_unit.name(), real_unit.plural()) );
	}

	if ( direct ) {
		progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
		kapp->processEvents();
	}

	if ( ing.prepMethodList.count() > 0 ) {
		for ( ElementList::iterator prep_it = ing.prepMethodList.begin(); prep_it != ing.prepMethodList.end(); ++prep_it ) {
			int new_prep_id = db->findExistingPrepByName((*prep_it).name);
			if ( new_prep_id == -1 ) {
				new_prep_id = db->createNewPrepMethod((*prep_it).name);
			}
			(*prep_it).id = new_prep_id;
		}
	}

	ing.units.setId(new_unit_id);
	ing.ingredientID = new_ing_id;

	if ( !db->ingredientContainsUnit( new_ing_id, new_unit_id ) )
		db->addUnitToIngredient( new_ing_id, new_unit_id );
}

void BaseImporter::importRecipes( RecipeList &selected_recipes, RecipeDB *db, KProgressDialog *progress_dialog )
{
	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Import" );
	bool overwrite = config.readEntry( "OverwriteExisting", false );

	RecipeList::iterator recipe_it; RecipeList::iterator recipe_list_end( selected_recipes.end() );
	RecipeList::iterator recipe_it_old = selected_recipes.end();
	for ( recipe_it = selected_recipes.begin(); recipe_it != recipe_list_end; ++recipe_it ) {
		if ( !direct ) {
			if ( progress_dialog->wasCancelled() ) {
				KMessageBox::information( kapp->mainWidget(), i18n( "All recipes up unto this point have been successfully imported." ) );
				//db->blockSignals(false);
				return ;
			}
		}

		if ( recipe_it_old != selected_recipes.end() )
			selected_recipes.erase( recipe_it_old );

		progress_dialog->setLabelText( i18n( "Importing recipe: %1" ,( *recipe_it ).title ));
		progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
		kapp->processEvents();

		//add all recipe items (authors, ingredients, etc. to the database if they aren't already
		IngredientList::iterator ing_list_end( ( *recipe_it ).ingList.end() );
		for ( IngredientList::iterator ing_it = ( *recipe_it ).ingList.begin(); ing_it != ing_list_end; ++ing_it ) {
			importIngredient( *ing_it, db, progress_dialog );

			for ( Ingredient::SubstitutesList::iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ++sub_it ) {
				importIngredient( *sub_it, db, progress_dialog );
			}
		}

		ElementList::iterator author_list_end( ( *recipe_it ).authorList.end() );
		for ( ElementList::iterator author_it = ( *recipe_it ).authorList.begin(); author_it != author_list_end; ++author_it ) {
			if ( direct ) {
				progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
				kapp->processEvents();
			}

			int new_author_id = db->findExistingAuthorByName(( *author_it ).name);
			if ( new_author_id == -1 && !( *author_it ).name.isEmpty() ) {
				new_author_id = db->createNewAuthor( ( *author_it ).name );
			}

			( *author_it ).id = new_author_id;
		}

		ElementList::iterator cat_list_end( ( *recipe_it ).categoryList.end() );
		for ( ElementList::iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != cat_list_end; ++cat_it ) {
			if ( direct ) {
				progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
				kapp->processEvents();
			}

			int new_cat_id = db->findExistingCategoryByName(( *cat_it ).name);
			if ( new_cat_id == -1 && !( *cat_it ).name.isEmpty() ) {
				new_cat_id = db->createNewCategory( ( *cat_it ).name );
			}

			( *cat_it ).id = new_cat_id;
		}

		if ( !(*recipe_it).yield.type().isEmpty() ) {
			int new_id = db->findExistingYieldTypeByName((*recipe_it).yield.type());
			if ( new_id == -1 ) {
				new_id = db->createNewYieldType( (*recipe_it).yield.type() );
			}
			(*recipe_it).yield.setTypeId(new_id);
		}

		RatingList::iterator rating_list_end( ( *recipe_it ).ratingList.end() );
		for ( RatingList::iterator rating_it = ( *recipe_it ).ratingList.begin(); rating_it != rating_list_end; ++rating_it ) {
			if ( direct ) {
				progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
				kapp->processEvents();
			}

			foreach (RatingCriteria rc, (*rating_it).ratingCriterias()) {
				int new_criteria_id = db->findExistingRatingByName(rc.name());
				if ( new_criteria_id == -1 && !rc.name().isEmpty() ) {
					new_criteria_id = db->createNewRating( rc.name() );
				}
	
				rc.setId( new_criteria_id );
			}
		}

		if ( overwrite )  //overwrite existing
			( *recipe_it ).recipeID = db->findExistingRecipeByName( ( *recipe_it ).title );
		else //rename
			( *recipe_it ).title = db->getUniqueRecipeTitle( ( *recipe_it ).title );

		if ( direct ) {
			progress_dialog->progressBar()->setValue(progress_dialog->progressBar()->value()+1);
			kapp->processEvents();
		}

		//save into the database
		db->saveRecipe( &( *recipe_it ) );

		recipe_it_old = recipe_it; //store to delete once we've got the next recipe
	}
}

void BaseImporter::setCategoryStructure( CategoryTree *cat_structure )
{
	if ( direct ) {
		importCategoryStructure( m_database, cat_structure );
	}
	else {
		delete m_cat_structure;
		m_cat_structure = cat_structure;
	}
}

void BaseImporter::importCategoryStructure( RecipeDB *db, const CategoryTree *categoryTree )
{
	for ( CategoryTree * child_it = categoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		int new_cat_id = db->findExistingCategoryByName( child_it->category.name );
		if ( new_cat_id == -1 ) {
			new_cat_id = db->createNewCategory( child_it->category.name, categoryTree->category.id );
		}

		child_it->category.id = new_cat_id;

		importCategoryStructure( db, child_it );
	}
}

void BaseImporter::setUnitRatioInfo( UnitRatioList &ratioList, UnitList &unitList )
{
	m_ratioList = ratioList;
	m_unitList = unitList;
}

void BaseImporter::importUnitRatios( RecipeDB *db )
{
	for ( UnitRatioList::const_iterator it = m_ratioList.constBegin(); it != m_ratioList.constEnd(); ++it ) {
		QString unitName1, unitName2;
		for ( UnitList::const_iterator unit_it = m_unitList.constBegin(); unit_it != m_unitList.constEnd(); ++unit_it ) {
			if ( ( *it ).unitId1() == ( *unit_it ).id() ) {
				unitName1 = ( *unit_it ).name();
				if ( !unitName2.isEmpty() )
					break;
			}
			else if ( ( *it ).unitId2() == ( *unit_it ).id() ) {
				unitName2 = ( *unit_it ).name();
				if ( !unitName1.isEmpty() )
					break;
			}
		}

		int unitId1 = db->findExistingUnitByName( unitName1 );
		int unitId2 = db->findExistingUnitByName( unitName2 );

		//the unit needed for the ratio may not have been added, because the
		//recipes chosen did not include the unit
		if ( unitId1 != -1 && unitId2 != -1 ) {
			UnitRatio ratio(unitId1, unitId2, ( *it ).ratio());
			db->saveUnitRatio( &ratio );
		}
	}
}

void BaseImporter::processMessages( const QString &file )
{
	if ( m_error_msgs.count() > 0 ) {
		//<!doc> ensures it is detected as RichText
		m_master_error += i18n( "<!doc>Import of recipes " //krazy:exclude=i18ncheckarg
		"from the file <b>\"%1\"</b> <b>failed</b> due to the following error(s):" , file );
		m_master_error += "<ul><li>" + m_error_msgs.join( "</li><li>" ) + "</li></ul>";

		m_error_msgs.clear();
	}
	else if ( m_warning_msgs.count() > 0 ) {
		m_master_warning += i18n( "The file <b>%1</b> generated the following warning(s):" ,file );
		m_master_warning += "<ul><li>" + m_warning_msgs.join( "</li><li>" ) + "</li></ul>";

		m_warning_msgs.clear();
	}
}

