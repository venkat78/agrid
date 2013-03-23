
#ifndef _GEOM_MISC_TEMPLATES_H_
#define _GEOM_MISC_TEMPLATES_H_

inline BOOL ValInRange(REAL lowerLimit, REAL upperLimit, REAL val)
{
  if(lowerLimit <= val && val <= upperLimit)
    return true;

  return false;
}

#endif //_GEOM_MISC_TEMPLATES_H_

