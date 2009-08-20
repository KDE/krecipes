/****************************************************************************
** Form interface generated from reading ui file 'createunitconversiondialog.ui'
**
** Created: Thu Sep 13 19:55:39 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CREATEUNITCONVERSIONDIALOG_H
#define CREATEUNITCONVERSIONDIALOG_H

#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <kdialog.h>

#include "datablocks/elementlist.h"
#include "widgets/fractioninput.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class Q3ButtonGroup;
class QLabel;
class QComboBox;

class CreateUnitConversionDialog : public KDialog
{
	Q_OBJECT

public:
	CreateUnitConversionDialog( const Element &fromUnit, const ElementList &toUnits, QWidget* parent = 0 );
	~CreateUnitConversionDialog();

	int toUnitID() const;
	double ratio() const;

protected:
	QHBoxLayout* buttonGroup1Layout;

	Q3ButtonGroup* buttonGroup1;
	FractionInput* fromUnitEdit;
	QLabel* fromUnitLabel;
	QLabel* textLabel4;
	FractionInput* toUnitEdit;
	QComboBox* toUnitComboBox;

protected slots:
	virtual void languageChange();

private:
	ElementList m_toUnits;

};

#endif // CREATEUNITCONVERSIONDIALOG_H
