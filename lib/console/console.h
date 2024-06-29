#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <Arduino.h>

#define TAG_MAX_LENGTH		7
#define HEAP_LOG_ENABLED
#define HEAP_T          "HEAP"

#define LOG_NORM        "\033[0m"
#define LOG_RED         "\033[1;31m"
#define LOG_GREN        "\033[1;32m"
#define LOG_BLUE        "\033[1;34m"
#define LOG_YELW        "\033[1;33m"
#define LOG_MAGN        "\033[1;35m"
#define LOG_CYAN        "\033[1;36m"
#define LOG_ORNG        "\033[38;5;214m"
#define LOG_PURP        "\033[38;5;91m"

typedef enum {
    DOTTED,             // ...
    DASHED,             // ---
    DOUBLE_DASHED,      // ===
    LINE,               // ___
    WAVE,               // ~~~
    HASH,               // ###
    ASTERISK,           // ***
} style_t;

class Console {
    private:
        uint8_t max_tag_length;
        bool tag_written;
        uint32_t last_free_heap;
	public:
		Console(void);
        void setTagLength(uint8_t length);
        void line(style_t style, uint8_t length, bool new_line);
        void header(String title, style_t style, uint8_t length);
        void writeTag(String tag);
		void write(String tag, String message);
        void log(String tag, String str);
        void success(String tag, String str);
        void info(String tag, String str);
        void warning(String tag, String str);
        void error(String tag, String message);
        void command(String tag, String message);
        void data(String tag, String message);
        void datain(String tag, String message);
        void dataout(String tag, String message);
        void newLine(void);
        void heap(String message);
};

extern Console console;

#endif  /* CONSOLE_H_ */