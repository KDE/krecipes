/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Based on kstartuplogo from Umbrello http://uml.sourceforge.net        *
 ***************************************************************************/

#include "kstartuplogo.h"

KStartupLogo::KStartupLogo(QWidget * parent, const char *name) : QWidget(parent,name, WStyle_NoBorderEx | WStyle_Customize | WDestructiveClose ),m_bReadyToHide(false) {
  QString dataDir = locate("data", "krecipes/pics/startlogo.png");
  QPixmap pm(dataDir);
  setBackgroundPixmap(pm);
  setGeometry(QApplication::desktop()->width()/2-pm.width()/2, QApplication::desktop()->height()/2-pm.height()/2, pm.width(),pm.height());
}

KStartupLogo::~KStartupLogo() {}

void KStartupLogo::mousePressEvent( QMouseEvent*) {
  if (m_bReadyToHide){
    hide();
  }
}
#include "kstartuplogo.moc"


