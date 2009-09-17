/***************************************************************************
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEMLEXPORTER_H
#define RECIPEMLEXPORTER_H

#include "baseexporter.h"

#include <qdom.h>

class IngredientData;

/**
  * Export class for the RecipeML file format <http://www.formatdata.com/recipeml>
  * @author Jason Kivlighn
  *
  * Note: This format does not handle all the properties of recipes.
  *       Data lost in export to this format include:
  *           -Recipe photo
  */
class RecipeMLExporter : public BaseExporter
{
public:
	RecipeMLExporter( const QString&, const QString& );
	virtual ~RecipeMLExporter();

	virtual int supportedItems() const;

protected:
	virtual QString createContent( const RecipeList& );
	virtual QString createHeader( const RecipeList& );
	virtual QString createFooter();

private:
	void createIngredient( QDomElement &ing_tag, const IngredientData &, QDomDocument &doc );
};

#endif //RECIPEMLEXPORTER_H
