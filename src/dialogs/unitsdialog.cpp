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

#include <qlayout.h>
#include "unitsdialog.h"
#include "createelementdialog.h"
#include "dependanciesdialog.h"
#include "DBBackend/recipedb.h"
#include "editbox.h"
#include "conversiontable.h"

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kprogress.h>

UnitsDialog::UnitsDialog(QWidget *parent, RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;
    // Design dialog
    QGridLayout* layout = new QGridLayout( this, 1, 1, 0, 0);

    QSpacerItem* spacer_buttonright = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_buttonright, 0,2,3,3 );
    QSpacerItem* spacer_tobutton = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_tobutton, 0,2,1,1 );

    QSpacerItem* spacer_betweenbuttons = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacer_betweenbuttons, 1,2 );


    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 0,2,3,3 );

    unitListView =new KListView(this);
    unitListView->setAllColumnsShowFocus(true);
    unitListView->addColumn(i18n("Id"));
    unitListView->addColumn(i18n("Unit"));
    unitListView->setRenameable(1, true);
    unitListView->setDefaultRenameAction(QListView::Reject);
    unitListView->setAllColumnsShowFocus(true);
    layout->addMultiCellWidget(unitListView,0,3,0,0);

    conversionTable=new ConversionTable(this,1,1);
    layout->addMultiCellWidget(conversionTable,0,3,4,4);

    newUnitButton=new QPushButton(this);
    newUnitButton->setText(i18n("Create New Unit"));
    newUnitButton->setFlat(true);
    layout->addWidget(newUnitButton,0,2);

    removeUnitButton=new QPushButton(this);
    removeUnitButton->setText(i18n("Remove Unit"));
    removeUnitButton->setFlat(true);
    layout->addWidget(removeUnitButton,2,2);


    // Connect signals & slots
    connect(newUnitButton,SIGNAL(clicked()),this,SLOT(createNewUnit()));
    connect(this->unitListView,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modUnit( QListViewItem* )));
    connect(this->unitListView,SIGNAL(itemRenamed (QListViewItem*)),this, SLOT(saveUnit( QListViewItem* )));
    connect(removeUnitButton,SIGNAL(clicked()),this,SLOT(removeUnit()));
    connect(conversionTable,SIGNAL(ratioChanged(int,int,double)),this,SLOT(saveRatio(int,int,double)));

    //Populate data into the table
    reloadData();

}

UnitsDialog::~UnitsDialog()
{
}


void UnitsDialog::loadUnitsList(void)
{
ElementList unitList;
database->loadUnits(&unitList);
unitListView->clear();
for ( ElementList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it )
{
(void)new QListViewItem(unitListView,QString::number((*unit_it).id),(*unit_it).name);
}
}

void UnitsDialog::reloadData(void)
{
loadUnitsList();
loadConversionTable();
}

void UnitsDialog::createNewUnit(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(this,QString(i18n("New Unit")));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewUnit(result); // Create the new unit in database
   reloadData(); // Reload the unitlist from the database
}
delete elementDialog;
}

void UnitsDialog::modUnit(QListViewItem* i)
{
  newUnitButton->setEnabled(false);
  removeUnitButton->setEnabled(false);
  unitListView->rename(i, 1);
}

void UnitsDialog::saveUnit(QListViewItem* i)
{
  database->modUnit((i->text(0)).toInt(), i->text(1));
  newUnitButton->setEnabled(true);
  removeUnitButton->setEnabled(true);
}

void UnitsDialog::removeUnit(void)
{
// Find selected unit item
QListViewItem *it;
int unitID=-1;
if ( (it=unitListView->selectedItem()) ) unitID=it->text(0).toInt();

if (unitID>=0) // a unit was selected previously
{
ElementList recipeDependancies, propertyDependancies;
database->findUnitDependancies(unitID,&propertyDependancies,&recipeDependancies);

if (recipeDependancies.isEmpty() && propertyDependancies.isEmpty()) database->removeUnit(unitID);
else {// need warning!
	DependanciesDialog *warnDialog=new DependanciesDialog(0,&recipeDependancies,0,&propertyDependancies);
	if (warnDialog->exec()==QDialog::Accepted) database->removeUnit(unitID);
	delete warnDialog;
	}


reloadData();// Reload the list from database
}
}


void UnitsDialog::loadConversionTable(void)
{
ElementList unitList;
database->loadUnits(&unitList);
QStringList unitNames;
IDList unitIDs; // We need to store these in the table, so rows and cols are identified by unitID, not name.
conversionTable->clear();
for ( ElementList::const_iterator unit_it = unitList.begin(); unit_it != unitList.end(); ++unit_it )
{
unitNames.append((*unit_it).name);
int *newId=new int((*unit_it).id); // Create the new int element
unitIDs.append(newId); // append the element
}

// Resize the table
conversionTable->resize(unitNames.count(),unitNames.count());

// Set the table labels, and id's
conversionTable->setRowLabels(unitNames);
conversionTable->setColumnLabels(unitNames);
conversionTable->setUnitIDs(unitIDs);


// Load and Populate the data into the table
UnitRatioList ratioList;
database->loadUnitRatios(&ratioList);
for ( UnitRatioList::const_iterator ratio_it = ratioList.begin(); ratio_it != ratioList.end(); ratio_it++ )
{
conversionTable->setRatio((*ratio_it).uID1,(*ratio_it).uID2,(*ratio_it).ratio );
}
}

void UnitsDialog::saveRatio(int r, int c, double value)
{
UnitRatio ratio;

ratio.uID1=conversionTable->getUnitID(r);
ratio.uID2=conversionTable->getUnitID(c);
ratio.ratio=value;
database->saveUnitRatio(&ratio);

UnitRatio reverse_ratio; reverse_ratio.uID1 = ratio.uID2; reverse_ratio.uID2 = ratio.uID1; reverse_ratio.ratio = 1.0/ratio.ratio;
database->saveUnitRatio(&reverse_ratio);
conversionTable->setRatio(reverse_ratio);

#if 0
UnitRatioList ratioList;
database->loadUnitRatios(&ratioList);

saveAllRatios(ratioList);
#endif
}

void UnitsDialog::saveAllRatios( UnitRatioList &ratioList )
{
	#if 0
	KProgressDialog progress_dialog(this,"progress_dialog",i18n("Finding Unit Ratios"),QString::null,true);
	progress_dialog.progressBar()->setTotalSteps(ratioList.count()*ratioList.count());

	for ( UnitRatioList::const_iterator current_it = ratioList.begin(); current_it != ratioList.end(); current_it++ )
	{
		UnitRatio current_ratio = *current_it;
		for ( UnitRatioList::const_iterator ratio_it = ratioList.begin(); ratio_it != ratioList.end(); ratio_it++ )
		{
			if (progress_dialog.wasCancelled())
				return;

			progress_dialog.progressBar()->advance(1);
			kapp->processEvents();

			UnitRatio new_ratio;
			new_ratio.uID1 = current_ratio.uID1;
			new_ratio.uID2 = (*ratio_it).uID2;
			new_ratio.ratio = (*ratio_it).ratio * current_ratio.ratio;
			
			if ( ratioList.contains(new_ratio))
				continue;

			if ( ((*ratio_it).uID1 == current_ratio.uID2) && ((*ratio_it).uID2!=current_ratio.uID1) )
			{			
				UnitRatio reverse_ratio; reverse_ratio.uID1 = new_ratio.uID2; reverse_ratio.uID2 = new_ratio.uID1; reverse_ratio.ratio = 1.0/new_ratio.ratio;
	
				database->saveUnitRatio(&new_ratio); database->saveUnitRatio(&reverse_ratio);
				conversionTable->setRatio(new_ratio); conversionTable->setRatio(reverse_ratio);
				//ratioList.append(new_ratio); ratioList.append(reverse_ratio);
			}
		}
	}
	#endif
}

void UnitsDialog::reload(void)
{
this->reloadData();
}
