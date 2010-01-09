/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   Based on kstartuplogo from Umbrello http://uml.sourceforge.net        *
***************************************************************************/

#include "kstartuplogo.h"

#include <kconfiggroup.h>
#include <kglobal.h>

#include <QCursor>
#include <QMouseEvent>
#include <QPixmap>
#include <QDesktopWidget>

KStartupLogo::KStartupLogo( QWidget * parent, const char *name ) : QWidget( parent, Qt::WStyle_NoBorder | Qt::WStyle_Customize | Qt::WDestructiveClose ), m_bReadyToHide( false )
{
   setObjectName( name );
	QString dataDir = KStandardDirs::locate( "data", "krecipes/pics/startlogo.png" );
	QPixmap pm( dataDir );
   QPalette p = palette();
   p.setBrush(backgroundRole(), QBrush(pm));
   setPalette(p);	

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
		KConfigGroup group = KGlobal::config()->group( "Windows" );
		int scr = group.readEntry("Unmanaged", -3);
		if (group.readEntry("XineramaEnabled", true) && scr != -2) {
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
