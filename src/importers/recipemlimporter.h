/***************************************************************************
*   Copyright © 2003 Richard Lärkäng                                      *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/


#ifndef RECIPEMLIMPORTER_H
#define RECIPEMLIMPORTER_H

#include "baseimporter.h"
#include "datablocks/ingredient.h"
#include "datablocks/recipe.h"

#include <qdom.h>

/** Class to import the RecipeML, XML-based file format.
  * More info at http://www.formatdata.com/recipeml
  *
  * @author Jason Kivlighn
  */
class RecipeMLImporter : public BaseImporter
{
public:
	RecipeMLImporter();
	virtual ~RecipeMLImporter();

protected:
	void parseFile( const QString& filename );

private:
	void readRecipemlDirections( const QDomElement& dirs );
	void readRecipemlHead( const QDomElement& head );
	void readRecipemlIng( const QDomElement& ing, Ingredient *ing_parent = 0, const QString &header = QString() );
	void readRecipemlIngs( const QDomElement& ings );
	void readRecipemlMenu( const QDomElement& menu );
	void readRecipemlSrcItems( const QDomElement& sources );
	void readRecipemlRecipe( const QDomElement& recipe );
	void readRecipemlPreptime( const QDomElement &preptime );
	void readRecipemlQty( const QDomElement &qty, Ingredient &ing );
	void readRecipemlRange( const QDomElement& range, double &amount, double &amount_offset );

	Recipe recipe;
};

#endif //RECIPEMLIMPORTER_H
