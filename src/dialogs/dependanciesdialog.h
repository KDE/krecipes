/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEPENDANCIESDIALOG_H
#define DEPENDANCIESDIALOG_H

#include <qdialog.h>
#include <klistview.h>
#include "elementlist.h"
/**
@author Unai Garro
*/
class DependanciesDialog:public QDialog{
public:
    //Methods
    DependanciesDialog(QWidget *parent=0,ElementList* recipeList=0, ElementList* ingredientList=0,ElementList* propertiesList=0);
    ~DependanciesDialog();
private:
    //Widgets
    KListView *recipeListView;
    KListView *ingredientListView;
    KListView *propertiesListView;
    void loadList(KListView* listView,ElementList *list);
};

#endif
