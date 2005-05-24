/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DUALPROGRESSDIALOG_H
#define DUALPROGRESSDIALOG_H

#include <kprogress.h>

#include <qobject.h>
//Added by qt3to4:
#include <QLabel>

class QLabel;

/** @brief A dialog that presents the user with a progress bar and a sub progress bar
  *
  * Set the total number of steps in the total progress bar, and then incrementing the progress
  * in the sub progress bar will be increment the total progress bar accordingly.
  * @author Jason Kivlighn
  */
class DualProgressDialog : public KProgressDialog
{
	Q_OBJECT

public:
	DualProgressDialog( QWidget* parent = 0, const char* name = 0, const QString& caption = QString::null, const QString& text = QString::null, const QString &sub_text = QString::null, bool modal = false );

	KProgress *subProgressBar()
	{
		return m_sub_progress_bar;
	};
	void setSubLabel( const QString & );
	void setTotalSteps( int steps );
	QString subLabelText();

private:
	KProgress *m_sub_progress_bar;
	QLabel *m_sub_label;
	int m_current_step;
	int m_total_steps;

	//make this private
	KProgress * progressBar()
	{
		return KProgressDialog::progressBar();
	}

private slots:
	void slotUpdateTotalProgress( int percentage );
};

#endif //DUALPROGRESSDIALOG_H
