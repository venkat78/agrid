#include "defs.hpp"
#include "table.hpp"

namespace utils {
  BOOL debugOn = false;

  VOID SetDebugOn()
  {
    debugOn = true;
  }

  VOID SetDebugOff()
  {
    debugOn = false;
  }

  BOOL IsDebugOn()
  {
    return debugOn;
  }

}
