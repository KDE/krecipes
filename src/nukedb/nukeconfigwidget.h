/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#ifndef NUKECONFIGWIDGET_H
#define NUKECONFIGWIDGET_H


#include <QWidget>

class RecipeDB;

namespace Ui {
	class NukeConfigWidget;
}


class NukeConfigWidget : public QWidget
{
	Q_OBJECT

public:
	NukeConfigWidget( QWidget * parent = 0 );

private slots:
	void proceed();

private:
	Ui::NukeConfigWidget * ui;
	
	RecipeDB * database;

};


#endif //NUKECONFIGWIDGET_H
