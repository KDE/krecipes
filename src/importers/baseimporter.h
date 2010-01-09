/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef BASEIMPORTER_H
#define BASEIMPORTER_H

#include <klocale.h>

#include <QString>
#include <QStringList>

#include "datablocks/recipelist.h"
#include "datablocks/elementlist.h"
#include "datablocks/unitratiolist.h"

class Recipe;
class RecipeDB;
class CategoryTree;
class IngredientData;

class KProgressDialog;

/** @brief Subclass this class to create an importer for a specific file type.
  *
  * Subclasses should take the file name of the file to import in their constructor
  * and then parse the file.  For every recipe found in the file, a Recipe object should
  * be created and added to the importer using the @ref add() function.
  *
  * @author Jason Kivlighn
  */
class BaseImporter
{
public:
	BaseImporter();
	virtual ~BaseImporter();

	QString getMessages() const
	{
		return m_master_error + m_master_warning;
	}
	QString getErrorMsg() const
	{
		return m_master_error;
	}
	QString getWarningMsg() const
	{
		return m_master_warning;
	}

	void parseFiles( const QStringList &filenames );

	/** Import all the recipes into the given database.  These recipes are the
	  * recipes added to this class by a subclass using the @ref add() method.
	  */
	void import( RecipeDB *db, bool automatic = false );

	RecipeList recipeList() const { return *m_recipe_list; }
	void setRecipeList( const RecipeList &list ) { *m_recipe_list = list; }

	const CategoryTree *categoryStructure() const { return m_cat_structure; }

protected:
	virtual void parseFile( const QString &filename ) = 0;
	
	void importRecipes( RecipeList &selected_recipes, RecipeDB *db, KProgressDialog *progess_dialog );

	/** Add a recipe to be imported into the database */
	void add( const Recipe &recipe );
	void add( const RecipeList &recipe_list );

	void setCategoryStructure( CategoryTree *cat_structure );
	void setUnitRatioInfo( UnitRatioList &ratioList, UnitList &unitList );

	int totalCount() const
	{
		return m_recipe_list->count();
	}
	int fileRecipeCount() const
	{
		return file_recipe_count;
	}

	void setErrorMsg( const QString & s )
	{
		m_error_msgs.append( s );
	}
	void addWarningMsg( const QString & s )
	{
		m_warning_msgs.append( s );
	}

private:
	void importCategoryStructure( RecipeDB *, const CategoryTree * );
	void importUnitRatios( RecipeDB * );
	void importIngredient( IngredientData &ing, RecipeDB *db, KProgressDialog *progress_dialog );

	void processMessages( const QString &file );

	RecipeList *m_recipe_list;
	CategoryTree *m_cat_structure;
	UnitRatioList m_ratioList;
	UnitList m_unitList;

	QStringList m_warning_msgs;
	QStringList m_error_msgs;
	QString m_master_warning;
	QString m_master_error;

	int file_recipe_count;
	bool direct;

	RecipeDB *m_database;
	KProgressDialog *m_progress_dialog;
	QStringList m_filenames;
};

#endif //BASEIMPORTER_H
