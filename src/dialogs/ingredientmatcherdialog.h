/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INGREDIENTMATCHERDIALOG_H
#define INGREDIENTMATCHERDIALOG_H

#include "element.h"
#include "ingredientlist.h"
#include "recipe.h"

#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qvbox.h>

#include <kcombobox.h>
#include <kstringhandler.h>

class KreListView;
class RecipeDB;


/**
@author Unai Garro
*/

class IngredientListItem:public QCheckListItem
{
public:
	IngredientListItem(QListView* qlv, const Element &ing ):QCheckListItem(qlv,QString::null,QCheckListItem::CheckBox)
	{
	// Initialize the ingredient data with the the property data
		ingStored=new Element();
		ingStored->id=ing.id;
		ingStored->name=ing.name;
	}

	~IngredientListItem(void)
	{
	delete ingStored;
	}
	int id(void){return ingStored->id;}
	QString name(void){return ingStored->name;}

private:
	Element *ingStored;

public:
	virtual QString text(int column) const
		{
		if (column==1) return(ingStored->name);
		else return(QString::null);
		}
};

// ### Should we make this the standard class for representing recipes? (we'd subclass it here to provide the ingredient list)
//     This way it would be consistent all around and maybe we wouldn't show the user the recipe id (which is probably just confusing).
//     It would even be easily configurable.
class RecipeListItem:public QListViewItem
{
public:
	RecipeListItem(QListView* qlv, const Recipe &r, const IngredientList &il ):QListViewItem(qlv,qlv->lastItem())
	{
		recipeStored=new Recipe();
		ingredientListStored=new QStringList();
		recipeStored->recipeID=r.recipeID;
		recipeStored->title=r.title;
		IngredientList::ConstIterator ili;
		for (ili=il.begin();ili!=il.end();++ili) ingredientListStored->append((*ili).name);
		
	}
	RecipeListItem(QListView* qlv, const Recipe &r):QListViewItem(qlv,qlv->lastItem())
	{
		recipeStored=new Recipe();
		ingredientListStored=0;
		recipeStored->recipeID=r.recipeID;
		recipeStored->title=r.title;
		
	}
	
	int rtti() const { return 1000; }

	~RecipeListItem(void)
	{
	delete recipeStored;
	delete ingredientListStored;
	}

	int recipeID() const { return recipeStored->recipeID; }
	
private:
	Recipe *recipeStored;
	QStringList *ingredientListStored;

public:
	virtual QString text(int column) const
		{
		if (column==0) return(recipeStored->title);
		else if ((column==1) && ingredientListStored) 
			return ingredientListStored->join (",");
		else return(QString::null);
		}
};

class SectionItem:public QListViewItem
{
public:
	SectionItem(QListView* qlv, QString sectionText ):QListViewItem(qlv,qlv->lastItem())
	{
	mText=sectionText;
	}

	~SectionItem(void){}
	virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );

private:
	QString mText;

public:
	virtual QString text(int column) const
	{
		if (column==0) return(mText);
		else return(QString::null);
	}
};
class IngredientMatcherDialog:public QVBox{

Q_OBJECT

public:

    IngredientMatcherDialog(QWidget *parent, RecipeDB* db);
    ~IngredientMatcherDialog();
    void reloadIngredients (void);

signals:
	void recipeSelected(int,int);    
    
private:
	//Private variables
	RecipeDB *database;
	
	//Widgets
	
	KreListView *ingredientListView;
	
	KreListView *recipeListView;
	QHBox *missingBox;
	QLabel *missingNumberLabel;
	KComboBox *missingNumberCombo;
	
	QPushButton *okButton;
	QPushButton *clearButton;
	
private slots:
	void findRecipes(void);

};
#endif
