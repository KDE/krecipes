/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RESIZERECIPEDIALOG_H
#define RESIZERECIPEDIALOG_H

#include <kdialog.h>

class Recipe;

class QVBoxLayout;
class QFormLayout;
class FractionInput;
class Q3ButtonGroup;
class QFrame;
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
	KLineEdit* currentYieldInput;
	FractionInput* newYieldInput;
	QRadioButton* factorRadioButton;
	QFrame* factorFrame;
	FractionInput* factorInput;

	QVBoxLayout* buttonGroupLayout;
	QFormLayout* yieldFrameLayout;
	QFormLayout* factorFrameLayout;
};

#endif //RESIZERECIPEDIALOG_H
