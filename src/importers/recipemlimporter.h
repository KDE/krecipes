/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Richard L�rk�ng                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef RECIPEMLIMPORTER_H
#define RECIPEMLIMPORTER_H

#include "baseimporter.h"
#include "ingredient.h"
#include "recipe.h"

#include <qdom.h>

class RecipeMLImporter : public BaseImporter
{
public:
	RecipeMLImporter();
	virtual ~RecipeMLImporter();

protected:
	void parseFile(const QString& filename);

private:
	void readRecipemlDirections(const QDomElement& dirs);
	void readRecipemlHead(const QDomElement& head);
	void readRecipemlIng(const QDomElement& ing);
	void readRecipemlIngs(const QDomElement& ings);
	void readRecipemlMenu(const QDomElement& menu);
	void readRecipemlSrcItems(const QDomElement& sources);
	void readRecipemlRecipe(const QDomElement& recipe);

	Recipe recipe;
};

#endif //RECIPEMLIMPORTER_H
