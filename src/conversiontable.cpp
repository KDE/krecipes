/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "conversiontable.h"
#include <iostream>

ConversionTable::ConversionTable(QWidget* parent,int maxrows,int maxcols):QTable( maxrows, maxcols, parent, "table" )
{
editBoxValue=-1;
items.setAutoDelete(true);
widgets.setAutoDelete(true);
initTable();
}

ConversionTable::~ConversionTable()
{
}

QTableItem* ConversionTable::item( int r, int c ) const
{
return items.find(indexOf(r,c));
}

void ConversionTable::setItem(int r, int c, QTableItem *i )
{
items.replace( indexOf( r, c ), i );
i->setRow(r); // Otherwise the item
i->setCol(c); //doesn't know where it is!
}

void ConversionTable::clearCell( int r, int c )
{
items.remove(indexOf(r,c));
}

void ConversionTable::takeItem(QTableItem *item)
{
items.setAutoDelete(false);
items.remove(indexOf(item->row(),item->col()));
items.setAutoDelete(true);
}

void ConversionTable::insertWidget(int r, int c, QWidget *w )
{
widgets.replace(indexOf(r,c),w);
}

QWidget* ConversionTable::cellWidget( int r, int c ) const
{
return widgets.find( indexOf( r, c ) );
}

void ConversionTable::clearCellWidget( int r, int c )
{
QWidget *w = widgets.take(indexOf(r,c));
if (w) w->deleteLater();
}


ConversionTableItem::ConversionTableItem( QTable *t, EditType et ):QTableItem( t, et), eb( 0 )
{
// we do not want this item to be replaced
	setReplaceable( false );
}

void ConversionTableItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected )
{
	QColorGroup g(cg);

	// Draw in gray all those cells which are not editable

	if (row() == col())
	g.setColor( QColorGroup::Base, gray );
	QTableItem::paint( p, g, cr, selected );
}

QWidget* ConversionTableItem::createEditor() const
{
	((ConversionTableItem*)this)->eb=new EditBox(table()->viewport());
	QObject::connect(eb,SIGNAL(valueChanged(double)),table(),SLOT(acceptValueAndClose()));
	return eb;
}

void ConversionTable::acceptValueAndClose()
{
QTable::endEdit(currentRow(),currentColumn(),true,false);
}

void ConversionTableItem::setContentFromEditor( QWidget *w )
{
	// the user changed the value of the combobox, so synchronize the
	// value of the item (its text), with the value of the combobox
    if ( w->inherits( "EditBox" ) )
	{
	if (eb->accepted)
		{
		setText(QString::number(eb->value())); // Only accept value if Ok was pressed before
		emit ratioChanged(row(),col(),eb->value());
		}
	}
    else
	QTableItem::setContentFromEditor( w );
}

void ConversionTableItem::setText( const QString &s )
{
	if (eb) {
	// initialize the editbox from the text
	eb->setValue(s.toDouble());
	}
	QTableItem::setText(s);
}
QString ConversionTable::text(int r, int c ) const			 // without this function, the usual (text(r,c)) won't work
{

return(item(r,c)->text());  //Note that item(r,c) was reimplemented here for large sparse tables...


}

void ConversionTable::initTable()
{
}

void ConversionTable::createNewItem(int r, int c, double amount)
{
ConversionTableItem *ci= new ConversionTableItem(this,QTableItem::WhenCurrent);
std::cerr<<r<<";"<<c<<"\n";
setItem(r,c, ci );
ci->setText(QString::number(amount));
// insert the data into the table
//setText(r,c,QString::number(amount));


// connect signal (forward) to know when it's actually changed
connect(ci, SIGNAL(ratioChanged(int,int,double)),this,SIGNAL(ratioChanged(int,int,double)));
}

void ConversionTable::setUnitIDs(const IDList &idList)
{
unitIDs=idList;
int *id;
for (id=unitIDs.first();id; id=unitIDs.next())
{
std::cerr<<"index taken: "<<*id<<"\n";
}
}

void ConversionTable::setRatio(int ingID1, int ingID2, double ratio)
{
int indexID1=unitIDs.find(&ingID1);
int indexID2=unitIDs.find(&ingID2);
std::cerr<<"Found indexID1: "<< indexID1<<"\n";
std::cerr<<"Found indexID2: "<< indexID2<<"\n";
createNewItem(indexID1,indexID2,ratio);
}

int IDList::compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2)
{
std::cerr<<"Items comparing: "<<*((int*)item1)<<" "<<*((int*)item2)<<"\n";
return (*((int*)item1)-*((int*)item2));
}

int ConversionTable::getUnitID(int rc)
{
return(*(unitIDs.at(rc)));
return(1);
}
