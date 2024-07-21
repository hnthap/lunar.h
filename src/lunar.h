enum Task
{
    LUNARH_GREGORIAN_TO_LUNAR_IN_CHINESE = 1,
    LUNARH_GREGORIAN_TO_LUNAR_IN_MODERN_NOTATION,
    LUNARH_LUNAR_TO_GREGORIAN,
    LUNARH_LUNAR_CALENDAR_OF_A_YEAR,
};

struct ArgumentList
{
    double tz;
    int valid;
    int year;
    int month;
    int leap;
    int day;
    enum Task task;
};

struct Gregorian 
{
    double tz;
    int year;
    int month;
    int day;
    int jd;
    int valid;
};

struct Lunar 
{
    double tz;
    int year;
    int month;
    int month_size;
    int leap;
    int day;
    int jd;
    int valid;
};

struct Lunar convert_Gregorian_to_Lunar(struct Gregorian date);
struct Gregorian convert_Lunar_to_Gregorian(struct Lunar lunar);
void display_Lunar_in_Chinese(struct ArgumentList argl);
void display_Lunar_in_modern_notation(struct ArgumentList argl);
void display_Gregorian(struct ArgumentList argl);
void display_Lunar_calendar_of_a_year(struct ArgumentList argl);
const char* get_branch_of_year(int year);
const char* get_Chinese_digit(int value);
int get_kth_new_moon_jd(int k, double tz);
int get_leap_month_offset(int new_moon_11_jd, double tz);
int get_lunar_month_11_start_jd(int year, double tz);
int get_num_days_of_Gregorian_month(int year, int month);
const char* get_stem_of_year(int year);
int get_sun_longitude(int jd, double tz);
int is_leap_Gregorian_year(int year);
int is_valid_Gregorian(int year, int month, int day);
struct Gregorian new_Gregorian(int year, int month, int day, double tz);
struct Gregorian new_Gregorian_2(int jd, double tz);
struct Lunar new_Lunar(int year, int month, int leap, int day, double tz);
struct ArgumentList parse_argv(int argc, char** argv);
double sin_degrees(double degrees);
void stringify_Chinese_numeral(int value, char* out, int out_size);
void stringify_Lunar_date(struct Lunar date, char* out, int out_size);
void stringify_Lunar_day(int day, char* out, int out_size);
void stringify_Lunar_month(int month, int leap, char* out, int out_size);
void stringify_Lunar_year(int year, char* out, int out_size);
