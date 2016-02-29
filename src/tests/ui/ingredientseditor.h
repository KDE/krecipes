/***************************************************************************
 *   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TEST_INGREDIENTSEDITOR_H
#define TEST_INGREDIENTSEDITOR_H

#include <QtGui>
#include <QtTest/QtTest>

class Test_IngredientsEditor : public QObject
{
    Q_OBJECT;

private slots:
    void testLoadAndDumpList();
};

#endif
