/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef EDITBOX_H
#define EDITBOX_H
#include <qpushbutton.h>
#include <qhbox.h>
#include <knuminput.h>


/**
@author Unai Garro
*/

class EditBox:public QWidget{
Q_OBJECT
public:

     EditBox(QWidget* parent);
    ~EditBox();
private:
    KDoubleNumInput *editBox;
    QPushButton *okButton;
signals:
    void valueChanged(double);
private slots:
    void acceptValue(void);

};

#endif
