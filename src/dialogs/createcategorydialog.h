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

#ifndef CREATECATEGORYDIALOG_H
#define CREATECATEGORYDIALOG_H

#include <qmap.h>
#include <QVBoxLayout>

#include <kdialog.h>
#include <kvbox.h>

#include "datablocks/elementlist.h"

class KLineEdit;
class QVBoxLayout;
class Q3GroupBox;
class KComboBox;

/**
@author Jason Kivlighn
*/
class CreateCategoryDialog : public KDialog
{
public:
	CreateCategoryDialog( QWidget *parent, const ElementList &categories );
	~CreateCategoryDialog();
	QString newCategoryName( void );
	int subcategory( void );

private:
	void loadCategories( const ElementList &categories );

	//Widgets
	Q3GroupBox *box;
	KLineEdit *elementEdit;
	KComboBox* categoryComboBox;
	QMap<QString, int> idMap;

};

#endif
