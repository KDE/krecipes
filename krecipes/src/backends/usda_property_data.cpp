/***************************************************************************
*   Copyright (C) 2003-2006                                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   Copyright (C) 2003                                                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "usda_property_data.h"

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>

#include <qfile.h>
#include <qstring.h>

namespace USDA {

QValueList<PropertyData> loadProperties()
{
	QValueList<PropertyData> result;

	QString dataFilename = locate( "appdata", "data/property-data-" + KGlobal::locale() ->language() + ".txt" );
	if ( dataFilename.isEmpty() ) {
		kdDebug() << "No localized property data available for " << KGlobal::locale() ->language() << endl;

		dataFilename = locate( "appdata", "data/property-data-en_US.txt" ); //default to English
	}

	QFile dataFile( dataFilename );
	if ( dataFile.open( IO_ReadOnly ) ) {
		kdDebug() << "Loading: " << dataFilename << endl;
		QTextStream stream( &dataFile );

		QString line;
		while ( (line = stream.readLine()) != QString::null ) {
			if ( line.stripWhiteSpace().isEmpty() ) continue;

			PropertyData data;
			int sepIndex = line.find(':');
			data.name = line.left(sepIndex);
			data.unit = line.right(line.length()-sepIndex-1);

			result << data;
		}

		dataFile.close();
	}
	else
		kdDebug() << "Unable to find or open property data file (property-data-en_US.sql)" << endl;

	return result;
}

} //namespace USDA
