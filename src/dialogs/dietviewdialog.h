/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DIETVIEWDIALOG_H
#define DIETVIEWDIALOG_H

#include <QPushButton>

#include <qwidget.h>

#include <khtml_part.h>
#include <khtmlview.h>
#include <kdialog.h>
#include <kvbox.h>

#include "datablocks/recipelist.h"

class DietViewDialog: public KDialog
{
	Q_OBJECT

public:
	DietViewDialog( QWidget *parent, const RecipeList &recipeList, int dayNumber, int mealNumber, const QList <int> &dishNumbers );
	~DietViewDialog();
private:
	// Widgets
	KHTMLPart *dietView;

	// Private methods
	void showDiet( const RecipeList &recipeList, int dayNumber, int mealNumber, const QList <int> &dishNumbers );
private slots:
	void print( void );
	void slotOk( void );
signals:
	void signalOk( void );
};

#endif
