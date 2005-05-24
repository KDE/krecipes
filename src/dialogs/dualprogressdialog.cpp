/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dualprogressdialog.h"

#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3Frame>

DualProgressDialog::DualProgressDialog( QWidget* parent, const char* name, const QString& caption, const QString& text, const QString &sub_text, bool modal ) : KProgressDialog( parent, name, caption, text, modal ),
		m_current_step( 0 ),
		m_total_steps( 1 )
{
	Q3Frame * mainWidget = plainPage();
	QLayout* layout = mainWidget->layout();

	m_sub_label = new QLabel( sub_text, mainWidget );
	layout->add
	( m_sub_label );

	m_sub_progress_bar = new KProgress( mainWidget );
	layout->add
	( m_sub_progress_bar );

	connect( m_sub_progress_bar, SIGNAL( percentageChanged( int ) ),
	         this, SLOT( slotUpdateTotalProgress( int ) ) );

	progressBar() ->setTotalSteps( 100 );

	slotAutoShow();
}

void DualProgressDialog::setSubLabel( const QString &text )
{
	m_sub_label->setText( text );
}

QString DualProgressDialog::subLabelText()
{
	return m_sub_label->text();
}

void DualProgressDialog::slotUpdateTotalProgress( int percentage )
{
	if ( percentage < 100 ) {
		int total_percent = int( ( double( m_current_step ) + double( percentage ) / 100.0 ) / double( m_total_steps ) * 100.0 );

		progressBar() ->setProgress( total_percent );
	}
	else {
		m_current_step++;
		m_sub_progress_bar->setProgress( 0 );
	}
}

void DualProgressDialog::setTotalSteps( int steps )
{
	m_total_steps = steps;
}

#include "dualprogressdialog.moc"
