/***************************************************************************
*   Copyright (C) 2003                                                    *
*                                                                         *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   Based on kstartuplogo from Umbrello http://uml.sourceforge.net        *
***************************************************************************/

#include "kstartuplogo.h"

#include <kconfig.h>
#include <kglobal.h>

#include <qcursor.h>

KStartupLogo::KStartupLogo( QWidget * parent, const char *name ) : QWidget( parent, name, WStyle_NoBorder | WStyle_Customize | WDestructiveClose ), m_bReadyToHide( false )
{
	QString dataDir = locate( "data", "krecipes/pics/startlogo.png" );
	QPixmap pm( dataDir );
	setBackgroundPixmap( pm );

	resize(pm.size());
	QRect desk = splashScreenDesktopGeometry();
	setGeometry( ( desk.width() / 2 ) - ( width() / 2 ) + desk.left(),
		( desk.height() / 2 ) - ( height() / 2 ) + desk.top(),
		width(), height() );
}

KStartupLogo::~KStartupLogo()
{}

void KStartupLogo::mousePressEvent( QMouseEvent* )
{
	if ( m_bReadyToHide ) {
		hide();
	}
}

/** This function is based on KDE's KGlobalSettings::splashScreenDesktopGeometry(),
  * which is not available in KDE 3.1.
  */
QRect KStartupLogo::splashScreenDesktopGeometry() const
{
	QDesktopWidget *dw = QApplication::desktop();
	
	if (dw->isVirtualDesktop()) {
		KConfigGroup group(KGlobal::config(), "Windows");
		int scr = group.readNumEntry("Unmanaged", -3);
		if (group.readBoolEntry("XineramaEnabled", true) && scr != -2) {
			if (scr == -3)
				scr = dw->screenNumber(QCursor::pos());
			return dw->screenGeometry(scr);
		}
		else {
			return dw->geometry();
		}
	}
	else {
		return dw->geometry();
	}
}

#include "kstartuplogo.moc"
