#ifndef SING_DLD_H
#define SING_DLD_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sing_dld.h,v 1.2 1999-11-15 17:20:49 obachman Exp $ */
/*
* ABSTRACT: dynamic loader
*/

#ifdef HAVE_DLD
  sleftv * iiMake_binary(idhdl pn, sleftv* sl);
  BOOLEAN dlLoad(char * fn, char *pn);
  #ifdef linux
    #ifdef __ELF__
      #define dlInit(A)
    #else
      void    dlInit(const char *thisfile);
    #endif
  #else
    #define   dlInit(A)
  #endif
  BOOLEAN dlUnload(char * fn, char *pn);
#else
  #define   dlInit(A)
#endif
#endif
