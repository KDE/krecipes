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

#include <qvariant.h>
#include <qdialog.h>

#include "datablocks/ingredientlist.h"
#include "datablocks/unit.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QTextEdit;
class QLineEdit;
class KPushButton;
class KListView;
class QListViewItem;
class QPushButton;

class IngredientParserDialog : public QDialog
{
	Q_OBJECT

public:
	IngredientParserDialog( const UnitList &units, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~IngredientParserDialog();
	
	IngredientList ingredients() const { return m_ingList; }

protected:
	QLabel* textLabel1;
	QLabel *previewLabel;
	QTextEdit* ingredientTextEdit;
	KPushButton* parseButton;
	KListView* previewIngView;
	QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonHelp;
	QPushButton* buttonGroup;

	QHBoxLayout* IngredientParserDialogLayout;
	QVBoxLayout* layout4;
	QVBoxLayout* Layout5;
	QSpacerItem* Spacer1;

protected slots:
	void accept();
	void parseText();
	void removeIngredient();
	void convertToHeader();
	virtual void languageChange();

private:
	void convertToHeader( const QPtrList<QListViewItem> &items );

	UnitList m_unitList;
	IngredientList m_ingList;
};

#endif // INGREDIENTPARSERDIALOG_H
