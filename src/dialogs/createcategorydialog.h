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

#ifndef CREATECATEGORYDIALOG_H
#define CREATECATEGORYDIALOG_H

#include <qmap.h>
#include <kdialog.h>
#include "datablocks/elementlist.h"

class KLineEdit;
class QGroupBox;
class KComboBox;

/**
@author Jason Kivlighn
*/
class CreateCategoryDialog : public KDialog
{
public:
	CreateCategoryDialog( QWidget *parent, const ElementList &categories );
	~CreateCategoryDialog();
	QString newCategoryName( ) const;
	int subcategory( void );

private:
	void loadCategories( const ElementList &categories );

	//Widgets
	QGroupBox *box;
	KLineEdit *elementEdit;
	KComboBox* categoryComboBox;
	QMap<QString, int> idMap;

};

#endif
