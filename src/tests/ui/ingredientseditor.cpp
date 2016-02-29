/***************************************************************************
 *   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ingredientseditor.h"

#include <qtest_kde.h>

#include "dialogs/recipeinput/ingredientseditor.h"
#include "backends/recipedb.h"
#include "datablocks/ingredient.h"

QTEST_KDEMAIN(Test_IngredientsEditor, GUI)

void Test_IngredientsEditor::testLoadAndDumpList()
{
	//Create ingredient data.
	int numberOfIngredients = 3;
	Ingredient ingredient[numberOfIngredients];

	Unit grams("gram","grams", 10);
	Unit cups("cup","cups", 11);

	ingredient[0].ingredientID = 1;
	ingredient[0].name = "first ingredient";
	ingredient[0].amount = 4.7;
	ingredient[0].units = grams;
	ingredient[0].groupID = RecipeDB::InvalidId;

	ingredient[1].ingredientID = 2;
	ingredient[1].name = "second ingredient";
	ingredient[1].amount = 4.5;
	ingredient[1].amount_offset = 0.5;
	ingredient[1].units = cups;
	ingredient[1].groupID = 1;
	ingredient[1].group = "group name";
	Ingredient sub1("sub 1 for second ing",1.0,Unit(),RecipeDB::InvalidId,100);
	Ingredient sub2("sub 2 for second ing",1.0,Unit(),RecipeDB::InvalidId,101);
	Ingredient sub3("sub 3 for second ing",1.0,Unit(),RecipeDB::InvalidId,102);
	ingredient[1].substitutes << sub1 << sub2 << sub3;

	ingredient[2].ingredientID = 3;
	ingredient[2].name = "third ingredient";
	ingredient[2].amount = 3.0;
	ingredient[2].amount_offset = 2.0;
	ingredient[2].groupID = 1;
	ingredient[2].group = "group name";
	ingredient[2].prepMethodList << Element("prep method 1",1) 
		<< Element("prep method 2",2);

	//Prepare the list with the above ingredients inside
	IngredientList ingList;
	for ( int i = 0; i < numberOfIngredients; ++i ) {
		ingList << ingredient[i];
	}

	//Create the ingredient editor and dump the list with it	
	IngredientsEditor * ingredientsEditor = new IngredientsEditor;
	ingredientsEditor->loadIngredientList( &ingList );
	ingredientsEditor->updateIngredientList();

	//Check the results
	qDebug() << ingList;
	QCOMPARE(ingList.count(), numberOfIngredients);

	int i = 0;
	IngredientList::const_iterator ing_it;
	for ( ing_it = ingList.constBegin(); ing_it != ingList.constEnd(); ++ing_it ) {

		//Check main fields
		QCOMPARE(ing_it->ingredientID, ingredient[i].ingredientID);
		QCOMPARE(ing_it->name, ingredient[i].name);
		QCOMPARE(ing_it->amount, ingredient[i].amount);
		QCOMPARE(ing_it->amount_offset, ingredient[i].amount_offset);
		QCOMPARE(ing_it->units.id(), ingredient[i].units.id());
		QCOMPARE(ing_it->groupID, ingredient[i].groupID);
		QCOMPARE(ing_it->group, ingredient[i].group);

		//Check preparation methods
		QCOMPARE(ing_it->prepMethodList.count(), ingredient[i].prepMethodList.count());
		//Iterators pointing to the resulting subs lists
		ElementList::const_iterator prep_it;
		ElementList::const_iterator prep_it_begin = ing_it->prepMethodList.constBegin(); 
		ElementList::const_iterator prep_it_end = ing_it->prepMethodList.constEnd(); 
		//Iterator pointing to the original (expected) subs list
		ElementList::const_iterator prep_it2 = ing_it->prepMethodList.constBegin();
		for ( prep_it = prep_it_begin; prep_it != prep_it_end; ++prep_it ) {
			QCOMPARE(prep_it->id, prep_it2->id);
			QCOMPARE(prep_it->name, prep_it2->name);
			++prep_it2;
		}

		//Check substitutes
		QCOMPARE(ing_it->substitutes.count(), ingredient[i].substitutes.count());
		//Iterators pointing to the resulting subs lists
		Ingredient::SubstitutesList::const_iterator sub_it;
		Ingredient::SubstitutesList::const_iterator sub_it_begin = ing_it->substitutes.constBegin();
		Ingredient::SubstitutesList::const_iterator sub_it_end = ing_it->substitutes.constEnd();
		//Iterator pointing to the original (expected) subs list
		Ingredient::SubstitutesList::const_iterator sub_it2 = ingredient[i].substitutes.constBegin();
		//Compare the substitutes list one by one
		for ( sub_it = sub_it_begin; sub_it != sub_it_end; ++sub_it ) {
			QCOMPARE(sub_it->ingredientID, sub_it2->ingredientID);
			QCOMPARE(sub_it->name, sub_it2->name);
			//Ingredient substitutes doesn't use the rest of fields for now
			//QCOMPARE(sub_it->amount, sub_it2->amount);
			//QCOMPARE(sub_it->amount_offset, sub_it2->amount_offset);
			//QCOMPARE(sub_it->units.id(), sub_it2->units.name);
			//QCOMPARE(sub_it->groupID, sub_it2->groupID);
			//QCOMPARE(sub_it->group, sub_it2->group);
			++sub_it2;
		}

		//Increment array index
		++i;
	}
}

#include "ingredientseditor.moc"
