/*
Copyright (C) 2003 by
   Richard Lärkäng
   Jason Kivlighn <mizunoami44@users.sourceforge.net>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef RECIPEMLIMPORTER_H
#define RECIPEMLIMPORTER_H

#include "baseimporter.h"
#include "ingredient.h"

#include <qdom.h>

class Recipe;

class RecipeMLImporter : public BaseImporter
{
public:
	RecipeMLImporter(const QString& filename);
	~RecipeMLImporter();

private:
	void readRecipemlRecipe(const QDomElement& recipe);
	void readRecipemlHead(const QDomElement& head);
	void readRecipemlIngs(const QDomElement& ings);
	void readRecipemlIng(const QDomElement& ing);
	void readRecipemlDirections(const QDomElement& dirs);
	Recipe* recipe;
};

#endif //RECIPEMLIMPORTER_H
