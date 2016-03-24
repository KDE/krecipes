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

void NutrientInfoStatusWidget::setStatus( NutrientInfo::Status status )
{
	if ( status == NutrientInfo::Complete ) {
		ui->m_statusLed->setColor( Qt::green );
		ui->m_statusLabel->setText( i18nc(
			"@info Property information for a recipe is complete",
			"Complete") );
		ui->m_detailsButton->setVisible(false);
	} else if ( status == NutrientInfo::Intermediate ) {
		ui->m_statusLed->setColor( Qt::yellow );
		ui->m_statusLabel->setText( i18nc(
			"@info Property information for a recipe is complete, but...",
			"Complete, but approximations made") );
		ui->m_detailsButton->setVisible(true);
	} else {
		ui->m_statusLed->setColor( Qt::red );
		ui->m_statusLabel->setText( i18nc(
			"@info Property information for a recipe is incomplete",
			"Incomplete") );
		ui->m_detailsButton->setVisible(true);
	}
}
