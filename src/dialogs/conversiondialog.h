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

#include <kdialogbase.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class KComboBox;
class QLabel;
class KLineEdit;
class QPushButton;

class RecipeDB;
class UnitComboBox;
class IngredientComboBox;
class FractionInput;

class ConversionDialog : public KDialogBase
{
Q_OBJECT
	
public:
	ConversionDialog( QWidget* parent, RecipeDB *, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~ConversionDialog();
	
	IngredientComboBox* ingredientBox;
	QLabel* convertLabel;
	UnitComboBox* toUnitBox;
	UnitComboBox* fromUnitBox;
	FractionInput* amountEdit;
	QLabel* toLabel;
	QPushButton* convertButton;
	QLabel* resultLabel;
	QLabel* resultText;
	
protected:
	QGridLayout* layout4;
	QHBoxLayout* layout6;
	QSpacerItem* Horizontal_Spacing2_2;
	QSpacerItem* Horizontal_Spacing2_3;
	QHBoxLayout* layout7;
	QHBoxLayout* Layout1;
	QSpacerItem* Horizontal_Spacing2;
	
protected slots:
	virtual void languageChange();
	void convert();

private:
	RecipeDB *m_database;
};

#endif // CONVERSIONDIALOG_H
