/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BASEIMPORTER_H
#define BASEIMPORTER_H

#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>

class Recipe;
class RecipeDB;
class DualProgressDialog;

/** @briefSubclass this class to create an importer for a specific file type.
  *
  * Subclasses should take the file name of the file to import in their constructor
  * and then parse the file.  For every recipe found in the file, a Recipe pointer should
  * be created and added to the importer using the @ref add() function.
  * @author Jason Kivlighn
  */
class BaseImporter
{
public:
	BaseImporter();
	~BaseImporter();

	enum ImportError { ImportSuccessful = 0, FileOpenError, InvalidFile };
	int getImportError(){ return error_code; }
	QString getErrorMsg(){ return m_error_msg; }
	QStringList getWarningMsgs(){ return m_warning_msgs; }

	/** Import the all recipes into the given database.  These recipes are the
	  * recipes added to this class by a subclass using the @ref add() method.
	  */
	void import( RecipeDB *db, DualProgressDialog *progress_dialog = 0 );

protected:
	/** Add a recipe to be imported into the database */
	void add( Recipe *recipe ){ m_recipe_list->append( recipe ); }

	int error_code;
	void setErrorMsg( const QString & s ){ m_error_msg += s + "\n"; }
	void addWarningMsg( const QString & s ){ m_warning_msgs.append(s); }

private:
	QPtrList<Recipe> *m_recipe_list;
	QString m_error_msg;
	QStringList m_warning_msgs;
};

#endif //BASEIMPORTER_H
