#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lunar.h"

const double TZ = 7.0;
const double PI = 3.1415926535898;
const double TAU = 2 * PI;
const double DEGREES_TO_RADIANS = PI / 180.0;
const char* BRANCHES[] = {
    "申", "酉", "戌", "亥", "子", "丑", "寅", "卯", "辰", "巳", "午", "未"
};
const char* STEMS[] = {
    "庚", "辛", "壬", "癸", "甲", "乙", "丙", "丁", "戊", "己"
};

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        time_t raw;
        time(&raw);
        struct tm* info = localtime(&raw);
        struct Gregorian date = new_Gregorian(
            info->tm_year + 1900, 
            info->tm_mon + 1, 
            info->tm_mday,
            TZ
        );
        struct Lunar lunar = convert_Gregorian_to_Lunar(date);
        char out[256];
        stringify_Lunar_date(lunar, out, 256);
        printf("%s\n", out);
    }
    else if (argc == 4)
    {
        int year = strtod(argv[1], NULL);
        int month = strtod(argv[2], NULL);
        int day = strtod(argv[3], NULL);
        struct Gregorian date = new_Gregorian(year, month, day, TZ);
        if (!date.valid)
        {
            printf("Invalid Gregorian date: %d-%d-%d\n", year, month, day);
            return 1;
        }
        struct Lunar lunar = convert_Gregorian_to_Lunar(date);
        char out[256];
        stringify_Lunar_date(lunar, out, 256);
        printf("%s\n", out);
    }
    else if (argc == 5)
    {
        int year = strtod(argv[1], NULL);
        int month = strtod(argv[2], NULL);
        int leap = strtod(argv[3], NULL);
        int day = strtod(argv[4], NULL);
        struct Lunar lunar = new_Lunar(year, month, leap, day, TZ);
        if (!lunar.valid)
        {
            printf("Invalid Lunar date: %d-%d%s-%d\n", year, month, 
                lunar.leap ? "i" : "", day);
            return 1;
        }
        struct Gregorian date = convert_Lunar_to_Gregorian(lunar);
        printf("%04d-%02d-%02d", date.year, date.month, date.day);
    }
    else
    {
        printf("Invalid arguments\n");
        return 1;
    }
}

struct Lunar convert_Gregorian_to_Lunar(struct Gregorian date)
{
    if (!date.valid)
    {
        struct Lunar lunar = { .valid = 0 };
        return lunar;
    }
    int year = date.year;
    int jd = date.jd;
    double tz = date.tz;
    int k = (int)floor((jd - 2415021.076998695) / 29.530588853);
    int month_start_jd = get_kth_new_moon_jd(k + 1, tz);
    int next_month_start_jd;
    if (month_start_jd > jd)
    {
        next_month_start_jd = month_start_jd;
        month_start_jd = get_kth_new_moon_jd(k, tz);
    }
    else
    {
        next_month_start_jd = get_kth_new_moon_jd(k + 2, tz);
    }
    int a = get_lunar_month_11_start_jd(year, tz);
    int b = a;
    int lunar_year;
    if (a >= month_start_jd)
    {
        lunar_year = year;
        a = get_lunar_month_11_start_jd(year - 1, tz);
    }
    else
    {
        lunar_year = year + 1;
        b = get_lunar_month_11_start_jd(year + 1, tz);
    }
    int lunar_day = jd - month_start_jd + 1;
    int diff = (int)floor((month_start_jd - a) / 29);
    int lunar_leap = 0;
    int lunar_month = diff + 11;
    int offset;
    if (b - a > 365)
    {
        offset = get_leap_month_offset(a, tz);
        if (diff >= offset)
        {
            lunar_month = diff + 10;
            if (diff == offset)
            {
                lunar_leap = 1;
            }
        }
    }
    if (lunar_month > 12)
    {
        lunar_month -= 12;
    }
    if (lunar_month >= 11 && diff < 4)
    {
        lunar_year -= 1;
    }
    struct Lunar lunar = {
        .year = lunar_year,
        .month = lunar_month,
        .leap = lunar_leap,
        .month_size = next_month_start_jd - month_start_jd,
        .day = lunar_day,
        .jd = jd,
        .tz = tz,
        .valid = 1
    };
    return lunar;
}

struct Gregorian convert_Lunar_to_Gregorian(struct Lunar lunar)
{
    if (!lunar.valid)
    {
        struct Gregorian date = { .valid = 0 };
        return date;
    }
    struct Gregorian date = new_Gregorian_2(lunar.jd, lunar.tz);
    return date;
}

const char *get_branch_of_year(int year)
{
    int branch = year % 12;
    return BRANCHES[branch < 0 ? branch + 12 : branch];
}

int get_kth_new_moon_jd(int k, double tz)
{
    double t = k / 1236.85;
    double t2 = t * t;
    double t3 = t2 * t;
    double jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * t2 - 
        0.000000155 * t3 + 0.00033 * sin(
            (166.56 + 132.87 * t - 0.009173 * t2) * DEGREES_TO_RADIANS
        );
    double m = 359.2242 + 29.10535608 * k - 0.0000333 * t2 - 0.00000347 * t3;
    double mpr = 306.0253 + 385.81691806 * k + 0.0107306 * t2 + 0.00001236 * t3;
    double f = 21.2964 + 390.67050646 * k - 0.0016528 * t2 - 0.00000239 * t3;
    double c1 = (0.1734 - 0.000393 * t) * sin(m * DEGREES_TO_RADIANS) + 
        0.0021 * sin(2 * m * DEGREES_TO_RADIANS) - 
        0.4068 * sin(mpr * DEGREES_TO_RADIANS) + 
        0.0161 * sin(2 * mpr * DEGREES_TO_RADIANS) -
        0.0004 * sin(3 * mpr * DEGREES_TO_RADIANS) + 
        0.0104 * sin(2 * f * DEGREES_TO_RADIANS) - 
        0.0051 * sin((m + mpr) * DEGREES_TO_RADIANS) - 
        0.0074 * sin((m - mpr) * DEGREES_TO_RADIANS) + 
        0.0004 * sin((2 * f + m) * DEGREES_TO_RADIANS) - 
        0.0004 * sin((2 * f - m) * DEGREES_TO_RADIANS) - 
        0.0006 * sin((2 * f + mpr) * DEGREES_TO_RADIANS) + 
        0.0010 * sin((2 * f - mpr) * DEGREES_TO_RADIANS) + 
        0.0005 * sin((2 * mpr + m) * DEGREES_TO_RADIANS);
    double delta_t;
    if (t < -11)
    {
        delta_t = 0.001 + 0.000839 * t + 0.0002261 * t2 - 0.00000845 * t3 - 
            0.000000081 * t3 * t;
    }
    else
    {
        delta_t = -0.000278 + 0.000265 * t + 0.000262 * t2;
    }
    double jd = jd1 + c1 - delta_t;
    return (int)floor(jd + 0.5 + tz / 24);
}

int get_leap_month_offset(int new_moon_11_jd, double tz)
{
    int k = floor((new_moon_11_jd - 2415021.076998695) / 29.530588853 + 0.5);
    int last = 0;
    int i = 1;
    int arc = get_sun_longitude(get_kth_new_moon_jd(k + 1, tz), tz);
    while (1)
    {
        last = arc;
        i += 1;
        arc = get_sun_longitude(get_kth_new_moon_jd(k + i, tz), tz);
        if (arc == last || i >= 14)
        {
            break;
        }
    }
    return i - 1;
}

int get_lunar_month_11_start_jd(int year, double tz)
{
    struct Gregorian date = new_Gregorian(year, 12, 31, tz);
    int offset = date.jd - 2415021;
    int k = floor(offset / 29.530588853);
    int new_moon_jd = get_kth_new_moon_jd(k, tz);
    int sun_longitude = get_sun_longitude(new_moon_jd, tz);
    if (sun_longitude >= 9) {
        return get_kth_new_moon_jd(k - 1, tz);
    }
    return new_moon_jd;
}

int is_leap_Gregorian_year(int year)
{
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

const char* get_Chinese_digit(int value)
{
    switch (value)
    {
    case 1:
        return "一";
    case 2: 
        return "二";
    case 3:
        return "三";
    case 4:
        return "四";
    case 5:
        return "五";
    case 6:
        return "六";
    case 7:
        return "七";
    case 8:
        return "八";
    case 9:
        return "九";
    case 10:
        return "十";
    case 20:
        return "廿";
    case 30:
        return "卅";
    default:
        return "";
    }
}

int get_num_days_of_Gregorian_month(int year, int month)
{
    switch (month)
    {
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;

    case 2:
        return is_leap_Gregorian_year(year) ? 29 : 28;
    
    default:
        return 31;
    }
}

const char *get_stem_of_year(int value)
{
    int stem = value % 10;
    return STEMS[stem < 0 ? stem + 10 : stem];
}

int get_sun_longitude(int jd, double tz)
{
    double t = (jd - 2451545.5 - tz / 24.0) / 36525;
    double t2 = t * t;
    double m = 357.52910 + 35999.05030 * t - 0.0001559 * t2 - 0.00000048 * t2 * t;
    double long0 = 280.46645 + 36000.76983 * t + 0.0003032 * t2;
    double dl = 
        (1.914600 - 0.004817 * t - 0.000014 * t2) * sin(m * DEGREES_TO_RADIANS) + 
        (0.019993 - 0.000101 * t) * sin(2 * m * DEGREES_TO_RADIANS) + 
        0.000290 * sin(3 * m * DEGREES_TO_RADIANS);
    double longitude = (long0 + dl) * DEGREES_TO_RADIANS;
    longitude -= TAU * floor(longitude / TAU);
    return (int)floor(longitude / PI * 6);
}

int is_valid_Gregorian(int year, int month, int day)
{
    if (month < 1 || month > 12)
    {
        return 0;
    }
    return day >= 1 && day <= get_num_days_of_Gregorian_month(year, month);
}

struct Gregorian new_Gregorian(int year, int month, int day, double tz)
{
    if (!is_valid_Gregorian(year, month, day))
    {
        struct Gregorian date = { .valid = 0 };
        return date;
    }
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    struct Gregorian date = { 
        .year = year, .month = month, .day = day, .jd = jd, .tz = tz, .valid = 1
    };
    return date;
}

struct Gregorian new_Gregorian_2(int jd, double tz)
{
    int a = jd + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - (b * 146097) / 4;
    int d = (4 * c + 3) / 1461;
    int e = c - (1461 * d) / 4;
    int m = (5 * e + 2) / 153;
    int day = e - (153 * m + 2) / 5 + 1;
    int month = m + 3 - 12 * (m / 10);
    int year = b * 100 + d - 4800 + m / 10;
    struct Gregorian date = { 
        .year = year, .month = month, .day = day, .jd = jd, .tz = tz, .valid = 1 
    };
    return date;
}

struct Lunar new_Lunar(int year, int month, int leap, int day, double tz)
{
    struct Lunar lunar = { .valid = 0 };
    if (month < 1 || month > 12 || day < 1 || day > 30)
    {
        return lunar;
    }
    int a;
    int b;
    if (month < 11)
    {
        a = get_lunar_month_11_start_jd(year - 1, tz);
        b = get_lunar_month_11_start_jd(year, tz);
    }
    else 
    {
        a = get_lunar_month_11_start_jd(year, tz);
        b = get_lunar_month_11_start_jd(year + 1, tz);
    }
    int offset = month - 11;
    if (offset < 0)
    {
        offset += 12;
    }
    if (b - a > 365)
    {
        int leap_month_offset = get_leap_month_offset(a, tz);
        int leap_month = leap_month_offset - 2;
        if (leap_month < 0)
        {
            leap_month += 12;
        }
        if (leap != 0 && month != leap_month)
        {
            return lunar;
        }
        else if (leap != 0 || offset >= leap_month_offset)
        {
            offset += 1;
        }
    }
    int k = (int)floor(0.5 + (a - 2415021.076998695) / 29.530588853);
    int month_start_jd = get_kth_new_moon_jd(k + offset, tz);
    int next_month_start_jd = get_kth_new_moon_jd(k + offset + 1, tz);
    int jd = month_start_jd + day - 1;
    lunar.year = year;
    lunar.month = month;
    lunar.leap = leap;
    lunar.month_size = next_month_start_jd - month_start_jd;
    lunar.day = day;
    lunar.jd = jd;
    lunar.valid = 1;
    return lunar;
}

void stringify_Chinese_numeral(int value, char *out, int out_size)
{
    int tens = (int)floor(value / 10.0) * 10;
    int units = value - tens;
    sprintf_s(out, out_size, "%s%s", get_Chinese_digit(tens), 
        get_Chinese_digit(units));
}

void stringify_Lunar_date(struct Lunar date, char *out, int out_size)
{
    char year[64];
    stringify_Lunar_year(date.year, year, 64);
    char month[64];
    stringify_Lunar_month(date.month, date.leap, month, 64);
    char day[64];
    stringify_Lunar_day(date.day, day, 64);
    sprintf_s(out, out_size, "%s%s（%s）%s", year, month, 
        date.month_size == 29 ? "小" : "大", day);
}

void stringify_Lunar_day(int day, char *out, int out_size)
{
    char s[64];
    stringify_Chinese_numeral(day, s, 64);
    sprintf_s(out, out_size, "%s%s日", day < 11 ? "初" : "", s);
}

void stringify_Lunar_month(int month, int leap, char *out, int out_size)
{
    char s[64];
    stringify_Chinese_numeral(month, s, 64);
    if (strncmp(s, "一", 64) == 0)
    {
        sprintf_s(s, 64, "正");
    }
    sprintf_s(out, out_size, "%s%s月", leap == 1 ? "閏" : "", s);
}

void stringify_Lunar_year(int year, char *out, int out_size)
{
    sprintf_s(out, out_size, "%s%s年", get_stem_of_year(year), 
        get_branch_of_year(year));
}
