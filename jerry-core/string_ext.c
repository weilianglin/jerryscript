/* Copyright 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "jerry.h"
#include "string_ext.h"

//
// strings in built-in scripts
//
#define JERRY_MAGIC_STRING_ITEMS \
  MAGICSTR_EX_DEF1(add) \
  MAGICSTR_EX_DEF1(addWidget) \
  MAGICSTR_EX_DEF1(ani) \
  MAGICSTR_EX_DEF1(Animation) \
  MAGICSTR_EX_DEF1(AnimationProperty) \
  MAGICSTR_EX_DEF1(anim_prop) \
  MAGICSTR_EX_DEF1(btn) \
  MAGICSTR_EX_DEF1(btn1_click_cb) \
  MAGICSTR_EX_DEF1(btn2_click_cb) \
  MAGICSTR_EX_DEF1(btn_author1_1) \
  MAGICSTR_EX_DEF1(btn_author2_1) \
  MAGICSTR_EX_DEF1(Button) \
  MAGICSTR_EX_DEF1(child) \
  MAGICSTR_EX_DEF1(children) \
  MAGICSTR_EX_DEF1(click) \
  MAGICSTR_EX_DEF1(conf) \
  MAGICSTR_EX_DEF1(createAnimation) \
  MAGICSTR_EX_DEF1(createAnimationProperty) \
  MAGICSTR_EX_DEF1(createButton) \
  MAGICSTR_EX_DEF1(createIcon) \
  MAGICSTR_EX_DEF1(createLabel) \
  MAGICSTR_EX_DEF1(createMoveProperty) \
  MAGICSTR_EX_DEF1(createView) \
  MAGICSTR_EX_DEF1(display) \
  MAGICSTR_EX_DEF1(duration) \
  MAGICSTR_EX_DEF1(dx) \
  MAGICSTR_EX_DEF1(dy) \
  MAGICSTR_EX_DEF1(e) \
  MAGICSTR_EX_DEF1(error_widget_type) \
  MAGICSTR_EX_DEF1(EVENT_CLICK) \
  MAGICSTR_EX_DEF1(EVENT_DOUBLE_CLICK) \
  MAGICSTR_EX_DEF1(EVENT_DOWN) \
  MAGICSTR_EX_DEF1(EVENT_GESTURE) \
  MAGICSTR_EX_DEF1(EVENT_MOVE) \
  MAGICSTR_EX_DEF1(EVENT_UNKOWN) \
  MAGICSTR_EX_DEF1(EVENT_UP) \
  MAGICSTR_EX_DEF1(final_position) \
  MAGICSTR_EX_DEF1(font) \
  MAGICSTR_EX_DEF1(FontCond20) \
  MAGICSTR_EX_DEF1(FontCond67) \
  MAGICSTR_EX_DEF1(getAbsoluteX) \
  MAGICSTR_EX_DEF1(GUI) \
  MAGICSTR_EX_DEF1(height) \
  MAGICSTR_EX_DEF1(hr) \
  MAGICSTR_EX_DEF1(hr_callback) \
  MAGICSTR_EX_DEF1(icn) \
  MAGICSTR_EX_DEF1(Icon) \
  MAGICSTR_EX_DEF1(id) \
  MAGICSTR_EX_DEF1(image) \
  MAGICSTR_EX_DEF1(images) \
  MAGICSTR_EX_DEF1(img) \
  MAGICSTR_EX_DEF1(initial_position) \
  MAGICSTR_EX_DEF1(interval) \
  MAGICSTR_EX_DEF1(j) \
  MAGICSTR_EX_DEF1(json_str) \
  MAGICSTR_EX_DEF1(k) \
  MAGICSTR_EX_DEF1(Label) \
  MAGICSTR_EX_DEF1(lbl) \
  MAGICSTR_EX_DEF1(lbl_hr) \
  MAGICSTR_EX_DEF1(lbl_sc) \
  MAGICSTR_EX_DEF1(MOVE_CUSTOM) \
  MAGICSTR_EX_DEF1(MOVE_E) \
  MAGICSTR_EX_DEF1(MOVE_N) \
  MAGICSTR_EX_DEF1(move_prop) \
  MAGICSTR_EX_DEF1(MoveProperty) \
  MAGICSTR_EX_DEF1(MOVE_S) \
  MAGICSTR_EX_DEF1(moveView) \
  MAGICSTR_EX_DEF1(MOVE_W) \
  MAGICSTR_EX_DEF1(_nativeAddWidget) \
  MAGICSTR_EX_DEF1(obj) \
  MAGICSTR_EX_DEF1(on) \
  MAGICSTR_EX_DEF1(onEvent) \
  MAGICSTR_EX_DEF1(POSITION_E) \
  MAGICSTR_EX_DEF1(POSITION_N) \
  MAGICSTR_EX_DEF1(POSITION_NONE) \
  MAGICSTR_EX_DEF1(POSITION_S) \
  MAGICSTR_EX_DEF1(POSITION_UNKNOWN) \
  MAGICSTR_EX_DEF1(POSITION_W) \
  MAGICSTR_EX_DEF1(PropertyMoveType) \
  MAGICSTR_EX_DEF1(property_type) \
  MAGICSTR_EX_DEF1(sc_callback) \
  MAGICSTR_EX_DEF1(SCREEN_HEIGHT) \
  MAGICSTR_EX_DEF1(SCREEN_WIDTH) \
  MAGICSTR_EX_DEF1(setAbsoluteX) \
  MAGICSTR_EX_DEF1(setAbsoluteY) \
  MAGICSTR_EX_DEF1(setActiveView) \
  MAGICSTR_EX_DEF1(setFont) \
  MAGICSTR_EX_DEF1(setText) \
  MAGICSTR_EX_DEF1(src) \
  MAGICSTR_EX_DEF1(SS) \
  MAGICSTR_EX_DEF1(start) \
  MAGICSTR_EX_DEF1(startHRCounting) \
  MAGICSTR_EX_DEF1(startStepCounting) \
  MAGICSTR_EX_DEF1(step) \
  MAGICSTR_EX_DEF1(switchView) \
  MAGICSTR_EX_DEF1(text) \
  MAGICSTR_EX_DEF1(TouchGesture) \
  MAGICSTR_EX_DEF1(TOUCH_GESTURE_SWIPE_E) \
  MAGICSTR_EX_DEF1(TOUCH_GESTURE_SWIPE_N) \
  MAGICSTR_EX_DEF1(TOUCH_GESTURE_SWIPE_S) \
  MAGICSTR_EX_DEF1(TOUCH_GESTURE_SWIPE_W) \
  MAGICSTR_EX_DEF1(TOUCH_MOVE_E) \
  MAGICSTR_EX_DEF1(TOUCH_MOVE_N) \
  MAGICSTR_EX_DEF1(TOUCH_MOVE_S) \
  MAGICSTR_EX_DEF1(TOUCH_MOVE_W) \
  MAGICSTR_EX_DEF1(type) \
  MAGICSTR_EX_DEF1(ui_json) \
  MAGICSTR_EX_DEF1(v) \
  MAGICSTR_EX_DEF1(View) \
  MAGICSTR_EX_DEF1(view1) \
  MAGICSTR_EX_DEF1(view1_on_event) \
  MAGICSTR_EX_DEF1(view2) \
  MAGICSTR_EX_DEF1(view2_on_event) \
  MAGICSTR_EX_DEF1(ViewGroup) \
  MAGICSTR_EX_DEF1(ViewManager) \
  MAGICSTR_EX_DEF1(view_obj) \
  MAGICSTR_EX_DEF1(ViewPositionType) \
  MAGICSTR_EX_DEF1(views) \
  MAGICSTR_EX_DEF1(widget) \
  MAGICSTR_EX_DEF1(WidgetEvent) \
  MAGICSTR_EX_DEF1(width) \
  MAGICSTR_EX_DEF1(x) \
  MAGICSTR_EX_DEF1(y) \
  MAGICSTR_EX_DEF2(fs, "Finding...") \
  MAGICSTR_EX_DEF2(click_btn1, "click button1, show heart rate") \
  MAGICSTR_EX_DEF2(click_btn2, "click button2, show step count") \
  MAGICSTR_EX_DEF2(no_gesture, "other gesture not handled in this view") \
  MAGICSTR_EX_DEF2(uijson, "{ \"type\": \"ViewGroup\", \"id\": \"viewGroup\", \"views\": [ { \"type\": \"View\", \"id\": \"view1\", \"children\": [ { \"image\": { \"src\": \"steps_big_icon\", \"height\": 16, \"width\": 16 }, \"height\": 30, \"width\": 20, \"y\": 50, \"x\": 140, \"type\": \"Icon\", \"id\": \"icon_sc\" }, { \"height\": 60, \"width\": 120, \"y\": 44, \"x\": 20, \"font\": \"FontCond67\", \"type\": \"Label\", \"id\": \"lbl_sc\" }, { \"text\": \"NDG DSW SH &\", \"height\": 20, \"width\": 120, \"y\": 103, \"x\": 40, \"font\": \"FontCond20\", \"type\": \"Button\", \"id\": \"btn_author1_1\" }, { \"text\": \"SSG WTO PRC\", \"height\": 20, \"width\": 120, \"y\": 123, \"x\": 40, \"font\": \"FontCond20\", \"type\": \"Button\", \"id\": \"btn_author1_2\" }, { \"image\": { \"src\": \"bt_on_icon\", \"height\": 13, \"width\": 8 }, \"height\": 30, \"width\": 120, \"y\": 10, \"x\": 90, \"type\": \"Icon\", \"id\": \"icon_bt_status1\" }, { \"image\": { \"src\": \"battery_60_icon\", \"height\": 10, \"width\": 21 }, \"height\": 30, \"width\": 120, \"y\": 10, \"x\": 135, \"type\": \"Icon\", \"id\": \"icon_battery1\" } ] }, { \"type\": \"View\", \"id\": \"view2\", \"children\": [ { \"height\": 65, \"width\": 80, \"y\": 39, \"x\": 20, \"font\": \"FontCond67\", \"type\": \"Label\", \"id\": \"lbl_hr\" }, { \"text\": \"NDG DSW SH &\", \"height\": 20, \"width\": 120, \"y\": 103, \"x\": 40, \"font\": \"FontCond20\", \"type\": \"Button\", \"id\": \"btn_author2_1\" }, { \"text\": \"SSG WTO PRC\", \"height\": 20, \"width\": 120, \"y\": 123, \"x\": 40, \"font\": \"FontCond20\", \"type\": \"Button\", \"id\": \"btn_author2_2\" }, { \"images\": [ { \"src\": \"hr_big_big_icon\", \"height\": 26, \"width\": 30 }, { \"src\": \"hr_big_small_icon\", \"height\": 26, \"width\": 30 } ], \"interval\": 500, \"height\": 30, \"width\": 30, \"y\": 50, \"x\": 110, \"type\": \"Animation\", \"id\": \"ani_hr\" }, { \"image\": { \"src\": \"bt_on_icon\", \"height\": 13, \"width\": 8 }, \"height\": 30, \"width\": 120, \"y\": 10, \"x\": 90, \"type\": \"Icon\", \"id\": \"icon_bt_status2\" }, { \"image\": { \"src\": \"battery_60_icon\", \"height\": 10, \"width\": 21 }, \"height\": 30, \"width\": 120, \"y\": 10, \"x\": 135, \"type\": \"Icon\", \"id\": \"icon_battery2\" } ] } ] }")


//
// declare string items
//
#define MAGICSTR_EX_DEF1(NAME) \
  static const char jerry_magic_string_ex_ ## NAME[] = # NAME;
#define MAGICSTR_EX_DEF2(NAME, STRING) \
  static const char jerry_magic_string_ex_ ## NAME[] = STRING;

JERRY_MAGIC_STRING_ITEMS

#undef MAGICSTR_EX_DEF1
#undef MAGICSTR_EX_DEF2


//
// declare strings length array
//
static const jerry_api_length_t magic_string_lengths[] =
{
#define MAGICSTR_EX_DEF1(NAME) \
  (jerry_api_length_t)(sizeof(jerry_magic_string_ex_ ## NAME) - 1u),
#define MAGICSTR_EX_DEF2(NAME, STRING) \
  (jerry_api_length_t)(sizeof(jerry_magic_string_ex_ ## NAME) - 1u),

  JERRY_MAGIC_STRING_ITEMS

#undef MAGICSTR_EX_DEF1
#undef MAGICSTR_EX_DEF2
};


//
// declare strings table
//
static const jerry_api_char_ptr_t magic_string_items[] =
{
#define MAGICSTR_EX_DEF1(NAME) \
  (const jerry_api_char_ptr_t)jerry_magic_string_ex_ ## NAME,
#define MAGICSTR_EX_DEF2(NAME, STRING) \
  (const jerry_api_char_ptr_t)jerry_magic_string_ex_ ## NAME,

  JERRY_MAGIC_STRING_ITEMS

#undef MAGICSTR_EX_DEF1
#undef MAGICSTR_EX_DEF2
};


void InitJerryMagicStringEx(void) {
  uint32_t num_magic_string_items = (uint32_t)(sizeof(magic_string_items)
                                    / sizeof(jerry_api_char_ptr_t));
  jerry_register_external_magic_strings(magic_string_items,
                                        num_magic_string_items,
                                        magic_string_lengths);
}
