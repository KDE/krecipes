/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef BORDERDIALOG_H
#define BORDERDIALOG_H

#include <kdialog.h>
#include <kcolordialog.h>
#include <khuesaturationselect.h>
#include <kcolorvalueselector.h>


#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class Q3GroupBox;
class QLabel;
class K3ListBox;
class QSpinBox;
class KHTMLPart;

class KreBorder;

class BorderDialog : public KDialog
{
	Q_OBJECT

public:
	explicit BorderDialog( const KreBorder &border, QWidget* parent = 0, const char* name = 0 );
	~BorderDialog();

	KreBorder border() const;

	Q3GroupBox* borderGroupBox;
	QLabel* styleLabel;
	K3ListBox* styleListBox;
	QLabel* colorLabel;
	KHueSaturationSelector* hsSelector;
	KColorValueSelector* valuePal;
	QLabel* widthLabel;
	QSpinBox* widthSpinBox;
	K3ListBox* widthListBox;
	KHTMLPart* borderPreview;

protected:
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
	void setColor( const QColor &color );

private:
	void loadBorder( const KreBorder &border );
	void initListBoxs();

	QColor selColor;

};

#endif // BORDERDIALOG_H
