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
    Ingredient(const Ingredient& i);
    Ingredient( const QString &name, double amount, const QString &units, int unitID = -1, int ingredientID = -1, const QString &prepMethod = QString::null, int prepMethodID = -1 );
    ~Ingredient();
    int ingredientID;
    QString name;
    double amount;
    int unitID;
    QString units;
    QString prepMethod;
    int prepMethodID;
    int groupID;
    QString group;
    Ingredient & operator=(const Ingredient &i);
    
    /** Compare two elements by their id */
    bool operator==(const Ingredient &) const;
    
    /** This is used for sorting, and so we compare by name */
    bool operator<(const Ingredient &) const;
};

#endif
