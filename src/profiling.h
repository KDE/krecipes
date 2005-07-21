/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROFILING_H
#define PROFILING_H

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef KRECIPES_PROFILING
  #include <qdatetime.h>
  #include <kdebug.h>
  static QTime dbg_timer;

  #define START_TIMER(MSG) \
   dbg_timer.start(); kdDebug()<<MSG<<endl;
  #define END_TIMER() \
   kdDebug()<<"...took "<<dbg_timer.elapsed()<<" ms"<<endl;
#else
  #define START_TIMER(MSG)
  #define END_TIMER()
#endif

#endif //PROFILING_H
