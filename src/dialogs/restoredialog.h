/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkvlighn@gmail.com)                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RESTOREDIALOG_H
#define RESTOREDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class KURLRequester;
class QPushButton;

class RestoreDialog : public QDialog
{
	Q_OBJECT
	
public:
	RestoreDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~RestoreDialog();
	
	QString backupFile() const;
	
protected:
	QVBoxLayout* RestoreDialogLayout;
	QHBoxLayout* layout3;
	QHBoxLayout* Layout1;
	QSpacerItem* Horizontal_Spacing2;
	
	QLabel* textLabel2;
	QLabel* textLabel1;
	KURLRequester* kURLRequester1;
	QPushButton* buttonHelp;
	QPushButton* buttonOk;
	QPushButton* buttonCancel;
	
protected slots:
	virtual void languageChange();

};

#endif // RESTOREDIALOG_H
