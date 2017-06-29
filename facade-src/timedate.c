/**
 * @file timedate.c
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 29/Sep/2012 - 18:36
 *
 * Facultad de Ciencias,
 * Universidad Nacional Autónoma de México, México.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit the following URL:
 * (http://www.gnu.org/licenses/gpl.html)
 * or write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @section DESCRIPTION
 * Library functions to translate dates between julian and gregorian calendars.
 *
 */
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include"timedate.h"

/**
 * List of valid UTC offsets. The zero entry of this array is invalid
 * and is included onl for searching purposes.
 */
char           *validUTCOffsets[UTCOFFNUM] = {
    "AAAAAAAAA\x0",
    "UTC-12:00\x0",
    "UTC-11:00\x0",
    "UTC-10:00\x0",
    "UTC-09:30\x0",
    "UTC-09:00\x0",
    "UTC-08:00\x0",
    "UTC-07:00\x0",
    "UTC-06:00\x0",
    "UTC-05:00\x0",
    "UTC-04:30\x0",
    "UTC-04:00\x0",
    "UTC-03:30\x0",
    "UTC-03:00\x0",
    "UTC-02:00\x0",
    "UTC-01:00\x0",
    "UTC+00:00\x0",
    "UTC+01:00\x0",
    "UTC+02:00\x0",
    "UTC+03:00\x0",
    "UTC+03:30\x0",
    "UTC+04:00\x0",
    "UTC+04:30\x0",
    "UTC+05:00\x0",
    "UTC+05:30\x0",
    "UTC+05:45\x0",
    "UTC+06:00\x0",
    "UTC+06:30\x0",
    "UTC+07:00\x0",
    "UTC+08:00\x0",
    "UTC+08:45\x0",
    "UTC+09:00\x0",
    "UTC+09:30\x0",
    "UTC+10:00\x0",
    "UTC+10:30\x0",
    "UTC+11:00\x0",
    "UTC+11:30\x0",
    "UTC+12:00\x0",
    "UTC+12:45\x0",
    "UTC+13:00\x0",
    "UTC+14:00\x0"
};

double
julianDate(int year, int month, int day, int hour, int min, double sec)
{
    double          djm0, djm;
    int             ly, my;
    long            iypmy;
    const int       IYMIN = -4799;
    static const int mtab[]
    = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    double          days;

    if (year < IYMIN)
        return -1;
    if (month < 1 || month > 12)
        return -2;
    /*
     * Leap year verification
     */
    ly = ((month == 2) && !(year % 4) && (year % 100 || !(year % 400)));
    if ((day < 1) || (day > (mtab[month - 1] + ly)))
        return -3;
    if (hour < 0 || hour > 23)
        return -4;
    if (min < 0 || min > 59)
        return -5;
    if (sec < 0.0 || sec >= 60.0)
        return -6;

    my = (month - 14) / 12;
    iypmy = (long) (year + my);
    djm0 = 2400000.5;
    djm = (double) ((1461L * (iypmy + 4800L)) / 4L
                    + (367L * (long) (month - 2 - 12 * my)) / 12L
                    - (3L * ((iypmy + 4900L) / 100L)) / 4L
                    + (long) day - 2432076L);
    days = (60.0 * (60.0 * ((double) abs(hour)) +
                    ((double) abs(min))) + fabs(sec)) / DAYSEC;
    return djm0 + djm + days;
}

int
calDateTime(double juliandate,
            int *year, int *month, int *day, int *hour, int *min, double *sec)
{
    static const double djmin = -68569.5;
    static const double djmax = 1e9;

    long            jd, l, n, i, k;
    double          dj, f, d;
    double          hf, mf;

    /*
     * date range is ok
     */
    if (juliandate < djmin || juliandate > djmax)
        return 0;

    dj = juliandate - 0.5;
    f = fmod(dj, 1.0);
    if (f < 0.0)
        f += 1.0;
    d = floor(dj - f);
    jd = (long) floor(d) + 1L;

    /*
     * Express day in Gregorian calendar.
     */
    l = jd + 68569L;
    n = (4L * l) / 146097L;
    l -= (146097L * n + 3L) / 4L;
    i = (4000L * (l + 1L)) / 1461001L;
    l -= (1461L * i) / 4L - 31L;
    k = (80L * l) / 2447L;
    *day = (int) (l - (2447L * k) / 80L);
    l = k / 11L;
    *month = (int) (k + 2L - 12L * l);
    *year = (int) (100L * (n - 49L) + i + l);

    hf = (f * 24.0L);
    *hour = (int) hf;
    f = hf - (double) *hour;
    mf = (f * 60.0L);
    *min = (int) mf;
    f = mf - (double) *min;
    *sec = f;

    return 1;
}

double
timeDayFrac(int hr, int min, double sec)
{
    if (hr < 0 || hr > 23)
        return -1.0;
    if (min < 0 || min > 59)
        return -2.0;
    if (sec < 0.0 || sec >= 60.0)
        return -3.0;
    return (60.0 * (60.0 * ((double) abs(hr)) +
                    ((double) abs(min))) + fabs(sec)) / DAYSEC;
}

int
isValidUTC(char *str)
{
    int             i;
    if ((str == NULL) || strlen(str) != UTCOFFLEN)
        return 0;
    for (i = UTCOFFNUM - 1; !strncasecmp(validUTCOffsets[i], str, UTCOFFLEN)
         && (i > 0); i--);
    return i;
}
/*
 * timedate.c ends here
 */
