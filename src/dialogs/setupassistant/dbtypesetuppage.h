/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                    *
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>             *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef DBTYPESETUPPAGE_H
#define DBTYPESETUPPAGE_H

#include "setupassistant.h"

#include <QWidget>

class QLabel;
class QGroupBox;
class QRadioButton;


class DBTypeSetupPage: public QWidget
{
	Q_OBJECT

public:
	// Methods
	DBTypeSetupPage( QWidget *parent );
	int dbType( void );
private:
	// Widgets
	QLabel *dbTypeSetupText;
	QLabel *logo;
	QGroupBox *bg;
	QRadioButton *liteCheckBox;
	QRadioButton *mysqlCheckBox;
	QRadioButton *psqlCheckBox;
private slots:
	void setSQLitePages();
	void setMySQLPages();
	void setPostgreSQLPages();
signals:
	void showPages( DBType );
};

#endif
