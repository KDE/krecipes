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

#ifndef USDA_PROPERTY_DATA_H
#define USDA_PROPERTY_DATA_H

#include <q3valuelist.h>

#define TOTAL_USDA_PROPERTIES 43

namespace USDA {
	struct PropertyData
	{
		int id;
		QString name;
		QString unit;
	};

	Q3ValueList<PropertyData> loadProperties();
}

#endif //USDA_PROPERTY_DATA_H
