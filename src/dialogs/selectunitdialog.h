/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SELECTUNITDIALOG_H
#define SELECTUNITDIALOG_H

#include <qwidget.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qvbox.h>
#include <klistview.h>

class ElementList;

/**
@author Unai Garro
*/
class SelectUnitDialog : public QDialog
{
public:
    SelectUnitDialog(QWidget* parent,const ElementList &unitList);

    ~SelectUnitDialog();

    int unitID(void);

private:
  //Widgets
  QVBoxLayout *container;
  QGroupBox *box;
  QVBox *vbox;
  KListView *unitChooseView;
  QPushButton* okButton;
  QPushButton* cancelButton;
  void loadUnits(const ElementList &unitList);

};

#endif
