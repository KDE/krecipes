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

#include "selectpropertydialog.h"
#include <klocale.h>

#include "element.h"
#include "elementlist.h"
#include "ingredientpropertylist.h"

SelectPropertyDialog::SelectPropertyDialog(QWidget* parent,IngredientPropertyList *propertyList,ElementList *unitList): QDialog(parent)
{

// Initialize internal variables
unitListBack =new ElementList;

// Initialize Widgets
    QVBoxLayout *layout = new QVBoxLayout( this, 11, 6 );

    box = new QGroupBox( this );
    box->setTitle(i18n("Choose Property"));
    box->setColumnLayout(0, Qt::Vertical );
    box->layout()->setSpacing( 6 );
    box->layout()->setMargin( 11 );
    QVBoxLayout *boxLayout = new QVBoxLayout( box->layout() );
    boxLayout->setAlignment( Qt::AlignTop );

    propertyChooseView = new KListView( box, "propertyChooseView" );
    propertyChooseView->addColumn( i18n( "Id" ) );
    propertyChooseView->addColumn( i18n( "Property" ) );
    propertyChooseView->setAllColumnsShowFocus(true);
    boxLayout->addWidget( propertyChooseView );

    QHBoxLayout *layout2 = new QHBoxLayout( this, 0, 6 );

    perUnitsLabel = new QLabel( box );
    perUnitsLabel->setGeometry(QRect(5,285,100,30));
    perUnitsLabel->setText(i18n("Per units:"));
    layout2->addWidget( perUnitsLabel );

    perUnitsBox = new KComboBox( FALSE, box );
    layout2->addWidget( perUnitsBox );
    boxLayout->addLayout( layout2 );

    QHBoxLayout *layout1 = new QHBoxLayout( this, 0, 6 );

    okButton = new QPushButton( box );
    okButton->setText(i18n("&OK"));
    okButton->setFlat(true);
    layout1->addWidget( okButton );

    cancelButton = new QPushButton( box );
    cancelButton->setText(i18n("&Cancel"));
    cancelButton->setFlat(true);
    layout1->addWidget( cancelButton );
    boxLayout->addLayout( layout1 );
    layout->addWidget( box );

    resize( QSize(200, 380).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

// Load data
loadProperties(propertyList);
loadUnits(unitList);

// Connect signals & Slots
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}


SelectPropertyDialog::~SelectPropertyDialog()
{
}


int SelectPropertyDialog::propertyID(void)
{

QListViewItem *it;
if ( (it=propertyChooseView->selectedItem()) )
{
return(it->text(0).toInt());
}
else return(-1);
}

int SelectPropertyDialog::perUnitsID()
{

int comboCount=perUnitsBox->count();
if (comboCount>0){ // If not, the list may be empty (no list defined) and crashes while reading as seen before. So check just in case.
int comboID=perUnitsBox->currentItem();
return unitListBack->getElement(comboID)->id;
}
else return(-1);
}

void SelectPropertyDialog::loadProperties(IngredientPropertyList *propertyList)
{
for ( IngredientProperty *property =propertyList->getFirst(); property; property =propertyList->getNext() )
{
(void)new QListViewItem(propertyChooseView,QString::number(property->id),property->name);
}
}
void SelectPropertyDialog::loadUnits(ElementList *unitList)
{
for ( Element *unit =unitList->getFirst(); unit; unit =unitList->getNext() )
{
// Insert in the combobox
perUnitsBox->insertItem(unit->name);

// Store with index for using later
Element newUnit; newUnit.id=unit->id; newUnit.name=unit->name;
unitListBack->add(newUnit);
}
}
