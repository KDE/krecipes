/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "propertiesdialog.h"
#include <klocale.h>
#include <kdialog.h>

#include "DBBackend/recipedb.h"
#include "createpropertydialog.h"
#include "widgets/propertylistview.h"

PropertiesDialog::PropertiesDialog(QWidget *parent,RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;

    // Design dialog

    QHBoxLayout* layout = new QHBoxLayout( this, KDialog::marginHint(), KDialog::spacingHint() );

    propertyListView=new StdPropertyListView(this,database,true);
    propertyListView->reload();
    layout->addWidget (propertyListView);

    QVBoxLayout* vboxl = new QVBoxLayout( this, 0, KDialog::spacingHint() );
    addPropertyButton=new QPushButton(this);
    addPropertyButton->setText(i18n("Create ..."));
    addPropertyButton->setFlat(true);
    vboxl->addWidget(addPropertyButton);
    removePropertyButton=new QPushButton(this);
    removePropertyButton->setText(i18n("Delete"));
    removePropertyButton->setFlat(true);
    vboxl->addWidget(removePropertyButton);
    vboxl->addStretch();
    layout->addLayout(vboxl);

    // Connect signals & slots
    connect(addPropertyButton,SIGNAL(clicked()),this,SLOT(createNewProperty()));
    connect(removePropertyButton,SIGNAL(clicked()),this,SLOT(removeProperty()));
}


PropertiesDialog::~PropertiesDialog()
{
}

void PropertiesDialog::createNewProperty(void)
{
ElementList list;
database->loadUnits(&list);
CreatePropertyDialog* propertyDialog=new CreatePropertyDialog(this,&list);

if ( propertyDialog->exec() == QDialog::Accepted ) {
   QString name = propertyDialog->newPropertyName();
   QString units= propertyDialog->newUnitsName();
   if (!((name.isNull()) || (units.isNull()))) // Make sure none of the fields are empty
      database->addProperty(name, units);
}
delete propertyDialog;
}

void PropertiesDialog::removeProperty(void)
{
int propertyID = -1;
QListViewItem *it;
if ( (it=propertyListView->selectedItem()) )
{
propertyID=it->text(0).toInt();
}
database->removeProperty(propertyID);
}

void PropertiesDialog::reload(void)
{
propertyListView->reload();
}

#include "propertiesdialog.moc"
