/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "datablocks/recipe.h"

Recipe::Recipe()
{
	empty(); //Create & initialize the recipe empty originally
}

Recipe::~Recipe()
{}

void Recipe::empty( void )
{
	recipeID = -1;

	yield.amount = 0;
	yield.amount_offset = 0;
	yield.type = QString::null;

	title = QString::null;
	instructions = QString::null;
	photo = QPixmap();
	ingList.empty();
	categoryList.clear();
	authorList.clear();
	ratingList.clear();
	prepTime = QTime( 0, 0 );
}


QString Yield::amountToString() const
{
	QString ret = QString::number(amount);
	if ( amount_offset > 0 )
		ret += "-"+QString::number(amount+amount_offset);

	return ret;
}

QString Yield::toString() const
{
	return amountToString() + " " + type;
}

