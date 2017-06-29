/**
 * @file timedate.h
 *
 * @author José Galaviz <jgc@fciencias.unam.mx>
 * @version 1.0
 * \date 28/Sep/2012 - 11:52
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
 * Time and date calculations. Some functions to perform
 * Julian-Gregorian calendar calculations.
 *
 */

#ifndef TIMEDATE_H
#define TIMEDATE_H

#include<math.h>

/**
 * Seconds in a day
 */
#define DAYSEC (86400.0)

/**
 * List of valid time zones UTC offsets.
 * - char *validUTCOffsets[UTCOFFNUM] = {
 *   - "UTC-12:00",
 *   - "UTC-11:00",
 *   - "UTC-10:00",
 *   - "UTC-09:30",
 *   - "UTC-09:00",
 *   - "UTC-08:00",
 *   - "UTC-07:00",
 *   - "UTC-06:00",
 *   - "UTC-05:00",
 *   - "UTC-04:30",
 *   - "UTC-04:00",
 *   - "UTC-03:30",
 *   - "UTC-03:00",
 *   - "UTC-02:00",
 *   - "UTC-01:00",
 *   - "UTC+00:00",
 *   - "UTC+01:00",
 *   - "UTC+02:00",
 *   - "UTC+03:00",
 *   - "UTC+03:30",
 *   - "UTC+04:00",
 *   - "UTC+04:30",
 *   - "UTC+05:00",
 *   - "UTC+05:30",
 *   - "UTC+05:45",
 *   - "UTC+06:00",
 *   - "UTC+06:30",
 *   - "UTC+07:00",
 *   - "UTC+08:00",
 *   - "UTC+08:45",
 *   - "UTC+09:00",
 *   - "UTC+09:30",
 *   - "UTC+10:00",
 *   - "UTC+10:30",
 *   - "UTC+11:00",
 *   - "UTC+11:30",
 *   - "UTC+12:00",
 *   - "UTC+12:45",
 *   - "UTC+13:00",
 *   - "UTC+14:00".
 * - }
 */
/**
 * Number of UTC offsets (plus one) actually used worldwide
 */
#define UTCOFFNUM 41

/**
 * Length of valid UTC offset string
 */
#define UTCOFFLEN 9

/**
 * \brief Translates a gregorian calendar date, with time included, to
 * its julian date equivalent.
 *
 * This function obtains the full julian date (including day fraction)
 * equivalent to the given gregorian date. The integer part of julian
 * date (julian day number or JDN) changes at noon, at midnight the
 * fractional part is 0.5. Therefore, if the user wants only the julian
 * day number (JDN) of a given date, the day hour must be past noon or
 * must be added 1 to the JDN returned, or result must be rounded if
 * fractional part is greater than or equal to 0.5.
 * Funtion is useful for dates after January 1 of -4713, which
 * corresponds to julian date 0.
 *
 * @param[in] year is the year number of the date to be translated.
 * @param[in] month is the month of the date to be translated.
 * @param[in] day is the day number of the date to be translated.
 * @param[in] hour is the hour number of the date to be translated.
 * @param[in] min is the minute number of the date to be
 * translated.
 * @param[in] sec is the second number of the date to be
 * translated.
 *
 * \return
 * - the julian date >= 0, whose integer part is the julian day
 * number, and fractional part is the fraction of the day minus 0.5.
 * - negative number if some error occurs.
 *   * -1 year out of range.
 *   * -2 month out of range.
 *   * -3 day out of range.
 *   * -4 hour out of range.
 *   * -5 minute out of range.
 *   * -6 seconds out of range.
 *
 * \pre year > -4713
 * \pre month in {1,..., 12}
 * \pre day in {1,..., lastdayofmonth} where lastdayofmonth is
 * {31, * 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} and february
 * (month 2) has 29 days in a leap year. Leap year are those which are
 * multiples of 4, unless multiples of 100, unless multiples of 400.
 * \pre hour in {0,.., 23}
 * \pre min in {0,..., 59}
 * \pre sec in [0, 60)
 */
double          julianDate(int year, int month, int day,
                           int hour, int min, double sec);

/**
 * \brief Translates a julian date to the equivalent calendar date
 * (gregorian).
 *
 * Given a julian date, this fuction returns the equivalent calendar
 * date and time of hte day. It's assumed that, as defined for julian
 * dates, the julian day begins at noon. The fractional part of julian
 * date is, therefore, the fraction of the day minus 0.5.
 *
 * @param[in] juliandate is the real number corresponding to a julian date
 * with fractional day to be translated.
 * @param[out] year is the year number of the calendar date obtained.
 * @param[out] month is the month of the calendar date obtained.
 * @param[out] day is the day number of the calendar date obtained.
 * @param[out] hour is the hour number of the calendar date obtained.
 * @param[out] min is the minute number of the calendar date obtained.
 * @param[out] sec is the second number of the calendar date obtained.
 *
 * \return 1 if success, 0 otherwise
 *
 * \pre juliandate in [-68569.5, 1,000,000,000]
 */
int             calDateTime(double juliandate,
                            int *year, int *month, int *day,
                            int *hour, int *min, double *sec);

/**
 * \brief Returns the number os days and decimal fractions of the day
 * equivalent to the given amount of time.
 *
 * Calculation of number of days anf fraction that correspondes to a
 * given amount of time expressed in hours, minutes and seconds (with
 * fractional part).
 *
 * @param[in] hr is the amount of hours.
 * @param[in] min is the amount of minutes.
 * @param[in] sec is the amount of seconds.
 *
 * \return a non-negative real number (as double) with the amount of
 * days corresponding to the given amount of time, or:
 * - -1 if hr is out of range.
 * - -2 if min is out of range.
 * - -3 if sec is out of range.
 *
 * \pre hr in {0, ..., 23}
 * \pre min in {0, ..., 59}
 * \pre sec in [0, 60)
 * \post result >= 0 if success, result < 0 otherwise.
 */
double          timeDayFrac(int hr, int min, double sec);

/**
 * \brief Validates a UTC offset string.
 *
 * Given a string, this fuction determines if such string is a valid
 * UTC offset string in the ISO 8601 format (UTC[+|-]HH:MM). The
 * string must be also an existing UTC.
 *
 * @param str is the string which must be validated.
 * \return 1 is the string format is valid and their UTC exist, 0
 * otherwise.
 *
 * \pre str must have the prefix "UTC" in uppercase, followed by a
 * plus or minus sign, HH in {00, ..., 23}, MM in {00, ..., 59}, if
 * the UTC HH:MM is zero only the plus sign is valid. The offset must
 * also exist in the world. validUTCOffsets contains the only valid
 * values.
 */
int             isValidUTC(char *str);

#endif
/*
 * end of timedate.h
 */
