#include <pebble.h>
#include <time.h>
#include <stdio.h>

Window *window;

TextLayer *text_day_layer;
TextLayer *text_week_layer;
TextLayer *text_period_layer;
TextLayer *text_date_layer;
TextLayer *text_time_layer;

Layer *line_layer;

static const char *day_names[] = {
  "Zondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrijdag", "Zaterdag"
};

static const char *month_names[] = {
  "Januari", "Februari", "Maart", "April", "Mei", "Juni", 
  "Juli", "Augustus", "September", "Oktober", "November", "December"
};

void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[]     = "00:00";
  static char day_text[]      = "          ";
  static char week_text[]     = "W00";
  static char period_text[]   = "P00.1";
  static char date_text[]     = "               ";
  static char new_date_text[] = "               ";

  int month = tick_time->tm_mon;
  strftime(new_date_text, sizeof(date_text), "%e ", tick_time);
  strcat(new_date_text, month_names[month]);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "new_date_text: |%s|", new_date_text);

  // Only update the date/day/week strings when they're changed.
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
	  strftime(week_text, sizeof(week_text), "W%V", tick_time);
	  text_layer_set_text(text_week_layer, week_text);

          // Parse week number (start at char after "W", hence the + 1)
          int week_nr = atoi(week_text + 1);
          int period_nr = ((week_nr - 1) / 4) + 1;
          int period_sub_nr = ((week_nr - 1) % 4) + 1;
          snprintf(period_text, 6, "P%02d.%1d", period_nr, period_sub_nr);
	  text_layer_set_text(text_period_layer, period_text);

	  int dayofweek = tick_time->tm_wday;
	  strcpy(day_text, day_names[dayofweek]);
	  text_layer_set_text(text_day_layer, day_text);

	  strncpy(date_text, new_date_text, sizeof(date_text));
	  text_layer_set_text(text_date_layer, date_text);
  }

  char *time_format;
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}

void handle_init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  // week
  text_week_layer = text_layer_create(GRect(90, 8, 144-90, 168-8));
  text_layer_set_text_color(text_week_layer, GColorWhite);
  text_layer_set_background_color(text_week_layer, GColorClear);
  text_layer_set_font(text_week_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_week_layer));

  // period
  text_period_layer = text_layer_create(GRect(8, 8, 144-8, 168-8));
  text_layer_set_text_color(text_period_layer, GColorWhite);
  text_layer_set_background_color(text_period_layer, GColorClear);
  text_layer_set_font(text_period_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_period_layer));

  // weekday
  text_day_layer = text_layer_create(GRect(8, 44, 144-8, 168-44));
  text_layer_set_text_color(text_day_layer, GColorWhite);
  text_layer_set_background_color(text_day_layer, GColorClear);
  text_layer_set_font(text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_day_layer));

  // day/month
  text_date_layer = text_layer_create(GRect(8, 68, 144-8, 168-68));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  // time
  text_time_layer = text_layer_create(GRect(7, 92, 144-7, 168-92));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  // line
  GRect line_frame = GRect(8, 97, 131, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);

  // Avoid blank display on launch
  time_t now = time(NULL);
  struct tm *tm_struct = localtime(&now);
  handle_minute_tick(tm_struct, (TimeUnits) NULL);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();

  layer_destroy(line_layer);
  text_layer_destroy(text_day_layer);
  text_layer_destroy(text_week_layer);
  text_layer_destroy(text_period_layer);
  text_layer_destroy(text_date_layer);
  text_layer_destroy(text_time_layer);

  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}


