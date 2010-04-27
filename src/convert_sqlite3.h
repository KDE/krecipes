/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CONVERT_SQLITE3_H
#define CONVERT_SQLITE3_H

#include <QObject>
#include <QProcess>

class KProcess;
class QString;
class QEventLoop;

class ConvertSQLite3 : public QObject
{
Q_OBJECT

public:
	ConvertSQLite3( const QString &db_file = QString() );
	void convert();
	~ConvertSQLite3();

private:
	bool copyFile( const QString &oldFilePath, const QString &newFilePath );

	bool error;
	QString db_file;
	bool m_process1Error;
	bool m_process2Error;
	bool m_process1Finished;
	bool m_process2Finished;
	int m_exitCode1;
	QProcess::ExitStatus m_exitStatus1;
	int m_exitCode2;
	QProcess::ExitStatus m_exitStatus2;
	QEventLoop *m_localEventLoop;

private slots:
	void process1Error();
	void process2Error();
	void process1Finished( int exitCode, QProcess::ExitStatus exitStatus );
	void process2Finished( int exitCode, QProcess::ExitStatus exitStatus );

};

#endif //CONVERT_SQLITE3_H
