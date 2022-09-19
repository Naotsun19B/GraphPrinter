// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace DeprecatedReminder
{
	namespace Private
	{
		// Total number of days since January 1st for each month.
    	static constexpr int32 DaysToMonth[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

		// Returns whether the specified year is a leap year.
		constexpr bool IsLeapYear(int32 Year)
		{
			return (Year % 4) == 0 && ((Year % 100) != 0) || ((Year % 400) == 0);
		}

		// Returns the total seconds for the specified date and time.
		constexpr int64 ToTotalSeconds(int32 Year, int32 Month, int32 Day, int32 Hour, int32 Minute)
		{
			int32 TotalDays = (Month > 2 && IsLeapYear(Year));

			// The current year is not a full year yet.
			--Year;
			// The current month is not a full month yet.
			--Month;

			TotalDays += Year * 365;
			// Leap year day every four years.
			TotalDays += Year / 4;
			// Except every 100 years.
			TotalDays -= Year / 100;
			// But also every 400 years.
			TotalDays += Year / 400;
			// Days in this year up to last month.
			TotalDays += DaysToMonth[Month];
			// Days in this month minus today.
			TotalDays += Day - 1;

			return (
				TotalDays * ETimespan::TicksPerDay +
				Hour * ETimespan::TicksPerHour +
				Minute * ETimespan::TicksPerMinute
			);
		}

		// Returns the Unix timestamp of the specified date and time.
		constexpr int64 ToUnixTimestamp(int32 Year, int32 Month, int32 Day, int32 Hour, int32 Minute)
		{
			constexpr int64 ReferenceTime = ToTotalSeconds(1970, 1, 1, 0, 0);
			const int64 CurrentTime = ToTotalSeconds(Year, Month, Day, Hour, Minute);

			return (CurrentTime - ReferenceTime) / ETimespan::TicksPerSecond;
		}
		
		/**
		 * A class template that determines if the current date and time is
		 * past the date and time passed in the template argument.
		 */
		template<int32 Year, int32 Month, int32 Day, int32 Hour, int32 Minute>
		class TIsDeprecated
		{
		public:
			static constexpr int64 Specified = ToUnixTimestamp(Year, Month, Day, Hour, Minute);
			static constexpr int64 AtCompile = ToUnixTimestamp(
				DEPRECATED_REMINDER_COMPILE_TIME_YEAR,
				DEPRECATED_REMINDER_COMPILE_TIME_MONTH,
				DEPRECATED_REMINDER_COMPILE_TIME_DAY,
				DEPRECATED_REMINDER_COMPILE_TIME_HOUR,
				DEPRECATED_REMINDER_COMPILE_TIME_MINUTE
			);
			static constexpr bool bValue = (
				Specified >= AtCompile
			);
		};

#if DEPRECATED_REMINDER_VERBOSITY_WARNING
		struct FTrueType {};
		struct FFalseType {};
		template <int Test> struct TConverter : public FTrueType {};
		template <> struct TConverter<0> : public FFalseType {};
#endif
	}
}

/**
 * You can switch the operation when date time has expired from the editor preferences.
 */
#if DEPRECATED_REMINDER_VERBOSITY_ERROR
#define DEPRECATED_REMINDER_INTERNAL(Year, Month, Day, Hour, Minute, ...) \
	static_assert(DeprecatedReminder::Private::TIsDeprecated<Year, Month, Day, Hour, Minute>::bValue, "This code has expired. (Period : " #Month "/" #Day "/" #Year " " #Hour ":" #Minute ")");

#elif DEPRECATED_REMINDER_VERBOSITY_WARNING
#if defined(__GNUC__)
#define DEPRECATED_REMINDER_DEPRECATE_WRAPPER(Type, Message) Type __attribute__((deprecated(Message)))
#elif defined(_MSC_VER)
#define DEPRECATED_REMINDER_DEPRECATE_WRAPPER(Type, Message) __declspec(deprecated(Message)) Type
#else
#error This compiler is not supported
#endif

#define DEPRECATED_REMINDER_COMBINE(Lhs, Rhs) DEPRECATED_REMINDER_COMBINE_INTERNAL(Lhs, Rhs)
#define DEPRECATED_REMINDER_COMBINE_INTERNAL(Lhs, Rhs) Lhs##Rhs

// Unreal Engine has COMPILE_WARNING, but I created below because we can't specify the condition like static_assert.
#define STATIC_WARNING(Condition, Message) \
	struct DEPRECATED_REMINDER_COMBINE(Line, __LINE__) \
	{ \
		DEPRECATED_REMINDER_DEPRECATE_WRAPPER(void Warning(DeprecatedReminder::Private::FFalseType const& ), Message) {}; \
		void Warning(DeprecatedReminder::Private::FTrueType const& ) {}; \
		DEPRECATED_REMINDER_COMBINE(Line, __LINE__)() \
		{ \
			Warning(DeprecatedReminder::Private::TConverter<(Condition)>()); \
		} \
	}

#define DEPRECATED_REMINDER_INTERNAL(Year, Month, Day, Hour, Minute, ...) \
	static constexpr bool b##Year##Month##Day##Hour##Minute = DeprecatedReminder::Private::TIsDeprecated<Year, Month, Day, Hour, Minute>::bValue; \
	STATIC_WARNING(b##Year##Month##Day##Hour##Minute, "This code has expired. (Period : " #Month "/" #Day "/" #Year " " #Hour ":" #Minute ")")

#else
#define DEPRECATED_REMINDER_INTERNAL(...)
#endif

/**
 * A macro that raises a compile error or warning after the date and time specified by the argument.
 * Any identifier can be given after the date and time.
 */
#define DEPRECATED_REMINDER(Year, Month, Day, Hour, Minute, ...) \
	static_assert(Month >= 1 && Month <= 12, "Month are from 1 to 12."); \
	static_assert(Day >= 1 && Day <= 31, "Day are from 1 to 31."); \
	static_assert(Hour >= 0 && Hour <= 23, "Hour are from 0 to 23."); \
	static_assert(Minute >= 0 && Minute <= 59, "Minute are from 0 to 59."); \
	DEPRECATED_REMINDER_INTERNAL(Year, Month, Day, Hour, Minute, ...)
