/***************************************************************************
*   Copyright (C) 2003                                                    *
*                                                                         *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef USDA_PROPERTY_DATA_H
#define USDA_PROPERTY_DATA_H

#include <klocale.h>

struct property_data
{
	int id;
	QString name;
	const char *unit;
};

#define TOTAL_USDA_PROPERTIES 43

//NOTE: the following must be in this order
static property_data property_data_list[] = {
            { -1, I18N_NOOP( "water" ), "g"},
            { -1, I18N_NOOP( "energy" ), "kcal"},
            { -1, I18N_NOOP( "protein" ), "g"},
            { -1, I18N_NOOP( "fat" ), "g"},
            { -1, I18N_NOOP( "ash" ), "g"},
            { -1, I18N_NOOP( "carbohydrates" ), "g"},
            { -1, I18N_NOOP( "dietary fiber" ), "g"},
            { -1, I18N_NOOP( "sugar" ), "g"},
            { -1, I18N_NOOP( "calcuim" ), "mg"},
            { -1, I18N_NOOP( "iron" ), "mg"},
            { -1, I18N_NOOP( "magnesium" ), "mg"},
            { -1, I18N_NOOP( "phosphorus" ), "mg"},
            { -1, I18N_NOOP( "potassium" ), "mg"},
            { -1, I18N_NOOP( "sodium" ), "mg"},
            { -1, I18N_NOOP( "zinc" ), "mg"},
            { -1, I18N_NOOP( "copper" ), "mg"},
            { -1, I18N_NOOP( "manganese" ), "mg"},
            { -1, I18N_NOOP( "selenium" ), "g"},
            { -1, I18N_NOOP( "vitamin C" ), "mg"},
            { -1, I18N_NOOP( "thiamin" ), "mg"},
            { -1, I18N_NOOP( "riboflavin" ), "mg"},
            { -1, I18N_NOOP( "niacin" ), "mg"},
            { -1, I18N_NOOP( "pantothenic acid" ), "mg"},
            { -1, I18N_NOOP( "vitamin B" ), "mg"},
            { -1, I18N_NOOP( "folate" ), "g"},
            { -1, I18N_NOOP( "folic acid" ), "g"},
            { -1, I18N_NOOP( "food folate" ), "g"},
            { -1, I18N_NOOP( "folate (DFE)" ), "g"},
            { -1, I18N_NOOP( "vitamin B12" ), "g"},
            { -1, I18N_NOOP( "vitamin A" ), "IU"},
            { -1, I18N_NOOP( "vitamin A (RAE)" ), "mg"},
            { -1, I18N_NOOP( "retinol" ), "g"},
            { -1, I18N_NOOP( "vitamin E" ), "g"},
            { -1, I18N_NOOP( "vitamin K" ), "g"},
            { -1, I18N_NOOP( "alpha-carotene" ), "g"},
            { -1, I18N_NOOP( "beta-carotene" ), "g"},
            { -1, I18N_NOOP( "beta-cryptoxanthin" ), "g"},
            { -1, I18N_NOOP( "lycopene" ), "g"},
            { -1, I18N_NOOP( "lutein+zeazanthin" ), "g"},
            { -1, I18N_NOOP( "saturated fat" ), "g"},
            { -1, I18N_NOOP( "monounsaturated fat" ), "g"},
            { -1, I18N_NOOP( "polyunsaturated fat" ), "g"},
            { -1, I18N_NOOP( "cholesterol" ), "mg"},
            { 0, 0, 0 }
        };

#endif //USDA_PROPERTY_DATA_H
