/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONVERSIONTABLE_H
#define CONVERSIONTABLE_H
#include <qstring.h>
#include <qtable.h>
#include "editbox.h"

/**
@author Unai Garro
*/

class IDList: public QPtrList <int>
{
public:
IDList(){};
~IDList(){};
protected:
 virtual int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2);

};

class ConversionTable:public QTable{
Q_OBJECT
public:

	ConversionTable(QWidget* parent,int maxrows,int maxcols);
	~ConversionTable();
	void createNewItem(int r, int c, double amount);
	void setUnitIDs(const IDList &idList);
	void setRatio(int ingID1, int ingID2, double ratio);
private:

	//Internal Variables
	double editBoxValue;
	QIntDict<QTableItem> items;
	QIntDict<QWidget> widgets;
	IDList unitIDs; // unit ID list to know the units by ID, not name
	//Internal Methods
	void resizeData(int) {};
	QTableItem *item( int r, int c ) const;
	void setItem(int r, int c, QTableItem *i );
	void clearCell( int r, int c );
	void takeItem(QTableItem *item);
	void insertWidget(int r, int c, QWidget *w );
	QWidget *cellWidget( int r, int c ) const;
	void clearCellWidget( int r, int c );
	void initTable();
	//Internal Widgets
	EditBox *eb;
private slots:
	void acceptValueAndClose(void);

};

class ConversionTableItem:public QTableItem
{
public:
	ConversionTableItem( QTable *t, EditType et );
	QWidget *createEditor() const;
	void setContentFromEditor( QWidget *w );
	void setText( const QString &s );
	void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );

private:
	EditBox *eb;

};


#endif
