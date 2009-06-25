/***************************************************************************
    Copyright (C) 2003-2009 Robby Stephenson <robby@periapsis.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

/* File obtained from tellico, developed in KDE svn:
http://websvn.kde.org/trunk/extragear/office/tellico/src/gui/ratingwidget.h?view=log
This file was developed by Robby Stephenson and slightly modified by in order to
including it in krecipes and to allow ratings having n stars and a half.*/

#ifndef RATINGWIDGET_H
#define RATINGWIDGET_H

#include <KHBox>

#include <QList>
#include <QLabel>
#include <QPixmap>

/**
 * @author Robby Stephenson
 */
class RatingWidget : public KHBox {
Q_OBJECT

typedef QList<QLabel*> LabelList;

public:
  RatingWidget(int stars, QWidget* parent);

  void clear();
  QString text() const;
  void setText(const QString& text);

  static const QPixmap& pixmap(const QString& value);

public slots:
  void update();

signals:
  void signalModified();

protected:
  virtual void mousePressEvent(QMouseEvent* e);

private:
  void init();
  void updateBounds();

  LabelList m_widgets;

  int m_currIndex;
  int m_total;
  int m_min;
  int m_max;

  QPixmap m_pixOn;
  QPixmap m_pixOff;
  QPixmap m_pixHalf;
};

#endif
