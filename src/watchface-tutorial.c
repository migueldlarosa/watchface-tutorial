#include <pebble.h>
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_MSG1 2
#define KEY_MSG2 3
#define KEY_MSG3 4
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_date_layer;
static TextLayer *s_date2_layer;
static TextLayer *s_bat_layer;
static TextLayer *s_bt_layer;
static TextLayer *s_msg1_layer;
static TextLayer *s_msg2_layer;
static TextLayer *s_msg3_layer;
static TextLayer *s_cus_layer;

static char s_num_buffer[6], s_day_buffer[6];

static GFont s_time_font;
static GFont s_weather_font;
 
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
 
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
 
  // Create a long-lived buffer
  static char buffer[] = "00:00";
 
  // Write the current hours and minutes into the buffer
  if(false) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
 
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", tick_time);
  text_layer_set_text(s_date_layer, s_num_buffer);
  
  strftime(s_day_buffer, sizeof(s_day_buffer), "%m", tick_time);
  text_layer_set_text(s_date2_layer, s_day_buffer);
}

static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d", new_state.charge_percent);
  text_layer_set_text(s_bat_layer, s_battery_buffer);
}

static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    text_layer_set_text(s_bt_layer, "B");
  } else {
    text_layer_set_text(s_bt_layer, "X");
  }
}
 
static void main_window_load(Window *window) {
  //Create GBitmap, then set to created BitmapLayer
  //s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  //s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  //bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  //layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  //Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
 
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
 
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Create date Layer
  s_date_layer = text_layer_create(GRect(0, 105, 65, 25));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_text(s_date_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  // Create date2 Layer
  s_date2_layer = text_layer_create(GRect(80, 105, 60, 25));
  text_layer_set_background_color(s_date2_layer, GColorClear);
  text_layer_set_text_color(s_date2_layer, GColorBlack);
  text_layer_set_text_alignment(s_date2_layer, GTextAlignmentLeft);
  text_layer_set_text(s_date2_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date2_layer));
  
  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(0, 130, 144, 25));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Loading...");
  
  // Create battery Layer
  s_bat_layer = text_layer_create(GRect(5, 0, 30, 10));
  text_layer_set_background_color(s_bat_layer, GColorClear);
  text_layer_set_text_color(s_bat_layer, GColorBlack);
  text_layer_set_text_alignment(s_bat_layer, GTextAlignmentLeft);
  text_layer_set_text(s_bat_layer, "...");
  text_layer_set_size(s_bat_layer, (GSize){ 50, 50 });
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bat_layer));
  
  // Create bluetooth Layer
  s_bt_layer = text_layer_create(GRect(window_bounds.size.w-60, 0, 30, 10));
  text_layer_set_background_color(s_bt_layer, GColorClear);
  text_layer_set_text_color(s_bt_layer, GColorBlack);
  text_layer_set_text_alignment(s_bt_layer, GTextAlignmentRight);
  text_layer_set_text(s_bt_layer, "...");
  text_layer_set_size(s_bt_layer, (GSize){ 50, 50 });
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bt_layer));
  
  //Create msg1 layer
  s_msg1_layer = text_layer_create(GRect(0, 10, 144, 25));
  text_layer_set_background_color(s_msg1_layer, GColorClear);
  text_layer_set_text_color(s_msg1_layer, GColorBlack);
  text_layer_set_text_alignment(s_msg1_layer, GTextAlignmentCenter);
  text_layer_set_text(s_msg1_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg1_layer));
  
  //Create msg2 layer
  s_msg2_layer = text_layer_create(GRect(0, 30, 144, 25));
  text_layer_set_background_color(s_msg2_layer, GColorClear);
  text_layer_set_text_color(s_msg2_layer, GColorBlack);
  text_layer_set_text_alignment(s_msg2_layer, GTextAlignmentCenter);
  text_layer_set_text(s_msg2_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg2_layer));
  
  //Create msg3 layer
  s_msg3_layer = text_layer_create(GRect(0, 50, 144, 25));
  text_layer_set_background_color(s_msg3_layer, GColorClear);
  text_layer_set_text_color(s_msg3_layer, GColorBlack);
  text_layer_set_text_alignment(s_msg3_layer, GTextAlignmentCenter);
  text_layer_set_text(s_msg3_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg3_layer));
  
  // Create second custom font, apply it and add to Window
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  text_layer_set_font(s_weather_layer, s_weather_font);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  
  // Make sure the time is displayed from the start
  bt_handler(bluetooth_connection_service_peek());
  battery_handler(battery_state_service_peek());
  update_time();
}
 
static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
 
  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  text_layer_destroy(s_cus_layer);
  
  // Destroy weather elements
  text_layer_destroy(s_weather_layer);
  fonts_unload_custom_font(s_weather_font);
}
 
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  if(tick_time->tm_min % 1 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
 
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
 
    // Send the message!
    app_message_outbox_send();
  }
}
 
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char msg1_buffer[32];
  static char msg2_buffer[32];
  static char msg3_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
 
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    case KEY_MSG1:
      snprintf(msg1_buffer, sizeof(msg1_buffer), "%s", t->value->cstring);
      break;
    case KEY_MSG2:
      snprintf(msg2_buffer, sizeof(msg2_buffer), "%s", t->value->cstring);
      break;
    case KEY_MSG3:
      snprintf(msg3_buffer, sizeof(msg3_buffer), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
 
    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
  text_layer_set_text(s_msg1_layer, msg1_buffer);
  text_layer_set_text(s_msg2_layer, msg2_buffer);
  text_layer_set_text(s_msg3_layer, msg3_buffer);
}
 
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
 
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
 
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
 
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
 
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  battery_state_service_subscribe(battery_handler);
  bluetooth_connection_service_subscribe(bt_handler);
}
 
static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}
 
int main(void) {
  init();
  app_event_loop();
  deinit();
}