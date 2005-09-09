/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CHECKS_H
#define CHECKS_H

#include <cmath>
#include <iostream>

#include <qstring.h>
#include <qpixmap.h>
#include <qimage.h>

#include "datablocks/categorytree.h"

using std::cout;
using std::cerr;
using std::endl;

bool check(const QString &txt, const QString &a, const QString &b)
{
	if ( a != b ) {
		cout << "ERROR: Tested " << txt.latin1() << ", expected" << endl;
		cout << "'" << b.latin1() << "' (" << b.length() << " chars)" << endl;
		cout << "but got" << endl;
		cout << "'" << a.latin1() << "' (" << a.length() << " chars)" << endl;
		exit( 1 );
	}

	return true;
}

bool check(const QString &txt, int a, int b)
{
	if ( a != b ) {
		cout << "ERROR: Tested " << txt.latin1() << ", expected" << endl;
		cout << "'" << b << "'" << endl;
		cout << "but got" << endl;
		cout << "'" << a << "'" << endl;
		exit( 1 );
	}

	return true;
}

bool check(const QString &txt, double a, double b)
{
	if ( fabs(a - b) > 1e-10 ) {
		cout << "ERROR: Tested " << txt.latin1() << ", expected" << endl;
		cout << "'" << b << "'" << endl;
		cout << "but got" << endl;
		cout << "'" << a << "'" << endl;
		exit( 1 );
	}

	return true;
}

bool check(const QString &txt, const QPixmap &a, const QPixmap &b)
{
	if ( a.size() != b.size() ) {
		
		cout << "ERROR: Tested " << txt.latin1() << ": photos differ" << endl;
	//	exit( 1 );
	}

	return true;
}

void check( const Recipe &recipe, const Recipe &base )
{
	check( "Recipe title", recipe.title, base.title );
	check( "Yield base", recipe.yield.amount, base.yield.amount );
	check( "Yield offset", recipe.yield.amount_offset, base.yield.amount_offset );
	check( "Yield type", recipe.yield.type, base.yield.type );
	check( "Instructions", recipe.instructions, base.instructions );
	check( "Photo", recipe.photo, base.photo );

	int cat_num = 1;
	ElementList::const_iterator cat_it = recipe.categoryList.begin();
	ElementList::const_iterator base_cat_it = base.categoryList.begin();
	for ( ; cat_it != recipe.categoryList.end() || base_cat_it != base.categoryList.end(); ++cat_it, ++base_cat_it ) {
		check( QString::number(cat_num)+": Category", (*cat_it).name, (*base_cat_it).name );
		++cat_num;
	}
	check( "category count", cat_num-1, base.categoryList.count() );

	int author_num = 1;
	ElementList::const_iterator author_it = recipe.authorList.begin();
	ElementList::const_iterator base_author_it = base.authorList.begin();
	for ( ; author_it != recipe.authorList.end() || base_author_it != base.authorList.end(); ++author_it, ++base_author_it ) {
		check( QString::number(author_num)+": Author", (*author_it).name, (*base_author_it).name );
		++author_num;
	}
	check( "author count", author_num-1, base.authorList.count() );

	int ing_num = 1;
	IngredientList::const_iterator ing_it = recipe.ingList.begin();
	IngredientList::const_iterator base_ing_it = base.ingList.begin();
	for ( ; ing_it != recipe.ingList.end() || base_ing_it != base.ingList.end(); ++ing_it, ++base_ing_it ) {
		check( QString::number(ing_num)+": Ingredient name", (*ing_it).name, (*base_ing_it).name );
		check( QString::number(ing_num)+": Ingredient amount", (*ing_it).amount,(*base_ing_it).amount );
		check( QString::number(ing_num)+": Ingredient amount_offset", (*ing_it).amount_offset,(*base_ing_it).amount_offset );
		check( QString::number(ing_num)+": Ingredient singular unit", (*ing_it).units.name, (*base_ing_it).units.name );
		check( QString::number(ing_num)+": Ingredient plural unit", (*ing_it).units.plural, (*base_ing_it).units.plural );
		check( QString::number(ing_num)+": Ingredient prep_method", (*ing_it).prepMethod, (*base_ing_it).prepMethod );
		check( QString::number(ing_num)+": Ingredient group", (*ing_it).group, (*base_ing_it).group );
		

		++ing_num;
	}
	check( "ingredient count", ing_num-1, base.ingList.count() );
}

bool check( const CategoryTree *catStructure, const CategoryTree *baseCatStructure )
{
	CategoryTree * it = catStructure->firstChild();
	CategoryTree * base_it = baseCatStructure->firstChild();
	for ( ; it && base_it; it = it->nextSibling(), base_it = base_it->nextSibling() ) {
		check( it, base_it );

		if ( it->category.name != base_it->category.name ) {
			printf("FAILED: Category structure differs\n");
			exit(1);
		}
	}

	if ( base_it != it ) { //these should both be NULL
		printf("FAILED: Category structure differs\n");
		exit(1);
	}

	return true;
}

#endif
