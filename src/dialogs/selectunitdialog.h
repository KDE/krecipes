/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
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
#include <qpushbutton.h>

/**
@author Unai Garro
*/
class SelectUnitDialog : public QDialog
{
public:
    SelectUnitDialog(QWidget* parent=0);

    ~SelectUnitDialog();
private:
  QPushButton *ok_button;

};

#endif
