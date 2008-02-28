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

#include <kdialog.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QFrame>
#include <Q3GridLayout>
#include <QHBoxLayout>
#include <QLabel>

class Recipe;

class QVBoxLayout;
class QHBoxLayout;
class Q3GridLayout;
class FractionInput;
class Q3ButtonGroup;
class QFrame;
class QLabel;
class KLineEdit;
class QRadioButton;

/**
  *@author Jason Kivlighn
  */
class ResizeRecipeDialog : public KDialog
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
	QFrame* yieldFrame;
	QLabel* currentYieldLabel;
	QLabel* newYieldLabel;
	KLineEdit* currentYieldInput;
	FractionInput* newYieldInput;
	QRadioButton* factorRadioButton;
	QFrame* factorFrame;
	QLabel* factorLabel;
	FractionInput* factorInput;

	QVBoxLayout* buttonGroupLayout;
	Q3GridLayout* yieldFrameLayout;
	QHBoxLayout* factorFrameLayout;
};

#endif //RESIZERECIPEDIALOG_H
