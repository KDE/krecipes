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
class ConversionTable:public QTable{
Q_OBJECT
public:

    ConversionTable(QWidget* parent);
    ~ConversionTable();
private:

	//Internal Variables
	double editBoxValue;
	//Internal Methods
	void setCellContentFromEditor ( int row, int col);
	QWidget* createEditor(int row,int col, bool initFromCell ) const;
	//Internal Widgets
	EditBox *eb;

};

#endif
