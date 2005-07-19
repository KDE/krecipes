#ifndef PROFILING_H
#define PROFILING_H

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef KRECIPES_PROFILING
  #include <qdatatime.h>
  #include <kdebug.h>
  QTime dbg_timer;

  #define START_TIMER(MSG) \
   dbg_timer.start(); kdDebug()<<MSG<<endl;
  #define END_TIMER() \
   kdDebug()<<"...took "<<dbg_timer.elapsed()<<" ms"<<endl;
#else
  #define START_TIMER(MSG)
  #define END_TIMER()
#endif

#endif //PROFILING_H
