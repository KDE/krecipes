/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "nukeconfigwidget.h"

#include "ui_nukeconfigwidget.h"

#include <KMessageBox>


NukeConfigWidget::NukeConfigWidget( QWidget * parent ) : 
	QWidget( parent ), 
	ui(new Ui::NukeConfigWidget)
{
	//Setup UI
	ui->setupUi( this );

	//Conect signals/slots.
	connect( ui->m_proceedButton, SIGNAL(clicked()), this, SLOT(proceed()) );
}

void NukeConfigWidget::proceed()
{
	KMessageBox::information( this, "Populating the database..." );
}

#include "nukeconfigwidget.moc"
