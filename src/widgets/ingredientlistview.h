/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INGREDIENTLISTVIEW_H
#define INGREDIENTLISTVIEW_H 
 
#include "klistview.h"

#include "element.h"

class RecipeDB;
class KPopupMenu;

/**
@author Unai Garro
*/
class IngredientCheckListItem:public QCheckListItem
{
public:
	IngredientCheckListItem(QListView* qlv, const Element &ing ):QCheckListItem(qlv,QString::null,QCheckListItem::CheckBox)
	{
	// Initialize the ingredient data with the the property data
		ingStored=new Element();
		ingStored->id=ing.id;
		ingStored->name=ing.name;
	}

	~IngredientCheckListItem(void)
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
		switch (column)
		{
		case 1: return(ingStored->name);
		case 2: return(QString::number(ingStored->id));
		default: return QString::null;
		}
	}
};



class IngredientListView : public KListView
{
Q_OBJECT

public:
	IngredientListView( QWidget *parent, RecipeDB *db );

	void reload();

protected slots:
	virtual void createIngredient(const Element &)=0;
	virtual void removeIngredient(int)=0;

protected:
	//make this protected because the data should always be synced with the database
	void clear(){ KListView::clear(); }

	RecipeDB *database;
};



class StdIngredientListView : public IngredientListView
{
Q_OBJECT

public:
	StdIngredientListView( QWidget *parent, RecipeDB *db, bool editable=false );

protected:
	virtual void createIngredient(const Element &);
	virtual void removeIngredient(int);

private slots:
	void showPopup(KListView *, QListViewItem *, const QPoint &);

	void createNew();
	void remove();
	void rename();

	void modIngredient(QListViewItem* i);
	void saveIngredient(QListViewItem* i);

private:
	KPopupMenu *kpop;
};



class IngredientCheckListView : public IngredientListView
{
public:
	IngredientCheckListView( QWidget *parent, RecipeDB *db );

protected:
	virtual void createIngredient(const Element &ing);
	virtual void removeIngredient(int);
};

#endif //INGREDIENTLISTVIEW_H
