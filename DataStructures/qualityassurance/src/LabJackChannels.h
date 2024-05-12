#ifndef LABJACK_CHANNELS_H
#define LABJACK_CHANNELS_H

#include <QObject>
#include <QString>
#include <QDateTime>

namespace t7 { // labjack model t7

enum class CLOCK : int { CLOCK0 = 0, // 32 bit clock, combination of CLOCK1 and CLOCK2
                         CLOCK1 = 1, // 16 bit clock, CIO0
                         CLOCK2 = 2 }; // 16 bit clock, CIO1

enum class DIO : int { DIO_FIRST = 0,
                       DIO0 = 0,
                       DIO1 = 1,
                       DIO2 = 2,
                       DIO3 = 3,
                       DIO4 = 4,
                       DIO5 = 5,
                       DIO6 = 6,
                       DIO7 = 7,
                       DIO8 = 8,
                       DIO9 = 9,
                       DIO10 = 10,
                       DIO11 = 11,
                       DIO12 = 12,
                       DIO13 = 13,
                       DIO14 = 14,
                       DIO15 = 15,
                       DIO16 = 16,
                       DIO17 = 17,
                       DIO18 = 18,
                       DIO19 = 19,
                       DIO20 = 20,
                       DIO21 = 21,
                       DIO22 = 22,
                       DIO_LAST = 22};



enum class AIN : int { AIN_FIRST = 0,
                       AIN0 = 0,
                       AIN1 = 1,
                       AIN2 = 2,
                       AIN3 = 3,
                       AIN4 = 4,
                       AIN5 = 5,
                       AIN6 = 6,
                       AIN7 = 7,
                       AIN8 = 8,
                       AIN9 = 9,
                       AIN10 = 10,
                       AIN11 = 11,
                       AIN12 = 12,
                       AIN13 = 13,
                       AIN_LAST = 14 };

enum class AOUT : int { AOUT_0 = 0,
                        AOUT_1 = 1 };

enum class PULSEOUT : int { DIO0 = 0,
                            DIO2 = 2,
                            DIO3 = 3,
                            DIO4 = 4,
                            DIO5 = 5 };

}

#endif
