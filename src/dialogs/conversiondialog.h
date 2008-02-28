/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CONVERSIONDIALOG_H
#define CONVERSIONDIALOG_H

#include <kdialog.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <Q3GridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QHBoxLayout;
class Q3GridLayout;
class QLabel;

class RecipeDB;
class UnitComboBox;
class IngredientComboBox;
class PrepMethodComboBox;
class FractionInput;

class ConversionDialog : public KDialog
{
Q_OBJECT
	
public:
	ConversionDialog( QWidget* parent, RecipeDB *, const char* name = 0 );
	~ConversionDialog();

	virtual void show();
	void reset();

protected:	
	IngredientComboBox* ingredientBox;
	PrepMethodComboBox* prepMethodBox;
	QLabel* convertLabel;
	UnitComboBox* toUnitBox;
	UnitComboBox* fromUnitBox;
	FractionInput* amountEdit;
	QLabel* toLabel;
	QLabel* resultLabel;
	QLabel* resultText;
	
protected slots:
	virtual void languageChange();
	void slotUser1();
	void convert();

private:
	RecipeDB *m_database;
};

#endif // CONVERSIONDIALOG_H
