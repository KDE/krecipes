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
#include <kdialogbase.h>

#include "datablocks/unit.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class KListView;
class QListViewItem;
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
	KListView* propertyListView;
	QLabel* weightLabel;
	QPushButton* weightAddButton;
	QPushButton* weightRemoveButton;
	KListView* weightListView;

protected:
	QVBoxLayout* EditPropertiesDialogLayout;
	QHBoxLayout* layout9;
	QVBoxLayout* layout8;
	QVBoxLayout* layout7;
	QHBoxLayout* layout3;
	QHBoxLayout* layout3_2;

protected slots:
	virtual void languageChange();

private slots:
	void addWeight();
	void removeWeight();
	void updateLists( void );
	void addPropertyToIngredient( void );
	void removePropertyFromIngredient( void );
	void insertPropertyEditBox( QListViewItem* it );
	void setPropertyAmount( double amount );
	void loadUSDAData( void );
	void itemRenamed( QListViewItem*, const QPoint &, int col );
	void loadDataFromFile();

private:
	void reloadPropertyList( void );
	void reloadWeightList( void );
	int findPropertyNo( QListViewItem *it );

	KDoubleNumInput* inputBox;

	ElementList *perUnitListBack;
	RecipeDB *db;
	QValueList<QStringList> loaded_data;

	int ingredientID;
	QString ingredientName;
};

#endif // EDITPROPERTIESDIALOG_H
