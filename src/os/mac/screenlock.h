#ifndef __MAC_SCREENLOCK_H__
#define __MAC_SCREENLOCK_H__

#include <functional>

namespace pws_os {
  void MacInitializeScreenLockListener(std::function<void()> listener);
  void MacFinalizeScreenLockListener();
};

#endif
