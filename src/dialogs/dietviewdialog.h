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
#include <qvbox.h>
#include <qwidget.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include "datablocks/recipelist.h"

class DietViewDialog: public QWidget
{
	Q_OBJECT

public:
	DietViewDialog( QWidget *parent, const RecipeList &recipeList, int dayNumber, int mealNumber, const QValueList <int> &dishNumbers );
	~DietViewDialog();
private:
	// Widgets
	QGridLayout *layout;
	QVBox *htmlBox;
	KHTMLPart *dietView;
	QHBox *buttonBox;
	QPushButton *okButton;
	QPushButton *printButton;
	QPushButton *cancelButton;

	// Private methods
	void showDiet( const RecipeList &recipeList, int dayNumber, int mealNumber, const QValueList <int> &dishNumbers );
private slots:
	void print( void );
	void slotOk( void );
signals:
	void signalOk( void );
};

#endif
