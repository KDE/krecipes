/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <qlayout.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <klistview.h>
#include "recipedb.h"

/**
@author Unai Garro
*/
class PropertiesDialog:QVBox{
public:
    PropertiesDialog(QWidget *parent,RecipeDB *db);

    ~PropertiesDialog();

private:
  // Variables
    RecipeDB *database;
  // Widgets
    QGridLayout* layout;
    QPushButton* addPropertyButton;
    QPushButton* removePropertyButton;
    KListView* propertyListView;


};

#endif
