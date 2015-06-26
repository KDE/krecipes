/***************************************************************************
*   Copyright © 2012-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KRESEARCHRESULTLISTWIDGET_H
#define KRESEARCHRESULTLISTWIDGET_H


#include "krerecipeslistwidget.h"

#include "datablocks/recipelist.h"


class KreSearchResultListWidget : public KreRecipesListWidget
{
	Q_OBJECT

public:
	KreSearchResultListWidget( QWidget *parent, RecipeDB *db );

	void showNotFoundMessage( const QString & message );
	void displayRecipes( const RecipeList & recipes );

protected slots:
	//Operations with categories.
	void createCategorySlot( const Element & /*category*/, int /*parent_id*/ ){};
	void removeCategorySlot( int /*id*/ ){};

	//Operations with recipes.
	void createRecipeSlot( const Element & /*recipe*/, const ElementList & /*categories*/ ){};
	void modifyRecipeSlot( const Element & recipe, const ElementList & categories );
	void removeRecipeFromCategorySlot( int /*recipe_id*/, int /*category_id*/ ){};
	//void removeRecipeSlot( int id );

};


#endif //KRESEARCHRESULTLISTWIDGET_H
