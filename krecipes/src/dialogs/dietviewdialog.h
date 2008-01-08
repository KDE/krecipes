/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DIETVIEWDIALOG_H
#define DIETVIEWDIALOG_H

#include <qpushbutton.h>
#include <qlayout.h>
#include <q3vbox.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <khtml_part.h>
#include <khtmlview.h>
#include <kdialogbase.h>

#include "datablocks/recipelist.h"

class DietViewDialog: public KDialogBase
{
	Q_OBJECT

public:
	DietViewDialog( QWidget *parent, const RecipeList &recipeList, int dayNumber, int mealNumber, const Q3ValueList <int> &dishNumbers );
	~DietViewDialog();
private:
	// Widgets
	KHTMLPart *dietView;

	// Private methods
	void showDiet( const RecipeList &recipeList, int dayNumber, int mealNumber, const Q3ValueList <int> &dishNumbers );
private slots:
	void print( void );
	void slotOk( void );
signals:
	void signalOk( void );
};

#endif
