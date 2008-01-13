/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTPARSERDIALOG_H
#define INGREDIENTPARSERDIALOG_H

#include <kdialog.h>

#include "datablocks/ingredientlist.h"
#include "datablocks/unit.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3PtrList>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QLabel;
class Q3TextEdit;
class QLineEdit;
class KPushButton;
class K3ListView;
class Q3ListViewItem;
class QPushButton;

class IngredientParserDialog : public KDialog
{
	Q_OBJECT

public:
	IngredientParserDialog( const UnitList &units, QWidget* parent = 0, const char* name = 0 );
	~IngredientParserDialog();
	
	IngredientList ingredients() const { return m_ingList; }

protected:
	QLabel* textLabel1;
	QLabel *previewLabel;
	Q3TextEdit* ingredientTextEdit;
	KPushButton* parseButton;
	K3ListView* previewIngView;
	QPushButton* buttonGroup;

protected slots:
	virtual void accept();
	void parseText();
	void removeIngredient();

	//Set Header button slot
	virtual void convertToHeader();
	virtual void languageChange();

private:
	void convertToHeader( const Q3PtrList<Q3ListViewItem> &items );

	UnitList m_unitList;
	IngredientList m_ingList;
};

#endif // INGREDIENTPARSERDIALOG_H
