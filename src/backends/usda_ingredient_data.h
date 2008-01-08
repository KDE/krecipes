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

#ifndef USDA_INGREDIENT_DATA_H
#define USDA_INGREDIENT_DATA_H

#include <q3valuelist.h>

namespace USDA {
	typedef struct IngredientData
	{
		QString name;
		int usda_id;
	};

	bool localizedIngredientsAvailable();
	Q3ValueList<IngredientData> loadIngredients();
}

#endif //USDA_INGREDIENT_DATA_H
