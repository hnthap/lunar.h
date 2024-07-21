# Gregorian-Lunar (Lunisolar) Calendars Converter

Converting from Gregorian calendar to Vietnamese Lunar calendar (change `TZ` in `lunar.c` to 8 for Chinese Lunar calendar), and from Lunar calendar to Gregorian.

Astronomical algorithms from [Ho Ngoc Duc's work](https://www.informatik.uni-leipzig.de/~duc/amlich/calrules.html).

I re-wrote the algorithms in C to implement an easy-to-use CLI application. This requires a compiler that supports C99 or later.

## Compilation

In my case, I use gcc 8.3.0, but any other compilers that support C99 may work just fine (have not tested yet).

```bash
# Change directory to the directory containing this README
# then run this

gcc -Wpedantic -g src/lunar.c -o bin/lunar
```

## Arguments

* `-tz`: Time zone in hours, e.g. `-tz 7` (default) is for UTC+07:00, `-tz 8` is for UTC+08:00.
* `-y`: Year, e.g. `-y 2024` is the year 2024.
* `-m`: Month from 1 to 12, e.g. `-m 7` is July (Gregorian calendar) or the 7th month (Lunar calendar).
* `-d`: Day of month, from 1 to 31, e.g. `-d 21` is the 21st day of the month.
* `-l`: (Lunar calendar only) Whether the month is leap. When `-l` does not present, the month is set as not leap, otherwise leap.
* `-lm`: (Only when displaying Lunar calendar) Display the date in Lunar calendar in "modern notation", e.g.
  * 2024.06.16: The **16th** day of the **6th** month (**non-leap**) of the Lunar year whose New Year falls in the Gregorian year **2024**.
  * 2025.06+.01: The **1st** day of the **6th** month (**leap**) of the Lunar year whose New Year falls in the Gregorian year **2025**.
* `-r`: Converting the date specified by `-y`, `-m`, `-l` and `-d` in Lunar calendar to Gregorian.
* `-ly`: Print the first days of all months in the Lunar year specified by `-y` along with their equivalent Gregorian date.

## Usage

Convert date from Gregorian calendar to Lunar calendar:

```bash
./bin/lunar 
# Output: "甲辰年六月（小）十六日"
# Explain: Today in Lunar calendar.
# Set "-tz 8" for Chinese calendar.

./bin/lunar -lm
# Output: "2024.06.16" 
# Explain: Today in Lunar calendar - modern notation.
# Set "-tz 8" for Chinese calendar.

./bin/lunar -y 2025 -m 7 -d 30
# Output: "乙巳年閏六月（小）初六日" 
# Explain: 2025-07-30 in Gregorian calendar to Lunar.
# Set "-tz 8" for Chinese calendar.

./bin/lunar -y 2025 -m 7 -d 30 -lm
# Output: "2025.06+.06"
# Explain: 2025-07-30 in Gregorian calendar to Lunar - modern notation.
# Set "-tz 8" for Chinese calendar.
```

Convert date from Lunar calendar to Gregorian calendar:

```bash
./bin/lunar -r -y 2025 -m 6 -l -d 6
# Output: 2025.07.30
# Explain: Lunar date 2025.06+.06 is Gregorian date 2025.07.30 in time zone UTC+07:00 (Vietnamese calendar).
# Set "-tz 8" for Chinese calendar.
```

List first days of all months in a Lunar year with their equivalent Gregorian dates:

```bash
./bin/lunar -ly -y 2025            
# Output:
# Lunar   Gregorian
# 2025.01.01      2025.01.29
# 2025.02.01      2025.02.28
# 2025.03.01      2025.03.29
# 2025.04.01      2025.04.28
# 2025.05.01      2025.05.27
# 2025.06.01      2025.06.25
# 2025.06+.01     2025.07.25
# 2025.07.01      2025.08.23
# 2025.08.01      2025.09.22
# 2025.09.01      2025.10.21
# 2025.10.01      2025.11.20
# 2025.11.01      2025.12.20
# 2025.12.01      2026.01.19
```

