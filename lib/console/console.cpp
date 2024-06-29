#include "console.h"

Console::Console(void) {
	Serial.begin(115200);
	Serial.println();
	this->max_tag_length = TAG_MAX_LENGTH;
	this->tag_written = false;
}

void Console::setTagLength(uint8_t length) {
	this->max_tag_length = length;
}

void Console::line(style_t style, uint8_t length, bool new_line) {
	String line_str;
	String chr;

	if(this->tag_written == true)
		newLine();

	switch(style)
	{
		case DOTTED:		chr = "."; break;
		case DASHED:		chr = "-"; break;
		case DOUBLE_DASHED:	chr = "="; break;
		case LINE:			chr = "_"; break;
		case WAVE:			chr = "~"; break;
		case HASH:			chr = "#"; break;
		case ASTERISK:		chr = "*"; break;
		default:			chr = "."; break;
	}

	for(uint8_t i=0 ; i<length ; i++)
		line_str += chr;

	if(new_line)
		Serial.println(line_str);
	else
		Serial.print(line_str);
}

void Console::header(String title, style_t style, uint8_t length) {
	uint8_t fill = (length - title.length() - 6);
	uint8_t side = fill / 2;

	if(this->tag_written == true)
		newLine();	

	line(style, length, true);
	line(style, side, false);
	Serial.print("   " + title + "   ");
	if(length%2)	// Odd length
		line(style, (title.length()%2 ? side : side+1), true);
	else			// Even length
		line(style, (title.length()%2 ? side+1 : side), true);
	line(style, length, true);
}

void Console::writeTag(String tag) {
	uint8_t num_spaces = this->max_tag_length - tag.length();
	String pad_str;

	if(this->tag_written == true)
		newLine();

	for(uint8_t i=0 ; i<num_spaces ; i++)
		pad_str += " ";
	Serial.print("[ " + tag + pad_str + " ] ");

	#ifdef HEAP_LOG_ENABLED
	uint32_t curr_free_heap = ESP.getFreeHeap();
	int32_t delta = curr_free_heap - last_free_heap;
	String heap = "[" + String(curr_free_heap) + " (" + String(delta) + ")] ";
	last_free_heap = curr_free_heap;
	Serial.print(heap);
	#endif
}

void Console::write(String tag, String message) {
	if(this->tag_written == false) {
		writeTag(tag);
		this->tag_written = true;
	}
	Serial.print(message);
}

void Console::log(String tag, String message) {
	write(tag, message);
	newLine();
}

// Terminal text in green -------------------------------------------
void Console::success(String tag, String message) {
	Serial.print(F(LOG_GREN));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in blue --------------------------------------------
void Console::info(String tag, String message) {
	Serial.print(F(LOG_BLUE));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in yellow ------------------------------------------
void Console::warning(String tag, String message) {
	Serial.print(F(LOG_YELW));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in red ---------------------------------------------
void Console::error(String tag, String message) {
	Serial.print(F(LOG_RED));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in magenta -----------------------------------------
void Console::command(String tag, String message) {
	Serial.print(F(LOG_MAGN));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in cyan --------------------------------------------
void Console::data(String tag, String message) {
	Serial.print(F(LOG_CYAN));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in orange -------------------------------------
void Console::datain(String tag, String message) {
	Serial.print(F(LOG_ORNG));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

// Terminal text in purple -----------------------------------
void Console::dataout(String tag, String message) {
	Serial.print(F(LOG_PURP));
	write(tag, message);
	Serial.print(F(LOG_NORM));
	newLine();
}

void Console::newLine(void) {
	this->tag_written = false;
	Serial.println();
}

// Terminal text in red ---------------------------------------------
void Console::heap(String message) {
	/*
	uint32_t curr_free_heap = ESP.getFreeHeap();
	int32_t delta = curr_free_heap - last_free_heap;
	String heap = "[" + String(curr_free_heap) + " (" + String(delta) + ")] ";
	last_free_heap = curr_free_heap;
	write(HEAP_T, heap + message);
	*/
	write(HEAP_T, message);
	newLine();
}

Console console;