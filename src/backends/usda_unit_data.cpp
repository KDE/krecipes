/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "usda_unit_data.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

namespace USDA {

UnitDataList loadUnits()
{
	UnitDataList result;

	QString dataFilename = KStandardDirs::locate( "appdata", "data/unit-data-" + KGlobal::locale() ->language() + ".txt" );
	if ( dataFilename.isEmpty() ) {
		kDebug() << "No localized unit data available for " << KGlobal::locale() ->language() ;

		dataFilename = KStandardDirs::locate( "appdata", "data/unit-data-en_US.txt" ); //default to English
	}

	QFile dataFile( dataFilename );
	if ( dataFile.open( QIODevice::ReadOnly ) ) {
		kDebug() << "Loading: " << dataFilename ;
		QTextStream stream( &dataFile );

		QString line;
		while ( (line = stream.readLine()) != QString() ) {
			if ( line.trimmed().isEmpty() ) continue;

			QStringList parts = line.split(':' , QString::KeepEmptyParts);

			UnitData data;
			data.name = parts[0];
			data.plural = parts[1];
			data.translation = parts[2];
			data.translationPlural = parts[3];

			if ( data.translation.isEmpty() )
				data.translation = data.name;
			if ( data.translationPlural.isEmpty() )
				data.translationPlural = data.plural;

			result << data;
		}

		dataFile.close();
	}
	else
		kDebug() << "Unable to find or open unit data file (unit-data-en_US.sql)" ;

	return result;
}

PrepDataList loadPrepMethods()
{
	PrepDataList result;

	QString dataFilename = KStandardDirs::locate( "appdata", "data/prep-data-" + KGlobal::locale() ->language() + ".txt" );
	if ( dataFilename.isEmpty() ) {
		kDebug() << "No localized prep data available for " << KGlobal::locale() ->language() ;

		dataFilename = KStandardDirs::locate( "appdata", "data/prep-data-en_US.txt" ); //default to English
	}

	QFile dataFile( dataFilename );
	if ( dataFile.open( QIODevice::ReadOnly ) ) {
		kDebug() << "Loading: " << dataFilename ;
		QTextStream stream( &dataFile );

		QString line;
		while ( (line = stream.readLine()) != QString() ) {
			if ( line.trimmed().isEmpty() ) continue;

			PrepData data;
			int sepIndex = line.indexOf(':');
			data.name = line.left(sepIndex);
			data.translation = line.right(line.length()-sepIndex-1);
			if ( data.translation.isEmpty() )
				data.translation = data.name;

			result << data;
		}

		dataFile.close();
	}
	else
		kDebug() << "Unable to find or open prep data file (prep-data-en_US.sql)" ;

	return result;
}

bool parseUnitAndPrep( const QString &string, QString &unit, QString &prep, const UnitDataList &unit_data_list, const PrepDataList &prep_data_list )
{
	int commaIndex = string.indexOf(",");
	QString unitPart = string.left(commaIndex);
	QString prepPart = string.right(string.length()-commaIndex-2).trimmed();

	QString localizedUnit;
	QString localizedPrep;

	bool acceptable = false;
	for ( UnitDataList::const_iterator it = unit_data_list.begin(); it != unit_data_list.end(); ++it ) {
		if ( unitPart == (*it).name ) {
			localizedUnit = (*it).translation;
			acceptable = true;
		}
		else if ( unitPart == (*it).plural ) {
			localizedUnit = (*it).translationPlural;
			acceptable = true;
		}
	}
	if ( !acceptable )
		return false;

	acceptable = false;
	if ( prepPart.isEmpty() )
		acceptable = true;
	else {
		for ( PrepDataList::const_iterator it = prep_data_list.begin(); it != prep_data_list.end(); ++it ) {
			if ( prepPart == (*it).name ) {
				localizedPrep = (*it).translation;
				acceptable = true;
			}
		}
	}
	if ( !acceptable )
		prepPart.clear();

	unit = localizedUnit;
	prep = localizedPrep;
	return true;
}

}
