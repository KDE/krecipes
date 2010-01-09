/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CREATEELEMENTDIALOG_H
#define CREATEELEMENTDIALOG_H

#include <kdialog.h>

class KLineEdit;

/**
@author Unai Garro
*/
class CreateElementDialog : public KDialog
{
	Q_OBJECT
public:
	CreateElementDialog( QWidget *parent, const QString &text );
	~CreateElementDialog();
	QString newElementName() const;
protected slots:
	void slotTextChanged( const QString& text );

private:
	KLineEdit *elementEdit;
};

#endif
