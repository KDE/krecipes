/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SELECTPROPERTYDIALOG_H
#define SELECTPROPERTYDIALOG_H

#include <qwidget.h>
#include <q3groupbox.h>
#include <QLabel>
#include <QPushButton>


#include <k3listview.h>
#include <kcombobox.h>
#include <kdialog.h>
#include <kvbox.h>

#include "datablocks/unit.h"

class IngredientPropertyList;
class RecipeDB;

/**
@author Unai Garro
*/
class SelectPropertyDialog: public KDialog
{
public:
	enum OptionFlag { ShowEmptyUnit, HideEmptyUnit } ;

	// Methods
	SelectPropertyDialog( QWidget* parent, int ingredientID, RecipeDB *db, OptionFlag showEmpty = ShowEmptyUnit );
	~SelectPropertyDialog();
	int propertyID( void );
	int perUnitsID( void );
private:
	//Widgets
	Q3GroupBox *box;
	K3ListView *propertyChooseView;
	QLabel *perUnitsLabel;
	KComboBox *perUnitsBox;
	OptionFlag m_showEmpty;

	void loadProperties( IngredientPropertyList *propertyList );
	void loadUnits( UnitList *unitList );

	//Internal variables
	UnitList *unitListBack; // To store unit list with ID's for later use
	int ingredientID;
	RecipeDB *db;

};

#endif
