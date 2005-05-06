/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROPERTYLISTVIEW_H
#define PROPERTYLISTVIEW_H

#include <klistview.h>

#include "datablocks/element.h"
#include "datablocks/ingredientproperty.h"
#include "datablocks/constraintlist.h"

class RecipeDB;
class KPopupMenu;

class PropertyCheckListItem : public QCheckListItem
{
public:
	PropertyCheckListItem( QListView* klv, const IngredientProperty &property );
	PropertyCheckListItem( QListViewItem* it, const IngredientProperty &property );

	~PropertyCheckListItem( void )
	{}
	virtual QString text( int column ) const;

	IngredientProperty property() const
	{
		return m_property;
	}

private:
	IngredientProperty m_property;

};


class ConstraintsListItem: public QCheckListItem
{
public:
	ConstraintsListItem( QListView* klv, const IngredientProperty &pty ) : QCheckListItem( klv, QString::null, QCheckListItem::CheckBox )
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
			return ( QString::null );
		}
	}
};


class PropertyListView : public KListView
{
	Q_OBJECT

public:
	PropertyListView( QWidget *parent, RecipeDB * );

public slots:
	void reload( void );

protected:
	RecipeDB *database;

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

private slots:
	void showPopup( KListView *, QListViewItem *, const QPoint & );

	void createNew();
	void remove
		();
	void rename();

	void modProperty( QListViewItem* i );
	void saveProperty( QListViewItem* i );

private:
	bool checkBounds( const QString &name );

	KPopupMenu *kpop;
};



class PropertyConstraintListView : public PropertyListView
{
public:
	PropertyConstraintListView( QWidget *parent, RecipeDB * );

protected:
	virtual void removeProperty( int id );
	virtual void createProperty( const IngredientProperty &property );
};

#endif //PROPERTYLISTVIEW_H
