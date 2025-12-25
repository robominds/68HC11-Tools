#pragma once

/* Compatibility layer for DOS-specific headers on modern systems */

#ifdef _WIN32
  /* Windows - dos.h and bios.h may be available */
  #include <dos.h>
  #include <bios.h>
#else
  /* Non-Windows systems (Unix/Linux/macOS) */
  
  /* DOS-specific modifiers that don't exist on Unix/Linux/macOS */
  #define far
  #define _far
  #define _interrupt
  #define interrupt
  #define _pascal
  #define pascal
  
  /* Stub for biostime() function */
  static inline long biostime(int cmd, long newtime) {
    /* Simple stub - would need proper implementation for actual timing */
    (void)cmd;
    (void)newtime;
    return 0;
  }
  
#endif
