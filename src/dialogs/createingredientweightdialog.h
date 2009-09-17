/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CREATEINGREDIENTWEIGHTDIALOG_H
#define CREATEINGREDIENTWEIGHTDIALOG_H

#include <kdialog.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class Q3GroupBox;
class QLabel;

class FractionInput;
class UnitComboBox;
class PrepMethodComboBox;
class RecipeDB;
class Weight;

class CreateIngredientWeightDialog : public KDialog
{
Q_OBJECT
	
public:
	CreateIngredientWeightDialog( QWidget* parent, RecipeDB* );
	~CreateIngredientWeightDialog();

	Weight weight() const;

protected:
	QGridLayout* groupBox1Layout;
	
protected slots:
	virtual void languageChange();
	void slotOk();

private:
	Q3GroupBox* groupBox1;
	FractionInput* perAmountEdit;
	FractionInput* weightEdit;
	UnitComboBox* weightUnitBox;
	QLabel* perAmountLabel;
	QLabel* weightLabel;
	UnitComboBox* perAmountUnitBox;
	PrepMethodComboBox* prepMethodBox;
};

#endif // CREATEINGREDIENTWEIGHTDIALOG_H
