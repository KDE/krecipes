/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef MMDATA_H
#define MMDATA_H

/** This file contains each of the Meal-Master unit abbreviations,
  * and their cooresponding expansions.
  */

struct expand_unit_info
{
	const char *short_form;
	const char *expanded_form;
	const char *plural_expanded_form;
};

static expand_unit_info unit_info[] = {
                                          {"bn", I18N_NOOP( "bunch" ), I18N_NOOP( "bunches" ) },
                                          {"c" , I18N_NOOP( "cup" ), I18N_NOOP( "cups" ) },
                                          {"cc", I18N_NOOP( "cubic cm" ), I18N_NOOP( "cubic cm" ) },
                                          {"cg", I18N_NOOP( "centigram" ), I18N_NOOP( "centigrams" ) },
                                          {"cl", I18N_NOOP( "centiliter" ), I18N_NOOP( "centiliters" ) },
                                          {"cn", I18N_NOOP( "can" ), I18N_NOOP( "cans" ) },
                                          {"ct", I18N_NOOP( "carton" ), I18N_NOOP( "cartons" ) },
                                          {"dg", I18N_NOOP( "decigram" ), I18N_NOOP( "decigrams" ) },
                                          {"dl", I18N_NOOP( "deciliter" ), I18N_NOOP( "deciliters" ) },
                                          {"dr", I18N_NOOP( "drop" ), I18N_NOOP( "drops" ) },
                                          {"ds", I18N_NOOP( "dash" ), I18N_NOOP( "dashes" ) },
                                          {"ea", I18N_NOOP( "each" ), I18N_NOOP( "each" ) },
                                          {"kg", I18N_NOOP( "kilogram" ), I18N_NOOP( "kilograms" ) },
                                          {"fl", I18N_NOOP( "fluid ounce" ), I18N_NOOP( "fluid ounces" ) },
                                          {"g" , I18N_NOOP( "gram" ), I18N_NOOP( "grams" ) },
                                          {"ga", I18N_NOOP( "gallon" ), I18N_NOOP( "gallons" ) },
                                          {"l" , I18N_NOOP( "liter" ), I18N_NOOP( "liters" ) },
                                          {"lb", I18N_NOOP( "pound" ), I18N_NOOP( "pounds" ) },
                                          {"lg", I18N_NOOP( "large" ), I18N_NOOP( "large" ) },
                                          {"md", I18N_NOOP( "medium" ), I18N_NOOP( "medium" ) },
                                          {"mg", I18N_NOOP( "milligram" ), I18N_NOOP( "milligrams" ) },
                                          {"ml", I18N_NOOP( "milliliter" ), I18N_NOOP( "milliliters" ) },
                                          {"pg", I18N_NOOP( "package" ), I18N_NOOP( "packages" ) },
                                          {"pk", I18N_NOOP( "package" ), I18N_NOOP( "packages" ) },
                                          {"pn", I18N_NOOP( "pinch" ), I18N_NOOP( "pinches" ) },
                                          {"pt", I18N_NOOP( "pint" ), I18N_NOOP( "pints" ) },
                                          {"oz", I18N_NOOP( "ounce" ), I18N_NOOP( "ounces" ) },
                                          {"qt", I18N_NOOP( "quart" ), I18N_NOOP( "quarts" ) },
                                          {"sl", I18N_NOOP( "slice" ), I18N_NOOP( "slices" ) },
                                          {"sm", I18N_NOOP( "small" ), I18N_NOOP( "small" ) },
                                          {"t" , I18N_NOOP( "teaspoon" ), I18N_NOOP( "teaspoons" ) },
                                          {"tb", I18N_NOOP( "tablespoon" ), I18N_NOOP( "tablespoons" ) },
                                          {"ts", I18N_NOOP( "teaspoon" ), I18N_NOOP( "teaspoons" ) },
                                          {"T" , I18N_NOOP( "tablespoon" ), I18N_NOOP( "tablespoons" ) },
                                          {"x" , I18N_NOOP( "per serving" ), I18N_NOOP( "per serving" ) },
                                          {"", "", ""},
                                          { 0, 0, 0 }
                                      };

#endif //MMDATA_H
