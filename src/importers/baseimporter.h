/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#ifndef BASEIMPORTER_H
#define BASEIMPORTER_H

#include <klocale.h>

#include <qstring.h>
#include <qstringlist.h>

#include "datablocks/recipelist.h"

class Recipe;
class RecipeDB;
class CategoryTree;
class CustomVector;

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

	QString getMessages() const{ return m_master_error + m_master_warning; }
	QString getErrorMsg() const{ return m_master_error; }
	QString getWarningMsg() const{ return m_master_warning; }

	void parseFiles( const QStringList &filenames );

	/** Import all the recipes into the given database.  These recipes are the
	  * recipes added to this class by a subclass using the @ref add() method.
	  */
	void import( RecipeDB *db, bool automatic=false );

protected:
	virtual void parseFile( const QString &filename ) = 0;

	/** Add a recipe to be imported into the database */
	void add( const Recipe &recipe ){ file_recipe_count++; m_recipe_list->append( recipe ); }
	void add( const RecipeList &recipe_list );

	void setCategoryStructure( CategoryTree *cat_structure );
	
	int totalCount() const { return m_recipe_list->count(); }
	int fileRecipeCount() const { return file_recipe_count; }

	void setErrorMsg( const QString & s ){ m_error_msgs.append(s); }
	void addWarningMsg( const QString & s ){ m_warning_msgs.append(s); }

private:
	void importCategoryStructure( RecipeDB *, CustomVector &, const CategoryTree * );

	RecipeList *m_recipe_list;
	CategoryTree *m_cat_structure;
	QStringList m_warning_msgs;
	QStringList m_error_msgs;
	QString m_master_warning;
	QString m_master_error;

	int file_recipe_count;
};

#endif //BASEIMPORTER_H
