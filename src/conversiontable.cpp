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


ConversionTable::ConversionTable(QWidget* parent):QTable(parent)
{
editBoxValue=-1;
}

ConversionTable::~ConversionTable()
{
}


void ConversionTable::setCellContentFromEditor ( int row, int col)
{
setText (row,col,QString::number(editBoxValue));
}


QWidget* ConversionTable::createEditor( int row, int col, bool initFromCell ) const
{
((ConversionTable*) this)->eb=new EditBox(viewport());
//connect(eb,SIGNAL(valueChanged(double)),this,SLOT(doValueChanged()));
return(eb);
}