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

#include <qvaluelist.h>
#include <qstring.h>
#include <qstringlist.h>

class Recipe;
class RecipeDB;

/** @brief Subclass this class to create an importer for a specific file type.
  *
  * Subclasses should take the file name of the file to import in their constructor
  * and then parse the file.  For every recipe found in the file, a Recipe pointer should
  * be created and added to the importer using the @ref add() function.
  *
  * @author Jason Kivlighn
  */
class BaseImporter
{
public:
	BaseImporter();
	~BaseImporter();

	QString getErrorMsg(){ return m_error_msg; }
	QStringList getWarningMsgs(){ return m_warning_msgs; }

	/** Import the all recipes into the given database.  These recipes are the
	  * recipes added to this class by a subclass using the @ref add() method.
	  */
	void import( RecipeDB *db );

protected:
	/** Add a recipe to be imported into the database */
	void add( Recipe *recipe ){ m_recipe_list->append( recipe ); }

	void setErrorMsg( const QString & s ){ m_error_msg += s + "\n"; }
	void addWarningMsg( const QString & s ){ m_warning_msgs.append(s); }

private:
	QValueList<Recipe*> *m_recipe_list;
	QString m_error_msg;
	QStringList m_warning_msgs;
};

#endif //BASEIMPORTER_H
