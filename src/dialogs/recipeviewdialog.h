/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RECIPEVIEWDIALOG_H
#define RECIPEVIEWDIALOG_H

#include <qvbox.h>
#include <qstring.h>

class RecipeDB;
class Recipe;
class KHTMLPart;
class QPushButton;

/**
@author Unai Garro
*/

class RecipeViewDialog : public QVBox
{
Q_OBJECT

public:
    RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID=-1);

    ~RecipeViewDialog();

    /** @return Boolean indicating whether or not the recipe was successfully loaded */
    bool loadRecipe(int recipeID);

    /** @return Boolean indicating whether or not the recipes were successfully loaded */
    bool loadRecipes( const QValueList<int> &ids );
    
    int recipesLoaded() const { return ids_loaded.count(); }
    const QValueList<int> currentRecipes() const { return ids_loaded; }
    
    void reload();

private:

  // Internal Variables
  KHTMLPart *recipeView;
  RecipeDB  *database;
  bool recipe_loaded;
  QValueList<int> ids_loaded;
  QString tmp_filename;

  // Internal Methods
  bool showRecipes( const QValueList<int> &ids );
  void removeOldFiles();

public slots:
	void print(void);

};


#endif
