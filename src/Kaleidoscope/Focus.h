/* -*- mode: c++ -*-
 * Kaleidoscope-Focus -- Bidirectional communication plugin
 * Copyright (C) 2017, 2018  Gergely Nagy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Kaleidoscope.h>

#if FOCUS_WITHOUT_DOCS
#define FOCUS_HOOK(n, d) ({                               \
      static kaleidoscope::Focus::HookNode _c = {         \
        &n, NULL, NULL};                                  \
      &_c;                                                \
    })
#else
#define FOCUS_HOOK(n, d) ({                               \
      static kaleidoscope::Focus::HookNode _c = {         \
        &n, F(d), NULL};                                  \
      &_c;                                                \
    })
#endif

namespace kaleidoscope {
class Focus : public kaleidoscope::Plugin {
 public:
  typedef bool (*Hook)(const char *command);
  typedef struct HookNode {
    Hook handler;
    const __FlashStringHelper *docs;
    HookNode *next;
  } HookNode;

  Focus(void) {}

  static void addHook(HookNode *new_node);

  /* Helpers */
  static void printNumber(uint16_t number);
  static void printSpace(void);
  static void printColor(uint8_t r, uint8_t g, uint8_t b);
  static void printSeparator(void);
  static void printBool(bool b);

  static void readColor(cRGB &color);

  /* Hooks */
  static bool helpHook(const char *command);
  static bool versionHook(const char *command);

  EventHandlerResult beforeReportingState();

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
  kaleidoscope::EventHandlerResult onSetup() {
    return kaleidoscope::EventHandlerResult::OK;
  }

 protected:
  void begin();
  static void legacyLoopHook(bool is_post_clear);
#endif

 private:
  static HookNode *root_node_;
  static char command_[32];

  static void drain(void);
};
};

extern kaleidoscope::Focus Focus;

#define FOCUS_HOOK_HELP    FOCUS_HOOK(Focus.helpHook, "help")
#define FOCUS_HOOK_VERSION FOCUS_HOOK(Focus.versionHook, "version")
