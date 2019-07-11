#include "screenlock.h"
#include <assert.h>
#import <Foundation/Foundation.h>

namespace pws_os {
  std::function<void()> sScreenLockListenerCallback;
}

@interface ScreenLockListener : NSObject
@end

@implementation ScreenLockListener

- (id)init
{
  if ((self = [super init]))
  {
    [[NSDistributedNotificationCenter defaultCenter]
      addObserver:self
      selector:@selector(onScreenIsLocked)
      name:@"com.apple.screenIsLocked"
      object:nil];
  }
  return self;
}

- (void)dealloc
{
  [[NSDistributedNotificationCenter defaultCenter]
    removeObserver:self
    name:@"com.apple.screenIsLocked"
    object:nil];
  [super dealloc];
}

- (void)onScreenIsLocked
{
  pws_os::sScreenLockListenerCallback();
}

@end

namespace pws_os {

  static ScreenLockListener *sScreenLockListener = nil;

  void MacInitializeScreenLockListener(std::function<void()> listener)
  {
    assert(!sScreenLockListener && "MacInitializeScreenLockListener() should only be called once.");
    sScreenLockListenerCallback = listener;
    sScreenLockListener = [[ScreenLockListener alloc] init];
  }

  void MacFinalizeScreenLockListener()
  {
    assert(sScreenLockListener && "Screen lock listener is not initialized.");
    sScreenLockListenerCallback = nullptr;
#if !__has_feature(objc_arc)
    [sScreenLockListener release];
#endif
    sScreenLockListener = nil;
  }
}
