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

#ifndef RESIZERECIPEDIALOG_H
#define RESIZERECIPEDIALOG_H

#include <kdialogbase.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3Frame>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Recipe;

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class FractionInput;
class Q3ButtonGroup;
class Q3Frame;
class QLabel;
class KIntNumInput;
class KLineEdit;
class QRadioButton;
class QPushButton;

/**
  *@author Jason Kivlighn
  */
class ResizeRecipeDialog : public KDialogBase
{
	Q_OBJECT

public:
	ResizeRecipeDialog( QWidget *parent, Recipe* );

protected slots:
	void accept();
	void activateCurrentOption( int );
	virtual void languageChange();

private:
	void resizeRecipe( double factor );

	Recipe *m_recipe;

	Q3ButtonGroup* buttonGroup;
	QRadioButton* yieldRadioButton;
	Q3Frame* yieldFrame;
	QLabel* currentYieldLabel;
	QLabel* newYieldLabel;
	KLineEdit* currentYieldInput;
	FractionInput* newYieldInput;
	QRadioButton* factorRadioButton;
	Q3Frame* factorFrame;
	QLabel* factorLabel;
	FractionInput* factorInput;

	Q3VBoxLayout* buttonGroupLayout;
	Q3GridLayout* yieldFrameLayout;
	Q3HBoxLayout* factorFrameLayout;
};

#endif //RESIZERECIPEDIALOG_H
