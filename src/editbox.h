/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
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

class RatioInput;

class EditBox:public QWidget{
Q_OBJECT
public:

     EditBox(QWidget* parent);
    ~EditBox();
    //Methods
    double value(void);
    void setValue(double newValue);
    void setPrecision(int dec);
    void setRange(double min,double max,double step,bool slider);

    // Variables
    bool accepted; // Indicates if the current value has been accepted (ok) or not
private:
    // Widgets
    RatioInput *editBox;
    QPushButton *okButton;

signals:
    void valueChanged(double);
private slots:
    void acceptValue(void);

};

class RatioInput:public KDoubleNumInput{
Q_OBJECT
public:
	RatioInput(QWidget *parent);

signals:
	void valueAccepted(void);
protected:
	void keyPressEvent ( QKeyEvent * e );
};

#endif
