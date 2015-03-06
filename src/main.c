#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;

static BitmapLayer *s_car_bitmaplayer;
static GBitmap *s_car_gbitmap;


static void update_time() {

  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";

  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  text_layer_set_text(s_time_layer, buffer);
}
  
static void main_window_load(Window *window) {
  
  window_set_background_color(window, GColorBlack);
  
  // Create GBitmap, then set to created BitmapLayer
  s_car_gbitmap = gbitmap_create_with_resource(RESOURCE_ID_MK6);
  s_car_bitmaplayer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_car_bitmaplayer, s_car_gbitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_car_bitmaplayer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 10, 144, 60));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorClear);

  // Improve the layout to be more like a watchface
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_VW_FONT_40));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  
  bitmap_layer_destroy(s_car_bitmaplayer);
  gbitmap_destroy(s_car_gbitmap);

  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  animation_unschedule_all();
  
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}