/**
* @file oe.h
* @brief Extenstions to OS functions.
*/

#pragma once

#include "../types.h"
#include "settings.h"

namespace nn
{
   namespace oe
   {
       typedef s32 FocusHandlingMode;
       typedef s32 PerformanceMode;

       struct DisplayVersion {
           char name[16];
       };

       void Initialize();
       void SetPerformanceConfiguration(nn::oe::PerformanceMode, s32);
       s32 GetOperationMode();
       s32 GetPerformanceMode();
       void SetResumeNotificationEnabled(bool);
       void SetOperationModeChangedNotificationEnabled(bool);
       void SetPerformanceModeChangedNotificationEnabled(bool);
       void SetFocusHandlingMode(nn::oe::FocusHandlingMode);
       bool TryPopNotificationMessage(u32 *);
       s32 GetCurrentFocusState();
       void EnableGamePlayRecording(void *, u64);
       bool IsUserInactivityDetectionTimeExtended();
       void SetUserInactivityDetectionTimeExtended(bool);
       void FinishStartupLogo();
       nn::settings::LanguageCode GetDesiredLanguage();
       void GetDisplayVersion(DisplayVersion*);
       void GetDefaultDisplayResolution(u32 * width, u32 * height);
   };
};
