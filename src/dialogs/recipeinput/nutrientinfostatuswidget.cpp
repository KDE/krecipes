/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "nutrientinfostatuswidget.h"

#include "ui_nutrientinfostatuswidget.h"


NutrientInfoStatusWidget::NutrientInfoStatusWidget( QWidget * parent )
	: QWidget(parent)
{
	ui = new Ui::NutrientInfoStatusWidget;
	ui->setupUi( this );

	connect( ui->m_detailsButton, SIGNAL(clicked()),
		this, SIGNAL(detailsButtonClicked()) );
}
