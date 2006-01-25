/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef BORDERDIALOG_H
#define BORDERDIALOG_H

#include <qdialog.h>

#include <kcolordialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class KListBox;
class QListBoxItem;
class QSpinBox;
class KHTMLPart;

class KreBorder;

class BorderDialog : public QDialog
{
	Q_OBJECT

public:
	BorderDialog( const KreBorder &border, QWidget* parent = 0, const char* name = 0 );
	~BorderDialog();

	KreBorder border() const;

	QGroupBox* borderGroupBox;
	QLabel* styleLabel;
	KListBox* styleListBox;
	QLabel* colorLabel;
	KHSSelector* hsSelector;
	KValueSelector* valuePal;
	QLabel* widthLabel;
	QSpinBox* widthSpinBox;
	KListBox* widthListBox;
	KHTMLPart* borderPreview;

protected:
	QVBoxLayout* BorderDialogLayout;
	QVBoxLayout* borderGroupBoxLayout;
	QHBoxLayout* layout4;
	QVBoxLayout* layout3;
	QVBoxLayout* layout2;
	QVBoxLayout* layout1;

protected slots:
	virtual void languageChange();
	void updatePreview();
	void updateSpinBox( int index );

	void slotHSChanged( int h, int s );
	void slotVChanged( int v );
	void setColor( const KColor &color );

private:
	void loadBorder( const KreBorder &border );
	void initListBoxs();

	KColor selColor;

};

#endif // BORDERDIALOG_H
