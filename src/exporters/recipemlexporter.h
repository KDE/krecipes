/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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

protected:
	virtual QString createContent( const RecipeList& );
	virtual QString createHeader( const RecipeList& );
	virtual QString createFooter();
};

#endif //RECIPEMLEXPORTER_H
