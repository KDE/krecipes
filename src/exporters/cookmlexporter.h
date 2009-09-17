/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef COOKMLEXPORTER_H
#define COOKMLEXPORTER_H

#include "baseexporter.h"

/**
  * Export class for the Meal-Master file format
  * @author Jason Kivlighn
  *
  * Note: This format does not handle all the properties of recipes.
  *       Data lost in export to this format include:
  *          ---none?---
  */
class CookMLExporter : public BaseExporter
{
public:
	CookMLExporter( const QString&, const QString& );
	virtual ~CookMLExporter();

	virtual int supportedItems() const;

protected:
	virtual QString createContent( const RecipeList& );
	virtual QString createHeader( const RecipeList& );
	virtual QString createFooter();
};

#endif //COOKMLEXPORTER_H
