/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "createelementdialog.h"

CreateElementDialog::CreateElementDialog(const QString &text)
 : QDialog(0,0,true)
{

container=new QVBoxLayout(this,5,5);
box=new QGroupBox(this);
container->addWidget(box);
box->setTitle(text);
elementEdit=new KLineEdit(box);
elementEdit->setGeometry( QRect( 5, 30, 180, 25 ) );
okButton=new QPushButton(box);
okButton->setGeometry( QRect( 5, 60, 100, 20 ) );
okButton->setText("Ok");
cancelButton=new QPushButton(box);
cancelButton->setGeometry( QRect( 110, 60, 60, 20 ) );
cancelButton->setText("Cancel");
resize(QSize(200,100));

connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}


CreateElementDialog::~CreateElementDialog()
{
}

QString CreateElementDialog::newElementName(void)
{
return(elementEdit->text());
}

