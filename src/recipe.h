/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef RECIPE_H
#define RECIPE_H

#include <qstring.h>
#include <qpixmap.h>
#include <qdatetime.h>

#include "ingredientlist.h"
#include "elementlist.h"

/**
@author Unai Garro
*/
class Recipe
{
public:
	Recipe();
	~Recipe();
	// Public variables

	int recipeID;
	int persons;
	QString title;
	QString instructions;
	QPixmap photo;
	IngredientList ingList;
	ElementList categoryList; // id+name
	ElementList authorList; //authors' id+name
	QTime prepTime;

	// Public methods
	void empty( void );



};

#endif
