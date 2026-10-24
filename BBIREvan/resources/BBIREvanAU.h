
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
  #import <UIKit/UIKit.h>
#else
  #import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vBBIREvan
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vBBIREvan
#import <BBIREvanAU/IPlugAUAudioUnit.h>
#import <BBIREvanAU/IPlugAUViewController.h>

//! Project version number for BBIREvanAU.
FOUNDATION_EXPORT double BBIREvanAUVersionNumber;

//! Project version string for BBIREvanAU.
FOUNDATION_EXPORT const unsigned char BBIREvanAUVersionString[];

@class IPlugAUViewController_vBBIREvan;
