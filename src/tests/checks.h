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
#include "datablocks/rating.h"

using std::cout;
using std::cerr;
using std::endl;

void check( const RatingList &rating, const RatingList &base );

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

	check( recipe.ratingList, base.ratingList );

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
		check( QString::number(ing_num)+": Ingredient group", (*ing_it).group, (*base_ing_it).group );

		ElementList::const_iterator prep_it = (*ing_it).prepMethodList.begin();
		ElementList::const_iterator base_prep_it = (*base_ing_it).prepMethodList.begin();
		for ( ; prep_it != (*ing_it).prepMethodList.end(); ++prep_it, ++base_prep_it ) {
			check( QString::number(ing_num)+": Ingredient prep_method", (*prep_it).name, (*base_prep_it).name );
		}
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

void check( const RatingList &rating, const RatingList &base )
{
	RatingList::const_iterator rating_it = rating.begin();
	RatingList::const_iterator base_rating_it = base.begin();
	for ( ; rating_it != rating.end() || base_rating_it != base.end(); ++rating_it, ++base_rating_it ) {
		check("checking rater",(*rating_it).rater,(*base_rating_it).rater);
		check("checking comment",(*rating_it).comment,(*base_rating_it).comment);

		RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin();
		RatingCriteriaList::const_iterator base_rc_it = (*base_rating_it).ratingCriteriaList.begin();
		for ( ; rc_it != (*rating_it).ratingCriteriaList.end() || base_rc_it != (*base_rating_it).ratingCriteriaList.end(); ++rc_it, ++base_rc_it ) {
			check("checking criteria name",(*rc_it).name,(*base_rc_it).name);
			check("checking stars",(*rc_it).stars,(*base_rc_it).stars);
		}
		check( "criteria count", int((*rating_it).ratingCriteriaList.count()), int((*base_rating_it).ratingCriteriaList.count()) );
	}
	check( "rating count", int(rating.count()), int(base.count()) );
}

#endif
