/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "datablocks/recipe.h"
//Added by qt3to4:
#include <QPixmap>

Recipe::Recipe()
{
	empty(); //Create & initialize the recipe empty originally
}

Recipe::~Recipe()
{}

void Recipe::empty( void )
{
	recipeID = -1;

	yield.setAmount(0);
	yield.setAmountOffset(0);
	yield.setType(QString());

	title.clear();
	instructions.clear();
	photo = QPixmap();
	ingList.clear();
	categoryList.clear();
	authorList.clear();
	ratingList.clear();
	prepTime = QTime( 0, 0 );
}


