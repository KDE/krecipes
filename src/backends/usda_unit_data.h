/***************************************************************************
*   Copyright (C) 2006                                                    *
*                                                                         *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef USDA_UNIT_DATA_H
#define USDA_UNIT_DATA_H

#include <klocale.h>

#include <qstring.h>
#include <qvaluelist.h>

namespace USDA {

typedef struct UnitData {
	QString name;
	QString plural;
	QString translation;
	QString translationPlural;
};

typedef struct PrepData {
	QString name;
	QString translation;
};

typedef QValueList<UnitData> UnitDataList;
typedef QValueList<PrepData> PrepDataList;

UnitDataList loadUnits();
PrepDataList loadPrepMethods();

bool parseUnitAndPrep( const QString &string, QString &unit, QString &prep, const UnitDataList &, const PrepDataList & );

};

#endif //USDA_UNIT_DATA_H
