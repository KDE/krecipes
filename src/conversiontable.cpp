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


connect(this,SIGNAL(signalSymmetric(int,int,double)),this,SLOT(makeSymmetric(int,int,double)));
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
	eb->setValue(text().toDouble()); // Initialize the box with this value
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
		emit ratioChanged(row(),col(),eb->value()); // Signal to store
		if (row()!=col()) emit signalSymmetric(row(),col(),eb->value()); // Signal to make symmetric. just in case, check if row,col are different (it shouldn't be editable, anyway)
		}
	}
    else
	QTableItem::setContentFromEditor( w );
}

void ConversionTableItem::setText( const QString &s )
{
std::cerr<<"Setting text:"<<s<<"\n";
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

for (int r=0;r<numRows();r++)
{
this->createNewItem(r,r,1.0);
item(r,r)->setEnabled(false); // Diagonal is not editable
}
}

void ConversionTable::createNewItem(int r, int c, double amount)
{

ConversionTableItem *ci= new ConversionTableItem(this,QTableItem::WhenCurrent);
setItem(r,c, ci );
ci->setText(QString::number(amount));
// connect signal (forward) to know when it's actually changed
connect(ci, SIGNAL(ratioChanged(int,int,double)),this,SIGNAL(ratioChanged(int,int,double)));
connect(ci, SIGNAL(signalSymmetric(int,int,double)),this,SIGNAL(signalSymmetric(int,int,double)));
connect(ci, SIGNAL(signalRepaintCell(int,int)),this,SLOT(repaintCell(int,int)));

}

void ConversionTable::setUnitIDs(const IDList &idList)
{
unitIDs=idList;
}

void ConversionTable::setRatio(int ingID1, int ingID2, double ratio)
{
int indexID1=unitIDs.find(&ingID1);
int indexID2=unitIDs.find(&ingID2);
createNewItem(indexID1,indexID2,ratio);
}

int IDList::compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2)
{
return (*((int*)item1)-*((int*)item2));
}

int ConversionTable::getUnitID(int rc)
{
return(*(unitIDs.at(rc)));
return(1);
}

QWidget * ConversionTable::beginEdit ( int row, int col, bool replace )
{
// If there's no item, create it first.
if (!item(row,col))
	{
	createNewItem(row,col,0);
	}

	if (!(item(col,row)) && (row!=col)) createNewItem(col,row,0); // Create the symmetric one.It shouldn't be necessary to do this if (row,col) exists, but just in case, it's checked. row==col is neither supposed to be editable, but check anyway

// Then call normal beginEdit
QTable::beginEdit(row,col,replace);
}

void ConversionTable::makeSymmetric(int r,int c,double amount)
{
std::cerr<<"Making symmetric...\n";
QTableItem *it;
it=item(c,r);
if (amount)
(( ConversionTableItem *) it)->setTextAndSave(QString::number(1.0/amount)); // Change value and store in database
else
(( ConversionTableItem *) it)->setTextAndSave(QString::number(0));// Change value and store in database
}

void ConversionTableItem::setTextAndSave(const QString &s)
{
setText(s); // Change text
emit signalRepaintCell(row(),col()); // Indicate to update the cell to the table. Otherwise it's not repainted
emit ratioChanged(row(),col(),s.toDouble()); // Signal to store
}

void ConversionTable::repaintCell(int r,int c)
{
QTable::updateCell(r,c);
}

void ConversionTable::resize(int r,int c)
{
setNumRows(r);
setNumCols(c);
initTable();
}

void ConversionTable::clear(void)
{
items.clear();
widgets.clear();
unitIDs.clear();
resize(0,0);

}