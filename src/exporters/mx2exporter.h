/***************************************************************************
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef MX2EXPORTER_H
#define MX2EXPORTER_H

#include "baseexporter.h"

#include <qdom.h>

class IngredientData;

/**
  * Export class for the mx2 file format (see MasterCook mx2.dtd)
  * @author Jason Kivlighn
  * portions Warren Severin
  *
  * Note: This format does not handle all the properties of recipes.
  *       Data lost in export to this format include:
  *           -Recipe photo
  */
class MX2Exporter : public BaseExporter
{
public:
	MX2Exporter( const QString&, const QString& );
	virtual ~MX2Exporter();

	virtual int supportedItems() const;

protected:
	virtual QString createContent( const RecipeList& );
	virtual QString createHeader( const RecipeList& );
	virtual QString createFooter();

};

#endif //MX2EXPORTER_H
