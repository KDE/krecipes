/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CREATECATEGORYDIALOG_H
#define CREATECATEGORYDIALOG_H

#include <qdialog.h>
#include <qmap.h>

#include "elementlist.h"

class KLineEdit;
class QPushButton;
class QVBoxLayout;
class QGroupBox;
class QVBox;
class KComboBox;

/**
@author Jason Kivlighn
*/
class CreateCategoryDialog : public QDialog
{
public:
    CreateCategoryDialog(QWidget *parent,const ElementList &categories);
    ~CreateCategoryDialog();
    QString newCategoryName(void);
    int subcategory(void);

private:
  void loadCategories(const ElementList &categories);

  //Widgets
  QVBoxLayout *container;
  QGroupBox *box;
  QVBox *vbox;
  KLineEdit *elementEdit;
  QPushButton* okButton;
  QPushButton* cancelButton;
  KComboBox* categoryComboBox;
  QMap<QString,int> idMap;

};

#endif
