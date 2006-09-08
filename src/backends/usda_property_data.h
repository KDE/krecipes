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

#ifndef USDA_PROPERTY_DATA_H
#define USDA_PROPERTY_DATA_H

#include <qvaluelist.h>

#define TOTAL_USDA_PROPERTIES 43

namespace USDA {
	typedef struct PropertyData
	{
		int id;
		QString name;
		QString unit;
	};

	QValueList<PropertyData> loadProperties();
}

#endif //USDA_PROPERTY_DATA_H
