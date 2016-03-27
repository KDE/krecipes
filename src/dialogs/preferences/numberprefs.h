/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003, 2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef NUMBERPREFS_H
#define NUMBERPREFS_H

#include <QWidget>

class QGroupBox;
class QRadioButton;
class QVBoxLayout;
class QCheckBox;

class NumbersPrefs : public QWidget
{
	Q_OBJECT

public:
	NumbersPrefs( QWidget *parent = 0 );

	void saveOptions();

protected:
	QGroupBox* numberButtonGroup;
	QRadioButton* fractionRadioButton;
	QRadioButton* decimalRadioButton;

	QVBoxLayout* Form1Layout;
	QVBoxLayout* numberButtonGroupLayout;

	QCheckBox *abbrevButton;

protected slots:
	virtual void languageChange();

};

#endif
