/***************************************************************************
*   Copyright (C) 2007 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef EDITPROPERTIESDIALOG_H
#define EDITPROPERTIESDIALOG_H

#include <qvariant.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>
#include <Q3ValueList>
#include <kdialogbase.h>

#include "datablocks/unit.h"

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class K3ListView;
class Q3ListViewItem;
class QPushButton;

class RecipeDB;
class KDoubleNumInput;
class ElementList;
class SelectUnitDialog;
class CreateElementDialog;
class IngredientPropertyList;
class SelectPropertyDialog;
class UnitsDialog;
class IngredientGroupsDialog;
class MixedNumber;
class KreListView;

class EditPropertiesDialog : public KDialogBase
{
	Q_OBJECT

public:
	EditPropertiesDialog( int ingID, const QString&ingName, RecipeDB *db, QWidget* parent = 0 );
	~EditPropertiesDialog();

	QLabel* infoLabel;
	KreListView* usdaListView;
	QPushButton* loadButton;
	QLabel* propertyLabel;
	QPushButton* propertyAddButton;
	QPushButton* propertyRemoveButton;
	K3ListView* propertyListView;
	QLabel* weightLabel;
	QPushButton* weightAddButton;
	QPushButton* weightRemoveButton;
	K3ListView* weightListView;

protected:
	Q3VBoxLayout* EditPropertiesDialogLayout;
	Q3HBoxLayout* layout9;
	Q3VBoxLayout* layout8;
	Q3VBoxLayout* layout7;
	Q3HBoxLayout* layout3;
	Q3HBoxLayout* layout3_2;

protected slots:
	virtual void languageChange();

private slots:
	void addWeight();
	void removeWeight();
	void updateLists( void );
	void addPropertyToIngredient( void );
	void removePropertyFromIngredient( void );
	void insertPropertyEditBox( Q3ListViewItem* it );
	void setPropertyAmount( double amount );
	void loadUSDAData( void );
	void itemRenamed( Q3ListViewItem*, const QPoint &, int col );
	void loadDataFromFile();

private:
	void reloadPropertyList( void );
	void reloadWeightList( void );
	int findPropertyNo( Q3ListViewItem *it );

	KDoubleNumInput* inputBox;

	ElementList *perUnitListBack;
	RecipeDB *db;
	Q3ValueList<QStringList> loaded_data;

	int ingredientID;
	QString ingredientName;
};

#endif // EDITPROPERTIESDIALOG_H
