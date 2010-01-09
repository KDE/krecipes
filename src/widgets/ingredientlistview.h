/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTLISTVIEW_H
#define INGREDIENTLISTVIEW_H

#include "dblistviewbase.h"

#include "datablocks/element.h"
//Added by qt3to4:
#include <QList>

class RecipeDB;
class KMenu;
class IngredientCheckListView;

/**
@author Unai Garro
*/
class IngredientCheckListItem: public Q3CheckListItem
{
public:
	IngredientCheckListItem( IngredientCheckListView* qlv, const Element &ing );
	IngredientCheckListItem( IngredientCheckListView* qlv, Q3ListViewItem *after, const Element &ing );
	~IngredientCheckListItem( void );

	int id( void ) const;
	QString name( void ) const;
	Element ingredient() const;

	virtual QString text( int column ) const;

protected:
	virtual void stateChange( bool on );

private:
	Element *ingStored;
	IngredientCheckListView *m_listview;
};



class IngredientListView : public DBListViewBase
{
	Q_OBJECT

public:
	IngredientListView( QWidget *parent, RecipeDB *db );

protected slots:
	virtual void createIngredient( const Element & ) = 0;
	virtual void removeIngredient( int ) = 0;
	virtual void load(int limit,int offset);

private slots:
	virtual void checkCreateIngredient( const Element & );
};



class StdIngredientListView : public IngredientListView
{
	Q_OBJECT

public:
	StdIngredientListView( QWidget *parent, RecipeDB *db, bool editable = false );

protected:
	virtual void createIngredient( const Element & );
	virtual void removeIngredient( int );
};



class IngredientCheckListView : public IngredientListView
{
public:
	IngredientCheckListView( QWidget *parent, RecipeDB *db );

	virtual void stateChange(IngredientCheckListItem *,bool);

	QList<Element> selections() const{ return m_selections; }

protected:
	virtual void createIngredient( const Element &ing );
	virtual void removeIngredient( int );

	virtual void load( int limit, int offset );

private:
	QList<Element> m_selections;
};

#endif //INGREDIENTLISTVIEW_H
