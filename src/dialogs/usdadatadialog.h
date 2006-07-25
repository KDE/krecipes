/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef USDADATADIALOG_H
#define USDADATADIALOG_H

#include <kdialogbase.h>

#include <qstringlist.h>
#include <qvaluelist.h>

#include "datablocks/element.h"

class RecipeDB;

class KListView;
class KLineEdit;

class USDADataDialog : public KDialogBase
{
	Q_OBJECT

public:
	USDADataDialog( const Element &, RecipeDB *database, QWidget *parent = 0 );
	~USDADataDialog();

private:
	void loadDataFromFile();

	KListView *listView;
	KLineEdit *search_edit;

	Element ingredient;
	RecipeDB *database;

	QValueList<QStringList> loaded_data;

private slots:
	void slotOk();
};

#endif //USDADATADIALOG_H
