/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CREATEELEMENTDIALOG_H
#define CREATEELEMENTDIALOG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <klineedit.h>

/**
@author Unai Garro
*/
class CreateElementDialog : public QDialog
{
public:
    CreateElementDialog(const QString &text);

    ~CreateElementDialog();

private:
  //Widgets
  QVBoxLayout *container;
  QGroupBox *box;
  QVBox *vbox;
  KLineEdit *elementEdit;
  QPushButton* okButton;
  QPushButton* cancelButton;

};

#endif
