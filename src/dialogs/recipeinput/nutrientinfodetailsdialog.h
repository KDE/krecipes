/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef NUTRIENTINFODETAILSDIALOG_H
#define NUTRIENTINFODETAILSDIALOG_H

#include <QDialog>

namespace Ui {
	class NutrientInfoDetailsDialog;
}

class NutrientInfoDetailsDialog : public QDialog
{
	Q_OBJECT

public:
	NutrientInfoDetailsDialog( QWidget *parent );

private:
	Ui::NutrientInfoDetailsDialog * ui;
};

#endif
