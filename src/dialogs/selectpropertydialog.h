/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SELECTPROPERTYDIALOG_H
#define SELECTPROPERTYDIALOG_H

#include <qwidget.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <klistview.h>
#include <kcombobox.h>

#include "datablocks/unit.h"

class IngredientPropertyList;

/**
@author Unai Garro
*/
class SelectPropertyDialog: public QDialog
{
public:
	// Methods
	SelectPropertyDialog( QWidget* parent, IngredientPropertyList *propertyList, UnitList *unitList );
	~SelectPropertyDialog();
	int propertyID( void );
	int perUnitsID( void );
private:
	//Widgets
	QVBoxLayout *container;
	QGroupBox *box;
	QVBox *vbox;
	KListView *propertyChooseView;
	QLabel *perUnitsLabel;
	KComboBox *perUnitsBox;
	QPushButton* okButton;
	QPushButton* cancelButton;
	void loadProperties( IngredientPropertyList *propertyList );
	void loadUnits( UnitList *unitList );

	//Internal variables
	UnitList *unitListBack; // To store unit list with ID's for later use

};

#endif
