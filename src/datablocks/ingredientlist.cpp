/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "ingredientlist.h"

#include "backends/recipedb.h"

IngredientList::IngredientList() : QList<Ingredient>()
{}

IngredientList::~IngredientList()
{}

bool IngredientList::contains( const Ingredient &ing, bool compareAmount, RecipeDB *database ) const
{
	bool ret = false;
	for ( IngredientList::const_iterator it = begin(); it != end(); ++it ) {
		if ( (*it).ingredientID == ing.ingredientID ) {
			//see if we have enough of the ingredient
			//(only if an amount was specified for both)
			if ( compareAmount && (*it).amount > 0 && ing.amount > 0 ) {
				Ingredient convertedIng;
				if ( database->convertIngredientUnits( ing, (*it).units, convertedIng ) )
					ret = (*it).amount >= convertedIng.amount;
				else //we couldn't do the conversion
					ret = true;
			}
			else
				ret = true;
		}
	}
	if ( !ret ) {
		for ( Ingredient::SubstitutesList::const_iterator it = ing.substitutes.begin(); it != ing.substitutes.end(); ++it ) {
			ret = contains(*it, compareAmount, database);
			if ( ret ) break;
		}
	}
	return ret;
}

bool IngredientList::containsSubSet( IngredientList &il, IngredientList &missing, bool compareAmount, RecipeDB *database )
{
	missing.clear();
	bool contained = true;
	IngredientList::const_iterator it;

	for ( it = il.begin();it != il.end();++it ) {
		if ( !contains( *it, compareAmount, database ) ) {
			contained = false;
			missing.append( *it );
		}
	}

	return contained;
}

bool IngredientList::containsSubSet( IngredientList &il ) const
{
	IngredientList::const_iterator it;

	for ( it = il.begin();it != il.end();++it ) {
		if ( !contains( *it ) ) {
			return false;
		}
	}

	return true;
}

bool IngredientList::containsAny( const IngredientList &list, bool compareAmount, RecipeDB *database )
{
	for ( IngredientList::const_iterator this_list_it = begin(); this_list_it != end(); ++this_list_it ) {
		for ( IngredientList::const_iterator contains_list_it = list.begin(); contains_list_it != list.end(); ++contains_list_it ) {
			if ( contains( *contains_list_it, compareAmount, database ) )
				return true;
		}
	}

	return false;
}

int IngredientList::find( int id ) const // Search by id (which uses search by item, with comparison defined on header)
{
	Ingredient i;
	i.ingredientID = id;
	return findIndex( i );
}

Ingredient IngredientList::findByName( const QString &ing ) const
{
	IngredientList::const_iterator it_end = end();
	for ( IngredientList::const_iterator it = begin(); it != it_end; ++it ) {
		if ( ( *it ).name == ing )
			return *it;

		for ( Ingredient::SubstitutesList::const_iterator sub_it = ( *it ).substitutes.begin(); sub_it != ( *it ).substitutes.end(); ++sub_it ) {
			if ( ( *sub_it ).name == ing )
				return *sub_it;
		}
	}

	Ingredient el;
	el.ingredientID = -1;
	return el;
}

Ingredient IngredientList::findByName( const QRegExp &rx ) const
{
	IngredientList::const_iterator it_end = end();
	for ( IngredientList::const_iterator it = begin(); it != it_end; ++it ) {
		if ( rx.exactMatch( it->name ) ) 
			return *it;

		for ( Ingredient::SubstitutesList::const_iterator sub_it = ( *it ).substitutes.begin(); sub_it != ( *it ).substitutes.end(); ++sub_it ) {
			if ( rx.exactMatch( sub_it->name ) )
				return *sub_it;
		}
	}

	Ingredient el;
	el.ingredientID = -1;
	return el;
}

IngredientList::const_iterator IngredientList::find( IngredientList::const_iterator it, int id ) const // Search by id (which uses search by item, with comparison defined on header)
{
	Ingredient i;
	i.ingredientID = id;
	return QList<Ingredient>::find( it, i );
}

IngredientList::iterator IngredientList::find( IngredientList::iterator it, int id )  // Search by id (which uses search by item, with comparison defined on header)
{
	Ingredient i;
	i.ingredientID = id;
	return QList<Ingredient>::find( it, i );
}


int IngredientList::indexOf(int pos, int id) const
{
    Ingredient i;
    i.ingredientID = id;
    return QList<Ingredient>::indexOf(i, pos);
}

IngredientData& IngredientList::findSubstitute( const Ingredient &i )  // Search by id (which uses search by item, with comparison defined on header)
{
	QList<Ingredient>::iterator result = QList<Ingredient>::find(i);
	if ( result == end() ) {
		for ( IngredientList::iterator it = begin(); it != end(); ++it ) {
			Ingredient::SubstitutesList::iterator result = (*it).substitutes.find(i);
			if ( result != (*it).substitutes.end() )
				return *result;
		}
	}
	return *result;
}

void IngredientList::removeSubstitute( const Ingredient &i )
{
	QList<Ingredient>::iterator result = QList<Ingredient>::find(i);
	if ( result == end() ) {
		for ( IngredientList::iterator it = begin(); it != end(); ++it ) {
			Ingredient::SubstitutesList::iterator result = (*it).substitutes.find(i);
			if ( result != (*it).substitutes.end() ) {
				(*it).substitutes.remove(result);
				return;
			}
		}
	}
	remove(result);
}

void IngredientList::move( int index1, int index2 )  //moves element in pos index1, to pos after index2
{
	Ingredient tmp_ing( at(index1) );
	removeAt(index1);
	insert( index2, tmp_ing );
}

void IngredientList::move( int index1, int count1, int index2 )  //moves element in pos index1 and the following count1 items, to pos after index2
{
    for(int i = 0; i < count1; ++i) {
        insert(index2 - 1 , takeAt(index1 - 1));
    }
}

IngredientList IngredientList::groupMembers( int id, IngredientList::const_iterator begin ) const
{
	bool first_found = false;

	IngredientList matches;
	for ( IngredientList::const_iterator it = begin; it != end(); ++it ) {
		if ( ( *it ).groupID == id ) {
			matches.append( *it );
			first_found = true;
		}
		else if ( first_found )  //this is the end of this group... there may be more later though
			break;
	}

	return matches;
}

QList<IngredientList::const_iterator> IngredientList::_groupMembers( int id, IngredientList::const_iterator begin ) const
{
	bool first_found = false;

	QList<IngredientList::const_iterator> matches;
	for ( IngredientList::const_iterator it = begin; it != end(); ++it ) {
		if ( ( *it ).groupID == id ) {
			matches << it;
			first_found = true;
		}
		else if ( first_found )  //this is the end of this group... there may be more later though
			break;
	}

	return matches;
}

IngredientList IngredientList::firstGroup()
{
	usedGroups.clear();

	QList<IngredientList::const_iterator> members = _groupMembers( ( *begin() ).groupID, begin() );

	for ( QList<IngredientList::const_iterator>::const_iterator members_it = members.begin(); members_it != members.end(); ++members_it ) {
		usedGroups << *members_it;
	}

	return groupMembers( ( *begin() ).groupID, begin() );
}

IngredientList IngredientList::nextGroup()
{
	for ( IngredientList::const_iterator it = begin(); it != end(); ++it ) {
		if ( usedGroups.find( it ) == usedGroups.end() ) {
			QList<IngredientList::const_iterator> members = _groupMembers( ( *it ).groupID, it );

			for ( QList<IngredientList::const_iterator>::const_iterator members_it = members.begin(); members_it != members.end(); ++members_it ) {
				usedGroups << *members_it;
			}

			return groupMembers( ( *it ).groupID, it );
		}
	}
	return IngredientList();
}
