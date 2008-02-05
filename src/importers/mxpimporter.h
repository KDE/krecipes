/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef MXPIMPORTER_H
#define MXPIMPORTER_H

#include <qstring.h>
#include <Q3TextStream>

#include "baseimporter.h"

/** Class to import MasterCook's MXP (MasterCook Export) file format.
  * This is a human-readable format used in Mastercook up until version 4.
  * @author Jason Kivlighn
  */
class MXPImporter : public BaseImporter
{
public:
	MXPImporter();
	virtual ~MXPImporter();

protected:
	void parseFile( const QString& filename );

private:
	void importMXP( Q3TextStream &stream );

	void loadCategories( Q3TextStream &stream, Recipe &recipe );
	void loadIngredients( Q3TextStream &stream, Recipe &recipe );
	void loadInstructions( Q3TextStream &stream, Recipe &recipe );
	void loadOptionalFields( Q3TextStream &stream, Recipe &recipe );

	void importMac( Q3TextStream &stream );
	void importGeneric( Q3TextStream &stream );

	QString getNextQuotedString( Q3TextStream &stream );
};

#endif //MXPIMPORTER_H
