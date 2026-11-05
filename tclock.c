#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

static struct termios saved;
static volatile sig_atomic_t running = 1;
static int terminal_saved;

static void stop(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static void cleanup(void)
{
    if (terminal_saved)
        tcsetattr(STDIN_FILENO, TCSANOW, &saved);
    /* Restore colors, cursor, and the original screen. */
    fputs("\033[0m\033[?25h\033[?1049l", stdout);
    fflush(stdout);
}

static void at(int row, int column)
{
    printf("\033[%d;%dH", row, column);
}

static void centered(int row, int columns, const char *text)
{
    int length = (int)strlen(text);
    if (length <= columns) {
        at(row, (columns - length) / 2 + 1);
        fputs("\033[2K", stdout);
        fputs(text, stdout);
    }
}

static void help(const char *program)
{
    printf("Usage: %s [OPTIONS]\n\n"
           "A centered, full-terminal digital clock.\n\n"
           "  -h, --help           Show this help and exit\n"
           "  --size SIZE          small, medium, large (default: large)\n"
           "  --scale N            Rows per font pixel, 1-20 (default: 3)\n"
           "  --width N            Columns per font pixel, 1-40 (default: 6)\n"
           "  --color COLOR        cyan, blue, green, yellow, red, magenta, white\n"
           "                       orange, grey (gray), or '#RRGGBB' (default: cyan)\n"
           "                       Quote hex colors; requires a true-color terminal.\n"
           "  --12-hour            Use 12-hour time with an AM/PM label\n"
           "  --no-seconds         Show only hours and minutes\n"
           "  --no-date            Hide the date\n"
           "  --no-hint            Hide the quit hint\n"
           "  --utc                Show UTC instead of local time\n\n"
           "Sizes shrink automatically to fit. Options apply left to right.\n"
           "Press q, Esc, or Ctrl+C to quit.\n\n"
           "Example: %s --width 6 --scale 3 --color green --no-seconds\n",
           program, program);
}

static int positive_number(const char *text, int maximum)
{
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno || end == text || *end || value < 1 || value > maximum)
        return 0;
    return (int)value;
}

int main(int argc, char **argv)

{
    static const char *digits[] = {
        "111101101101111", "010110010010111", "111001111100111",
        "111001111001111", "101101111001001", "111100111001111",
        "111100111101111", "111001001001001", "111101111101111",
        "111101111001111"
    };
    struct termios raw;
    struct sigaction action = {0};

    int requested_width = 6, requested_scale = 3, color = 6;
    long rgb = -1;
    int seconds = 1, show_date = 1, hint = 1, twelve = 0, utc = 0;
    enum { SIZE = 256, SCALE, WIDTH, COLOR, TWELVE, NO_SECONDS,
           NO_DATE, NO_HINT, UTC };
    static const struct option options[] = {
        {"help", no_argument, NULL, 'h'},
        {"size", required_argument, NULL, SIZE},
        {"scale", required_argument, NULL, SCALE},
        {"width", required_argument, NULL, WIDTH},
        {"color", required_argument, NULL, COLOR},
        {"12-hour", no_argument, NULL, TWELVE},
        {"no-seconds", no_argument, NULL, NO_SECONDS},
        {"no-date", no_argument, NULL, NO_DATE},
        {"no-hint", no_argument, NULL, NO_HINT},
        {"utc", no_argument, NULL, UTC},
        {NULL, 0, NULL, 0}
    };
    int option;
    while ((option = getopt_long(argc, argv, "h", options, NULL)) != -1) {
        switch (option) {
        case 'h': help(argv[0]); return EXIT_SUCCESS;
        case SIZE:
            if (!strcmp(optarg, "small")) {
                requested_width = 2; requested_scale = 1;
            } else if (!strcmp(optarg, "medium")) {
                requested_width = 4; requested_scale = 2;
            } else if (!strcmp(optarg, "large")) {
                requested_width = 6; requested_scale = 3;
            } else goto invalid_value;
            break;
        case SCALE:
            requested_scale = positive_number(optarg, 20);
            if (!requested_scale) goto invalid_value;
            break;
        case WIDTH:
            requested_width = positive_number(optarg, 40);
            if (!requested_width) goto invalid_value;
            break;
        case COLOR: {
            static const char *names[] = {
                "black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"
            };
            rgb = -1;
            if (!strcmp(optarg, "orange")) {
                rgb = 0xFF8800;
            } else if (!strcmp(optarg, "grey") || !strcmp(optarg, "gray")) {
                rgb = 0x808080;
            } else if (optarg[0] == '#' ||
                       (strlen(optarg) == 6 && strspn(optarg, "0123456789abcdefABCDEF") == 6)) {
                const char *hex = optarg + (optarg[0] == '#');
                if (strlen(hex) != 6) goto invalid_value;
                for (int i = 0; i < 6; ++i)
                    if (!isxdigit((unsigned char)hex[i])) goto invalid_value;
                rgb = strtol(hex, NULL, 16);
            } else {
                for (color = 1; color < 8; ++color)
                    if (!strcmp(optarg, names[color])) break;
                if (color == 8) goto invalid_value;
            }
            break;
        }
        case TWELVE: twelve = 1; break;
        case NO_SECONDS: seconds = 0; break;
        case NO_DATE: show_date = 0; break;
        case NO_HINT: hint = 0; break;
        case UTC: utc = 1; break;
        default: return EXIT_FAILURE;
        }
        continue;
invalid_value:
        fprintf(stderr, "Invalid value: %s. Run %s --help.\n", optarg, argv[0]);
        return EXIT_FAILURE;
    }
    if (optind < argc) {
        fprintf(stderr, "Unexpected argument: %s. Run %s --help.\n", argv[optind], argv[0]);
        return EXIT_FAILURE;
    }

    char foreground[48], background[48];
    if (rgb >= 0) {
        unsigned red = ((unsigned long)rgb >> 16) & 255;
        unsigned green = ((unsigned long)rgb >> 8) & 255;
        unsigned blue = (unsigned long)rgb & 255;
        snprintf(foreground, sizeof foreground, "\033[0;38;2;%u;%u;%um", red, green, blue);
        snprintf(background, sizeof background, "\033[48;2;%u;%u;%um", red, green, blue);
    } else {
        snprintf(foreground, sizeof foreground, "\033[0;%dm", 30 + color);
        snprintf(background, sizeof background, "\033[%dm", 40 + color);
    }

    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
        fputs("Please run this clock in an interactive terminal.\n", stderr);
        return EXIT_FAILURE;
    }
    if (tcgetattr(STDIN_FILENO, &saved) == -1) {
        perror("tcgetattr");
        return EXIT_FAILURE;
    }
    raw = saved;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }
    terminal_saved = 1;
    atexit(cleanup);
    action.sa_handler = stop;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
    /* Buffer each normal-sized frame so updates reach the terminal together. */
    static char output_buffer[65536];
    setvbuf(stdout, output_buffer, _IOFBF, sizeof output_buffer);
    fputs("\033[?1049h\033[?25l", stdout);
    int previous_columns = 0, previous_rows = 0;

    while (running) {
        struct winsize size = {0};
        struct tm local;
        time_t now = time(NULL);
        char clock_text[16], date[64];
        int columns = 80, rows = 24;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) {
            if (size.ws_col) columns = size.ws_col;
            if (size.ws_row) rows = size.ws_row;
        }
        if (!(utc ? gmtime_r(&now, &local) : localtime_r(&now, &local))) break;
        strftime(clock_text, sizeof clock_text,
                 twelve ? (seconds ? "%I:%M:%S" : "%I:%M")
                        : (seconds ? "%H:%M:%S" : "%H:%M"), &local);
        strftime(date, sizeof date, "%A, %d %B %Y", &local);
        char label[24] = "";
        if (twelve) strftime(label, sizeof label, "%p", &local);
        if (utc) strcat(label, twelve ? " UTC" : "UTC");
        int count = (int)strlen(clock_text);
        int pixels = seconds ? 20 : 13;
        int gaps = 2 * (count - 1);
        int extras = (show_date ? 3 : 0) + (label[0] ? 2 : 0);
        int available_rows = rows - (hint ? 2 : 0);
        int pixel_width = requested_width;
        int scale = requested_scale;
        while (pixel_width > 1 && pixels * pixel_width + gaps > columns - 2)
            --pixel_width;
        while (scale > 1 && 5 * scale + extras > available_rows)
            --scale;
        fputs("\033[0m", stdout);
        if (columns != previous_columns || rows != previous_rows) {
            fputs("\033[2J", stdout);
            previous_columns = columns;
            previous_rows = rows;
        }

        if (pixels * pixel_width + gaps <= columns - 2 &&
            5 * scale + extras <= available_rows) {
            int width = pixels * pixel_width + gaps;
            int top = (available_rows - (5 * scale + extras)) / 2 + 1;
            for (int y = 0; y < 5 * scale; ++y) {
                at(top + y, (columns - width) / 2 + 1);
                for (int i = 0; i < count; ++i) {
                    int colon = clock_text[i] == ':';
                    for (int x = 0; x < (colon ? 1 : 3); ++x) {
                        int lit = colon ? (y / scale == 1 || y / scale == 3)
                            : digits[clock_text[i] - '0'][(y / scale) * 3 + x] == '1';
                        fputs(lit ? background : "\033[0m", stdout);
                        for (int p = 0; p < pixel_width; ++p) putchar(' ');
                    }
                    if (i < count - 1) fputs("\033[0m  ", stdout);
                }
            }
            fputs(foreground, stdout);
            int next_row = top + 5 * scale + 1;
            if (label[0]) {
                centered(next_row, columns, label);
                next_row += 2;
            }
            if (show_date) centered(next_row + 1, columns, date);
        } else {
            char compact[48];
            snprintf(compact, sizeof compact, "%s%s%s", clock_text,
                     label[0] ? " " : "", label);
            fputs(foreground, stdout);
            fputs("\033[1m", stdout);
            int row = rows > 3 ? rows / 2 : 1;
            centered(row, columns, compact);
            if (show_date && row + 2 < rows - (hint ? 1 : 0))
                centered(row + 2, columns, date);
        }
        fputs("\033[0;2m", stdout);
        if (hint && rows >= 5) centered(rows - 1, columns, "q / Esc to quit");
        fputs("\033[0m", stdout);
        fflush(stdout);

        fd_set input;
        FD_ZERO(&input);
        FD_SET(STDIN_FILENO, &input);
        struct timeval wait = {.tv_sec = 0, .tv_usec = 100000};
        int ready = select(STDIN_FILENO + 1, &input, NULL, NULL, &wait);
        if (ready > 0) {
            char key;
            if (read(STDIN_FILENO, &key, 1) <= 0 || key == 'q' || key == 'Q' || key == 27)
                running = 0;
        } else if (ready < 0 && errno != EINTR) {
            running = 0;
        }
    }
    return EXIT_SUCCESS;
}
