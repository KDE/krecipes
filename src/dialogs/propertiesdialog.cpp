/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "propertiesdialog.h"

PropertiesDialog::PropertiesDialog(QWidget *parent,RecipeDB *db):QVBox(parent)
{

    // Store pointer to database
    database=db;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 1,4,0,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addMultiCell(spacer_top,0,0,1,4);

    propertyListView=new KListView (this);
    layout->addMultiCellWidget (propertyListView,1,4,1,6);
    propertyListView->addColumn("Id");
    propertyListView->addColumn("Property");
    propertyListView->addColumn("Units");

}


PropertiesDialog::~PropertiesDialog()
{
}


