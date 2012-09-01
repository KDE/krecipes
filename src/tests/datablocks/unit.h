/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef TEST_UNIT_H
#define TEST_UNIT_H

#include <QtCore/QObject>

class Test_Unit : public QObject
{
    Q_OBJECT;

private slots:
    void testConstructor();
    void testGetterSetter();
    void testCopyConstructionAndAssignment();
    void testEqual();
    void testLesser();
    void testDetermineName();
};

#endif // TEST_UNIT_H
