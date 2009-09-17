/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef MXPIMPORTER_H
#define MXPIMPORTER_H

#include <QString>
#include <QTextStream>

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
	void importMXP( QTextStream &stream );

	void loadCategories( QTextStream &stream, Recipe &recipe );
	void loadIngredients( QTextStream &stream, Recipe &recipe );
	void loadInstructions( QTextStream &stream, Recipe &recipe );
	void loadOptionalFields( QTextStream &stream, Recipe &recipe );

	void importMac( QTextStream &stream );
	void importGeneric( QTextStream &stream );

	QString getNextQuotedString( QTextStream &stream );
};

#endif //MXPIMPORTER_H
