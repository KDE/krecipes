/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef INGREDIENT_H
#define INGREDIENT_H

#include <qstring.h>

/**
@author Unai Garro
*/
class Ingredient{
public:
    Ingredient();
    Ingredient( const QString &name, double amount, const QString &units, int unitID = -1, int ingredientID = -1 );
    Ingredient(const Ingredient &ing);
    ~Ingredient();
    int ingredientID;
    QString name;
    double amount;
    int unitID;
    QString units;
    Ingredient& operator=(const Ingredient &ing);

    /** Compare two elements by their id */
    bool operator==(const Ingredient &);
};

#endif
