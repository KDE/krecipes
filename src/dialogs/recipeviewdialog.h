/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEVIEWDIALOG_H
#define RECIPEVIEWDIALOG_H

#include <q3vbox.h>
#include <qstring.h>
//Added by qt3to4:
#include <Q3ValueList>

class RecipeDB;
class Recipe;
class KHTMLPart;

/**
@author Unai Garro
*/

class RecipeViewDialog : public Q3VBox
{
	Q_OBJECT

public:
	RecipeViewDialog( QWidget *parent, RecipeDB *db, int recipeID = -1 );

	~RecipeViewDialog();

	/** @return Boolean indicating whether or not the recipe was successfully loaded */
	bool loadRecipe( int recipeID );

	/** @return Boolean indicating whether or not the recipes were successfully loaded */
	bool loadRecipes( const Q3ValueList<int> &ids, const QString &layoutConfig = QString::null );

	int recipesLoaded() const
	{
		return ids_loaded.count();
	}
	const Q3ValueList<int> currentRecipes() const
	{
		return ids_loaded;
	}

	void reload( const QString &layoutConfig = QString::null );

public slots:
	void printNoPreview( void );
	void print( void );

private:

	// Internal Variables
	KHTMLPart *recipeView;
	RecipeDB *database;
	bool recipe_loaded;
	Q3ValueList<int> ids_loaded;
	QString tmp_filename;

	// Internal Methods
	bool showRecipes( const Q3ValueList<int> &ids, const QString &layoutConfig );
	void removeOldFiles();

private slots:
	void recipeRemoved(int);

};


#endif
