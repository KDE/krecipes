/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DBIMPORTDIALOG_H
#define DBIMPORTDIALOG_H

#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QButtonGroup;
class QRadioButton;
class QWidgetStack;
class QWidget;
class QLineEdit;
class QLabel;
class QPushButton;

class KURLRequester;

/**
@author Jason Kivlighn
*/

class DBImportDialog:public QDialog{
Q_OBJECT
public:
	DBImportDialog(QWidget *parent=0, const char *name=0, bool modal=true, WFlags f=0);
	~DBImportDialog();

	QString dbType() const;
	void serverParams(QString &host, QString &user, QString &pass, QString &table) const;
	QString dbFile() const;

private:
	// Widgets
	QButtonGroup* dbButtonGroup;
	QRadioButton* liteRadioButton;
	QRadioButton* mysqlRadioButton;
	QRadioButton* psqlRadioButton;
	QWidgetStack* paramStack;
	QWidget* serverPage;
	QLineEdit* nameEdit;
	QLabel* passwordLabel;
	QLineEdit* hostEdit;
	QLineEdit* passwordEdit;
	QLineEdit* userEdit;
	QLabel* userLabel;
	QLabel* hostLabel;
	QLabel* nameLabel;
	QWidget* sqlitePage;
	KURLRequester* sqliteDBRequester;
	QPushButton* buttonOk;
	QPushButton* buttonCancel;

	QHBoxLayout* MyDialogLayout;
	QVBoxLayout* dbButtonGroupLayout;
	QVBoxLayout* serverPageLayout;
	QGridLayout* layout5;
	QVBoxLayout* serverPageLayout_2;
	QVBoxLayout* Layout5;
	QSpacerItem* Spacer1;

protected slots:
	void languageChange();
	void switchDBPage(int id);

};

#endif
