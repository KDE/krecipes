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
#include "widgets/recipelistview.h"

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

class CustomRecipeListItem : public RecipeListItem
{
public:
	CustomRecipeListItem(QListView* qlv, const Recipe &r, const IngredientList &il ):RecipeListItem(qlv,r)
	{
		ingredientListStored=new QStringList();
		IngredientList::ConstIterator ili;
		for (ili=il.begin();ili!=il.end();++ili) ingredientListStored->append((*ili).name);
		
		moveItem(qlv->lastItem());
	}
	CustomRecipeListItem(QListView* qlv, const Recipe &r):RecipeListItem(qlv,r)
	{
		ingredientListStored=0;

		moveItem(qlv->lastItem());
	}
	
	~CustomRecipeListItem(void)
	{
	delete ingredientListStored;
	}
	
private:
	QStringList *ingredientListStored;

public:
	virtual QString text(int column) const {
		if (column==2  && ingredientListStored) 
			return ingredientListStored->join (",");
		else return(RecipeListItem::text(column));
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
