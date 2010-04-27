/***************************************************************************
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
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
#include <kstandarddirs.h>

#include <QFile>
#include <QString>
//Added by qt3to4:
#include <Q3ValueList>
#include <QTextStream>

namespace USDA {

Q3ValueList<PropertyData> loadProperties()
{
	Q3ValueList<PropertyData> result;

	QString dataFilename = KStandardDirs::locate( "appdata", "data/property-data-" + KGlobal::locale() ->language() + ".txt" );
	if ( dataFilename.isEmpty() ) {
		kDebug() << "No localized property data available for " << KGlobal::locale() ->language() ;

		dataFilename = KStandardDirs::locate( "appdata", "data/property-data-en_US.txt" ); //default to English
	}

	QFile dataFile( dataFilename );
	if ( dataFile.open( QIODevice::ReadOnly ) ) {
		kDebug() << "Loading: " << dataFilename ;
		QTextStream stream( &dataFile );

		QString line;
		while ( (line = stream.readLine()) != QString() ) {
			if ( line.trimmed().isEmpty() ) continue;

			PropertyData data;
			int sepIndex = line.indexOf(':');
			data.name = line.left(sepIndex);
			data.unit = line.right(line.length()-sepIndex-1);

			result << data;
		}

		dataFile.close();
	}
	else
		kDebug() << "Unable to find or open property data file (property-data-en_US.sql)" ;

	return result;
}

} //namespace USDA
