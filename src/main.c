#include <pebble.h>

#define LOGGING 0
	
static void handle_tick(struct tm *tick_time, TimeUnits units_changed);	
void out_sent_handler(DictionaryIterator *sent, void *context);
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
void in_received_handler(DictionaryIterator *received, void *context);
void in_dropped_handler(AppMessageResult reason, void *context);

Window *window;
TextLayer *battery_layer;
TextLayer *clock_layer;
TextLayer *temp_layer;

enum {
    KEY_TEMPERATURE = 0x00,
	KEY_CITY = 0x01,
};

void handle_init(void) {
	//call handle_tick every minute
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	//create a window
	window = window_create();
	//create a battery layer
	GRect battery_rect;
	battery_rect.origin.x = 0;
	battery_rect.origin.y = 0;
	battery_rect.size.h = 20;
	battery_rect.size.w = 144;
	battery_layer = text_layer_create(battery_rect);
	
	//create a clock layer
	GRect clock_rect;
	clock_rect.origin.x = 0;
	clock_rect.origin.y = 20;
	clock_rect.size.h = 80;
	clock_rect.size.w = 144;
	clock_layer = text_layer_create(clock_rect);
	
	//create a temp layer
	GRect temp_rect;
	temp_rect.origin.x = 0;
	temp_rect.origin.y = 100;
	temp_rect.size.h = 20;
	temp_rect.size.w = 144;
	temp_layer = text_layer_create(temp_rect);

	//Set up the window, layer and text
	window_stack_push(window, true /* Animated */);
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	//Set up the battery
	text_layer_set_font(battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(battery_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(battery_layer));
	
	//Set up the clock
	text_layer_set_font(clock_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(clock_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(clock_layer));
	
	//Set up the weather
	text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(temp_layer));
	
	//Set up receiving messages from phone
	app_message_register_inbox_received(in_received_handler);
   	app_message_register_inbox_dropped(in_dropped_handler);
   	app_message_register_outbox_sent(out_sent_handler);
   	app_message_register_outbox_failed(out_failed_handler);
	const uint32_t inbound_size = 64;
   	const uint32_t outbound_size = 64;
   	app_message_open(inbound_size, outbound_size);
}

void handle_deinit(void) {
	//desctroy the batter layer
	text_layer_destroy(battery_layer);
	//destroy the clock layer
	text_layer_destroy(clock_layer);
	//destroy the temp layer
	text_layer_destroy(temp_layer);
	//destroy the window
	window_destroy(window);
	//stop ticker
	tick_timer_service_unsubscribe();
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG, "Time flies!");

	//Get the battery
	static char battery_text[] = "100% charged";
    BatteryChargeState charge_state = battery_state_service_peek();
    if (charge_state.is_charging) {
      snprintf(battery_text, sizeof(battery_text), "charging");
    } else {
      snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
    }
	//Display the battery
	text_layer_set_text(battery_layer,battery_text);
	
	//Get the time
	static char time_text[] = "12:00";
	clock_copy_time_string(time_text, sizeof(time_text));
	if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG, time_text);
	//Display the time
	text_layer_set_text(clock_layer, time_text);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
	if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG, "out_sent_handler");
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG, "out_failed_handler");
}

void in_received_handler(DictionaryIterator *received, void *context) {
	if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler");

	//Read the termperature from the phone
	static char temp_text[] = "Checking";
	Tuple *temp_tuple = dict_find(received, KEY_TEMPERATURE);
	if (temp_tuple) {
    	strncpy(temp_text, temp_tuple->value->cstring, sizeof(temp_text));
        text_layer_set_text(temp_layer, temp_text);
		if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG,temp_text);
    }
	else {
		if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG,"No temperature found");
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) {
	if(LOGGING) APP_LOG(APP_LOG_LEVEL_DEBUG, "in_dropped_handler");
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
