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

#include <Kaleidoscope-Focus.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

namespace kaleidoscope {

char Focus::command_[32];
Focus::HookNode *Focus::root_node_;

void Focus::drain(void) {
  if (Serial.available())
    while (Serial.peek() != '\n')
      Serial.read();
}

void Focus::addHook(HookNode *new_node) {
  if (!root_node_) {
    root_node_ = new_node;
  } else {
    HookNode *node = root_node_;

    while (node->next) {
      node = node->next;
    }
    node->next = new_node;
  }
}

EventHandlerResult Focus::beforeReportingState() {
  if (Serial.available() == 0)
    return EventHandlerResult::OK;

  uint8_t i = 0;
  do {
    command_[i++] = Serial.read();

    if (Serial.peek() == '\n')
      break;
  } while (command_[i - 1] != ' ' && i < 32);
  if (command_[i - 1] == ' ')
    command_[i - 1] = '\0';
  else
    command_[i] = '\0';

  for (HookNode *node = root_node_; node; node = node->next) {
    if ((*node->handler)(command_)) {
      break;
    }
  }

  Serial.println(F("."));

  drain();

  if (Serial.peek() == '\n')
    Serial.read();

  return EventHandlerResult::OK;
}

void Focus::printSpace(void) {
  Serial.print(F(" "));
}

void Focus::printNumber(uint16_t num) {
  Serial.print(num);
}

void Focus::printColor(uint8_t r, uint8_t g, uint8_t b) {
  printNumber(r);
  printSpace();
  printNumber(g);
  printSpace();
  printNumber(b);
}

void Focus::printSeparator(void) {
  Serial.print(F(" | "));
}

void Focus::printBool(bool b) {
  Serial.print((b) ? F("true") : F("false"));
}

bool Focus::helpHook(const char *command) {
  if (strcmp_P(command, PSTR("help")) != 0)
    return false;

  for (const HookNode *node = root_node_; node; node = node->next) {
    if (!node->docs)
      continue;

    Serial.println(node->docs);
  }

  return true;
}

bool Focus::versionHook(const char *command) {
  if (strcmp_P(command, PSTR("version")) != 0)
    return false;

  Serial.print(F("Kaleidoscope/"));
  Serial.print(F(VERSION));
  printSpace();
  Serial.print(F(USB_MANUFACTURER));
  Serial.print(F("/"));
  Serial.print(F(USB_PRODUCT));
  printSeparator();
  Serial.print(F(__DATE__));
  printSpace();
  Serial.println(F(__TIME__));

  return true;
}

// Legacy V1 API
#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
void Focus::begin() {
  Kaleidoscope.useLoopHook(legacyLoopHook);
}

void Focus::legacyLoopHook(bool is_post_clear) {
  if (is_post_clear)
    return;

  ::Focus.beforeReportingState();
}
#endif

}

kaleidoscope::Focus Focus;
