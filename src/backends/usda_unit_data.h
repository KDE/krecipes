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

struct unit_data {
	const char *name;
	const char *plural;
};

static unit_data unit_data_list[] = {
            {"bag","bags"},
            {"block","blocks"},
            {"bottle","bottles"},
            {"box","boxes"},
            {"bunch","bunches"},
            {"can","cans"},
            {"cone","cones"},
            {"container","containers"},
            {"cube","cubes"},
            {"cup","cups"},
            {"fl oz","fl oz"},
            {"glass","glasses"},
            {"item","items"},
            {"loaf","loaves"},
            {"large","large"},
            {"lb","lbs"},
            {"junior","junior"},
            {"leaf","leaves"},
            {"medium","medium"},
            {"oz","oz"},
            {"pack","packs"},
            {"package","packages"},
            {"packet","packets"},
            {"piece","pieces"},
            {"pouch","pouches"},
            {"quart","quarts"},
            {"scoop","scoops"},
            {"sheet","sheets"},
            {"slice","slices"},
            {"small","small"},
            {"spear","spears"},
            {"sprout","spouts"},
            {"sprig","sprigs"},
            {"square","squares"},
            {"stalk","stalks"},
            {"stem","stems"},
            {"strip","strips"},
            {"tablespoon","tablespoons"},
            {"tbsp","tbsp"},
            {"teaspoon","teaspoons"},
            {"tsp","tsp"},
            {"tube","tubes"},
            {"unit","units"},
            {0,0}
};

static const char * prep_data_list[] = {
            "chopped",
            "diced",
            "sliced",
            "crumbled",
            "crushed",
            "ground",
            "grated",
            "mashed",
            "melted",
            "packed",
            "pureed",
            "quartered",
            "thawed",
            "shredded",
            "sifted",
            "pared",
            "flaked",
            "unpacked",
            "unsifted",
            "unthawed",
            "pitted",
            "peeled",
            "cooked",
            "hulled",
            "shelled",
            "raw",
            "whipped",
            0
};

bool parseUSDAUnitAndPrep( const QString &string, QString &unit, QString &prep );

#endif //USDA_UNIT_DATA_H
