/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CREATEINGREDIENTWEIGHTDIALOG_H
#define CREATEINGREDIENTWEIGHTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class KLineEdit;
class KComboBox;
class QLabel;
class QPushButton;

class FractionInput;
class UnitComboBox;
class RecipeDB;
class Weight;

class CreateIngredientWeightDialog : public QDialog
{
Q_OBJECT
	
public:
	CreateIngredientWeightDialog( QWidget* parent, RecipeDB* );
	~CreateIngredientWeightDialog();

	Weight weight() const;

protected:
	QVBoxLayout* CreateIngredientWeightDialogLayout;
	QGridLayout* groupBox1Layout;
	QHBoxLayout* layout1;
	QSpacerItem* spacer1;
	
protected slots:
	virtual void languageChange();
	void validateAndAccept();

private:
	QGroupBox* groupBox1;
	FractionInput* perAmountEdit;
	FractionInput* weightEdit;
	UnitComboBox* weightUnitBox;
	QLabel* perAmountLabel;
	QLabel* weightLabel;
	UnitComboBox* perAmountUnitBox;
	QPushButton* cancelButton;
	QPushButton* okButton;
};

#endif // CREATEINGREDIENTWEIGHTDIALOG_H
