/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGLISTVIEWITEM_H
#define INGLISTVIEWITEM_H

#include "qlistview.h"

#include "datablocks/ingredient.h"

#define INGGRPLISTVIEWITEM_RTTI 1003
#define INGLISTVIEWITEM_RTTI 1004

class IngListViewItem : public QListViewItem
{
public:
	IngListViewItem( QListView* qlv, const Ingredient &i );
	IngListViewItem( QListView* qlv, QListViewItem *after, const Ingredient &i );
	IngListViewItem( QListViewItem* qli, QListViewItem *after, const Ingredient &i );

	int rtti() const;

	Ingredient ingredient() const;

	void setAmount( double amount, double amount_offset );
	void setUnit( const Unit &unit );
	void setPrepMethod( const QString &prepMethod );

	virtual void setText( int column, const QString &text );

protected:
	Ingredient m_ing;
	QString amount_str;

public:
	virtual QString text( int column ) const;

private:
	void init( const Ingredient &i );
};


class IngGrpListViewItem : public QListViewItem
{
public:
	IngGrpListViewItem( QListView* qlv, QListViewItem *after, const QString &group, int id );

	int rtti() const;

	QString group() const;
	int id() const;

	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );

protected:
	QString m_group;
	int m_id;

private:
	void init( const QString &group, int id );
};

#endif
