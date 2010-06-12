/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#ifndef RECIPE_H
#define RECIPE_H

#include <QString>
#include <QPixmap>
#include <QDateTime>

#include "ingredientlist.h"
#include "datablocks/rating.h"
#include "datablocks/elementlist.h"
#include "datablocks/ingredientpropertylist.h"
#include "yield.h"

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
	Yield yield;
	QString title;
	QString instructions;
	QPixmap photo;
	IngredientList ingList;
	ElementList categoryList; // id+name
	ElementList authorList; //authors' id+name
	QTime prepTime;

	QDateTime ctime;
	QDateTime mtime;
	QDateTime atime;

	RatingList ratingList;
	IngredientPropertyList properties;

	// Public methods
	void empty( void );



};

#endif
