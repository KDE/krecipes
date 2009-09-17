/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@users.sourceforge.net>            *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

/**
@author Unai Garro
*/

#ifndef CREATEPROPERTYDIALOG_H
#define CREATEPROPERTYDIALOG_H

#include <QPushButton>
#include <q3groupbox.h>

#include <QLabel>

#include <klineedit.h>
#include <kdialog.h>
#include <kvbox.h>

#include "datablocks/unit.h"

class CreatePropertyDialog : public KDialog
{
public:
	CreatePropertyDialog( QWidget *parent, UnitList *list );
	~CreatePropertyDialog();
	QString newPropertyName( void );
	QString newUnitsName( void );


private:
	//Widgets
	Q3GroupBox *box;
	KLineEdit *propertyNameEdit;
	KLineEdit *propertyUnits;
	QLabel *nameEditText;
	QLabel *unitsText;

	//Internal variables
	UnitList *unitList;

	//Methods
	void loadUnits();
};

#endif
