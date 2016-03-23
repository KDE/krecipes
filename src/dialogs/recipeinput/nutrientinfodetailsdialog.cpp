/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "nutrientinfodetailsdialog.h"

#include "ui_nutrientinfodetailsdialog.h"

NutrientInfoDetailsDialog::NutrientInfoDetailsDialog( QWidget *parent )
		: QDialog( parent )
{
	ui = new Ui::NutrientInfoDetailsDialog;
	QWidget * mainWidget = new QWidget( this );
	ui->setupUi( mainWidget );

	connect( ui->m_closeButton, SIGNAL(clicked()),
		this, SLOT(hide()) );
}

