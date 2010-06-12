/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROPERTYLISTVIEW_H
#define PROPERTYLISTVIEW_H

#include "widgets/dblistviewbase.h"

#include "datablocks/element.h"
#include "datablocks/ingredientproperty.h"
#include "datablocks/constraintlist.h"

class RecipeDB;
class KMenu;

class PropertyCheckListItem : public Q3CheckListItem
{
public:
	PropertyCheckListItem( Q3ListView* klv, const IngredientProperty &property );
	PropertyCheckListItem( Q3ListViewItem* it, const IngredientProperty &property );

	~PropertyCheckListItem( void )
	{}
	virtual QString text( int column ) const;
	virtual void setText ( int column, const QString & text );

	IngredientProperty property() const
	{
		return m_property;
	}

protected:
	IngredientProperty m_property;

};

class HidePropertyCheckListItem : public PropertyCheckListItem
{
public:
	HidePropertyCheckListItem( Q3ListView* klv, const IngredientProperty &property, bool enable = false );
	HidePropertyCheckListItem( Q3ListViewItem* it, const IngredientProperty &property, bool enable = false );

protected:
	virtual void stateChange( bool on );

private:
	bool m_holdSettings;
};


class ConstraintsListItem: public Q3CheckListItem
{
public:
	ConstraintsListItem( Q3ListView* klv, const IngredientProperty &pty ) : Q3CheckListItem( klv, QString(), Q3CheckListItem::CheckBox )
	{
		// Initialize the constraint data with the the property data
		ctStored = new Constraint();
		ctStored->id = pty.id;
		ctStored->name = pty.name;
		ctStored->perUnit = pty.perUnit;
		ctStored->units = pty.units;
		ctStored->max = 0;
		ctStored->min = 0;
	}

	~ConstraintsListItem( void )
	{
		delete ctStored;
	}

private:
	Constraint *ctStored;

public:
	void setConstraint( const Constraint &constraint )
	{
		delete ctStored;
		ctStored = new Constraint( constraint );

		setOn( ctStored->enabled );
	}
	double maxVal()
	{
		return ctStored->max;
	}
	double minVal()
	{
		return ctStored->min;
	}
	int propertyId()
	{
		return ctStored->id;
	}
	void setMax( double maxValue )
	{
		ctStored->max = maxValue;
		setText( 3, QString::number( maxValue ) );
	}
	void setMin( double minValue )
	{
		ctStored->min = minValue;
		setText( 2, QString::number( minValue ) );
	}
	virtual QString text( int column ) const
	{
		switch ( column ) {
		case 1:
			return ( ctStored->name );
		case 2:
			return ( QString::number( ctStored->min ) );
		case 3:
			return ( QString::number( ctStored->max ) );
		case 4:
			return ( QString::number( ctStored->id ) );
		default:
			return ( QString() );
		}
	}
};


class PropertyListView : public DBListViewBase
{
	Q_OBJECT

public:
	PropertyListView( QWidget *parent, RecipeDB * );

public slots:
	void reload( void );
	void load(int, int);

protected:
	bool m_loading;

protected slots:
	virtual void removeProperty( int id ) = 0;
	virtual void createProperty( const IngredientProperty &property ) = 0;
};



class StdPropertyListView : public PropertyListView
{
	Q_OBJECT

public:
	StdPropertyListView( QWidget *parent, RecipeDB *, bool editable = false );

protected:
	virtual void removeProperty( int id );
	virtual void createProperty( const IngredientProperty &property );
};



class PropertyConstraintListView : public PropertyListView
{
public:
	PropertyConstraintListView( QWidget *parent, RecipeDB * );

protected:
	virtual void removeProperty( int id );
	virtual void createProperty( const IngredientProperty &property );
};

class CheckPropertyListView : public StdPropertyListView
{
	Q_OBJECT

public:
	CheckPropertyListView( QWidget *parent, RecipeDB *, bool editable = false );

protected:
	virtual void createProperty( const IngredientProperty &property );

private:
	bool checkBounds( const QString &name );
};

#endif //PROPERTYLISTVIEW_H
