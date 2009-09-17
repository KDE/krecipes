/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef REZKONVEXPORTER_H
#define REZKONVEXPORTER_H

#include "baseexporter.h"

/**
  * Export class for the Rezkonv file format
  * @author Jason Kivlighn
  *
  * Note: This format does not handle all the properties of recipes.
  *       Data lost in export to this format include:
  *           -Recipe photo
  *           -Authors
  *           -5 category maximum
  *           -Title is limited to 60 characters
  *           -Servings are limited to the range of 0-9999
  *           -Units are limited: If a given unit does not have a
  *            corresponding MM abbrev., otherwise it will be
  *            exported without a unit.
  */
class RezkonvExporter : public BaseExporter
{
public:
	RezkonvExporter( const QString&, const QString& );
	virtual ~RezkonvExporter();

	virtual int supportedItems() const;

protected:
	virtual QString createContent( const RecipeList & );

private:
	void writeHeader( QString &content, const Recipe &recipe );
	void writeIngredients( QString &content, const Recipe &recipe );
	void writeSingleIngredient( QString &content, const IngredientData &ing, bool is_sub = false );
	void writeDirections( QString &content, const Recipe &recipe );

	QStringList wrapText( const QString& str, int at ) const;
};

#endif //REZKONVEXPORTER_H
