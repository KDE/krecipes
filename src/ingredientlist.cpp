/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *                                                                         *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientlist.h"

IngredientList::IngredientList():QValueList<Ingredient>()
{
}

IngredientList::~IngredientList()
{
}

bool IngredientList::contains(Ingredient &ing) const
{
return(find(ing.ingredientID)!=-1);
}

bool IngredientList::containsSubSet(IngredientList &il,IngredientList &missing)
{
missing.empty();
bool contained=true;
IngredientList::Iterator it;

for (it=il.begin();it!=il.end();++it)
	{
	if (!contains(*it)) 
		{
		contained=false;
		missing.append(*it);
		}
	}

return contained;
}

void IngredientList::empty(void)
{
this->clear();
}

int IngredientList::find(int id) const // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return findIndex(i);
}

IngredientList::const_iterator IngredientList::find(IngredientList::const_iterator it,int id) const // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return QValueList<Ingredient>::find(it,i);
}

IngredientList::iterator IngredientList::find(IngredientList::iterator it,int id) // Search by id (which uses search by item, with comparison defined on header)
{
Ingredient i; i.ingredientID=id;
return QValueList<Ingredient>::find(it,i);
}

void IngredientList::move(int index1,int index2) //moves element in pos index1, to pos after index2
{
IngredientList::iterator tmp_it = this->at(index1);
Ingredient tmp_ing(*tmp_it);

this->remove( tmp_it );

tmp_it = this->at(index2);
this->insert(tmp_it,tmp_ing);
}

IngredientList IngredientList::groupMembers(int id, IngredientList::const_iterator begin ) const
{
bool first_found = false;

IngredientList matches;
for ( IngredientList::const_iterator it = begin; it != end(); ++it ) {
	if ( (*it).groupID == id ) {
		matches.append(*it);
		first_found = true;
	}
	else if ( first_found ) //this is the end of this group... there may be more later though
		break;
}

return matches;
}

QValueList<IngredientList::const_iterator> IngredientList::_groupMembers(int id, IngredientList::const_iterator begin ) const
{
bool first_found = false;

QValueList<IngredientList::const_iterator> matches;
for ( IngredientList::const_iterator it = begin; it != end(); ++it ) {
	if ( (*it).groupID == id ) {
		matches << it;
		first_found = true;
	}
	else if ( first_found ) //this is the end of this group... there may be more later though
		break;
}

return matches;
}

IngredientList IngredientList::firstGroup()
{
usedGroups.clear();

QValueList<IngredientList::const_iterator> members = _groupMembers((*begin()).groupID,begin());

for ( QValueList<IngredientList::const_iterator>::const_iterator members_it = members.begin(); members_it != members.end(); ++members_it ) {
	usedGroups << *members_it;
}

return groupMembers((*begin()).groupID,begin());
}

IngredientList IngredientList::nextGroup()
{
for ( IngredientList::const_iterator it = begin(); it != end(); ++it ) {
	if ( usedGroups.find(it) == usedGroups.end() ) {
		QValueList<IngredientList::const_iterator> members = _groupMembers((*it).groupID,it);

		for ( QValueList<IngredientList::const_iterator>::const_iterator members_it = members.begin(); members_it != members.end(); ++members_it ) {
			usedGroups << *members_it;
		}

		return groupMembers((*it).groupID,it);
	}
}
return IngredientList();
}
