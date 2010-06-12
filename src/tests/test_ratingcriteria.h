/***************************************************************************
 *   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef TEST_RATINGCRITERIA_H
#define TEST_RATINGCRITERIA_H

#include <QtCore/QObject>

class Test_RatingCriteria : public QObject
{
    Q_OBJECT;

private slots:
    void testGetterSetter();
    void testCopyConstructionAndAssignment();
};

#endif // TEST_RATINGCRITERIA_H
