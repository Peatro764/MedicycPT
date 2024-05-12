#include "LabJack.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <string>
#include <QTimer>
#include <QElapsedTimer>

#include "/usr/local/include/LabJackM.h"
#include "/usr/local/include/LJM_Utilities.h"

#include "Calc.h"

LabJack::LabJack() {}

LabJack::~LabJack() {}

void LabJack::Connect(QString ip) {
    GetAndPrintConfigValue(LJM_LIBRARY_VERSION);
    SetConfigValue(LJM_OPEN_TCP_DEVICE_TIMEOUT_MS, 500);
    SetConfigValue(LJM_SEND_RECEIVE_TIMEOUT_MS, 500);

    int error = LJM_Open(LJM_dtT7, LJM_ctETHERNET, ip.toStdString().c_str(), &device_handle);
    if (error == 0) {
        qDebug() << "LabJack::Connect ok";
        connected_ = true;
    } else {
        connected_ = false;
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        throw std::runtime_error(error_string);
    }
}

void LabJack::ConnectUSB() {
    int error = LJM_Open(LJM_dtT7, LJM_ctUSB, "ANY", &device_handle);
    if (error == 0) {
        qDebug() << "LabJack::Connect ok";
        connected_ = true;
    } else {
        connected_ = false;
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        throw std::runtime_error(error_string);
    }
}

void LabJack::Disconnect() {
    LJM_Close(device_handle);
    connected_ = false;
}

void LabJack::LoadConstantsFile() {
//    Q_INIT_RESOURCE(labjack_config);
//    QFile constants_file(":/config/ljm_constants.json");
//    if (!constants_file.open(QFile::ReadOnly)) {
//        throw std::runtime_error("LabJack::LoadConstantsFile File could not be opened");
//    }
//    QString file_content = constants_file.readAll();
//    int error = LJM_LoadConstantsFromString(file_content.toStdString().c_str());
//    if (error != 0) {
//        char error_string[LJM_MAX_NAME_SIZE];
//        LJM_ErrorToString(error, error_string);
//        throw std::runtime_error(std::string("LabJack::LoadConstantsFile Error: ") + error_string);
//    }
//    qDebug() << "LabJack::LoadConstantsFile ok";
}

void LabJack::LoadConfigurationFile() {
    int error = LJM_LoadConfigurationFile("/usr/local/share/LabJack/LJM/ljm_startup_configs.json");
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        throw std::runtime_error(std::string("LabJack::LoadConfigurationFile Error: ") + error_string);
    }
    qDebug() << "LabJack::LoadConfigurationFile Success";

//    error = LJM_WriteLibraryConfigStringS(LJM_DEBUG_LOG_FILE, "ljm.log");
//    if (error != 0) {
//        char error_string[LJM_MAX_NAME_SIZE];
//        LJM_ErrorToString(error, error_string);
//        qDebug() << "LabJack::LoadConfigurationFile Error: " << error_string;
//    } else {
//        qDebug() << "LabJack::LoadConfigurationFile Success";
//    }

//    char DebugLogFile[100];
//    error = LJM_ReadLibraryConfigStringS(LJM_DEBUG_LOG_FILE, &DebugLogFile[0]);
//    if (error != 0) {
//        char error_string[LJM_MAX_NAME_SIZE];
//        LJM_ErrorToString(error, error_string);
//        qDebug() << "LabJack::LoadConfigurationFile Error: " << error_string;
//    }
//    printf("LJM_DEBUG_LOG_FILE is now %s\n", DebugLogFile);
//    qDebug() << "FILE " << DebugLogFile;
}

void LabJack::SetDefaultToCurrent() {
    // The current configuration is saved to memory and will be used as powerup defaults
    int error = LJM_eWriteName(device_handle, "IO_CONFIG_SET_DEFAULT_TO_CURRENT", 1);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << QString("LabJack::SetDefaultToCurrent Error ") + error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::SetDefaultToCurrent";
}

void LabJack::SetCurrentToDefault() {
    // The config saved in memory is retrieved and replaces the current configuration
    int error = LJM_eWriteName(device_handle, "IO_CONFIG_SET_CURRENT_TO_DEFAULT", 1);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << QString("LabJack::SetCurrentToDefault Error ") + error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::SetCurrentToDefault";
}

void LabJack::ConfigureRapidAINScan(AIN ch, int n_samples, int scan_rate) {
    const int NUM_FRAMES = 3;
    std::string base_string = "AIN" + std::to_string(static_cast<int>(ch));
    qDebug() << "LabJack::ConfigureRapidAINScan " << QString::fromStdString(base_string);
    std::string index_string = base_string + "_EF_INDEX";
    std::string config_a_string = base_string + "_EF_CONFIG_A";
    std::string config_d_string = base_string + "_EF_CONFIG_D";
    const char * aNamesConfigure[NUM_FRAMES] = {
        index_string.c_str(),
        config_a_string.c_str(), // number of samples
        config_d_string.c_str() // scan rate
    };
    double aValuesConfigure[NUM_FRAMES] = {
        static_cast<double>(3), // average and min max
        static_cast<double>(n_samples),
        static_cast<double>(scan_rate)
    };
    int error_address = INITIAL_ERR_ADDRESS;
    int error = LJM_eWriteNames(device_handle, NUM_FRAMES, aNamesConfigure,
                    aValuesConfigure, &error_address);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ConfigureRapidAINScan Error " << error_string;
        throw std::runtime_error(error_string);
        return;
    }
    qDebug() << "LabJack::ConfigureRapidAINScan";
}

double LabJack::ReadRapidAINScan(AIN ch) {
    std::string cmd = "AIN" + std::to_string(static_cast<int>(ch)) + "_EF_READ_A";
    double value = 0;
    int error = LJM_eReadName(device_handle, cmd.c_str(), &value);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadRapidAINScan Error " << error_string;
        throw std::runtime_error(error_string);
    }
    return value;
}

void LabJack::DisableRapidAINScan(AIN ch) {
    std::string cmd = "AIN" + std::to_string(static_cast<int>(ch)) + "_EF_INDEX";
    qDebug() << "LabJack::DisableRapidAINScan " << QString::fromStdString(cmd);
    int error = LJM_eWriteName(device_handle, cmd.c_str(), 0);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::DisableRapidAINScan " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::DisableRapidAINScan " << static_cast<int>(ch);
}

void LabJack::ConfigureAIN(AIN ch, int single_ended_channel, double range, int resolution_index, int settling_us) {
    const int NUM_FRAMES_CONFIGURE = 4;
    std::string base = "AIN" + std::to_string(static_cast<int>(ch));
    std::string single_ended_channel_string = base + "_NEGATIVE_CH";
    std::string range_string = base + "_RANGE";
    std::string resolution_index_string = base + "_RESOLUTION_INDEX";
    std::string settling_us_string = base + "_SETTLING_US";
    const char * aNamesConfigure[NUM_FRAMES_CONFIGURE] = {
        single_ended_channel_string.c_str(),
        range_string.c_str(),
        resolution_index_string.c_str(),
        settling_us_string.c_str()
    };
    double aValuesConfigure[NUM_FRAMES_CONFIGURE] = {
        static_cast<double>(single_ended_channel),
        range, // 10, 1.0, 0.1, 0.001
        static_cast<double>(resolution_index),
        static_cast<double>(settling_us)
    };

    int error_address = INITIAL_ERR_ADDRESS;
    int error = LJM_eWriteNames(device_handle, NUM_FRAMES_CONFIGURE, aNamesConfigure,
                    aValuesConfigure, &error_address);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << QString("LabJack::ConfigureAIN Error ") + error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::ConfigureAIN " + QString::number(static_cast<int>(ch));
}

double LabJack::ReadAIN(AIN ch) {
    std::string cmd = "AIN" + std::to_string(static_cast<int>(ch));
    double ch_value = 0;
    int error = LJM_eReadName(device_handle, cmd.c_str(), &ch_value);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadAIN Error " << error_string;
        throw std::runtime_error(error_string);
    }
//    qDebug() << "LabJack::ReadAIN " << cmd.c_str() << " " << ch_value;
    return ch_value;
}

std::vector<double> LabJack::ReadAINS(AIN ch1, AIN ch2, int n_measurements) {
    if (n_measurements < 1) {
        throw std::runtime_error("Cannot do less than 1 measurement");
    }
    std::vector<double> ch1_values;
    std::vector<double> ch2_values;
    double ch1_value = 0;
    double ch2_value = 0;
    for (int i = 0; i < n_measurements; ++i) {
        ReadAINS(ch1, ch2, &ch1_value, &ch2_value);
        ch1_values.push_back(ch1_value);
        ch2_values.push_back(ch2_value);
    }
    std::vector<double> values { calc::Mean(ch1_values), calc::Mean(ch2_values) };
    return values;
}

void LabJack::ReadAINS(AIN ch1, AIN ch2, double *value_ch1, double *value_ch2) {
    const int NUM_CH = 2;
    double ainValues[NUM_CH];
    std::string ch_name1 = "AIN" + std::to_string(static_cast<int>(ch1));
    std::string ch_name2 = "AIN" + std::to_string(static_cast<int>(ch2));
    const char* ainNames[NUM_CH] = {
        ch_name1.c_str(),
        ch_name2.c_str()
    };

    int error_address = INITIAL_ERR_ADDRESS;
    int error = LJM_eReadNames(device_handle, NUM_CH, ainNames,
                    ainValues, &error_address);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << QString("LabJack::ReadAINS Error ") + error_string;
        throw std::runtime_error(error_string);
    }

    *value_ch1 = ainValues[0];
    *value_ch2 = ainValues[1];
}

void LabJack::MultiReadAIN(AIN ch, int n_times) {
    QFile file("/home/hofverberg/ain.csv");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "LabJack::MultiReadAIN Could not open file";
        return;
    }
    QElapsedTimer timer;
    timer.start();
    QTextStream out(&file);
    for (int i = 0; i < n_times; ++i) {
        out <<  QString::number(timer.elapsed()) << ", " << QString::number(ReadAIN(ch), 'f', 10) << "\n";
    }
    qDebug() << "LabJack::MultiReadAIN Success";
}


//double LabJack::ReadAOUT(AOUT ch) {
//    std::string cmd = "DAC" + std::to_string(static_cast<int>(ch));
//    double ch_value = 0.0;
//    int error = LJM_eReadName(device_handle, cmd.c_str(), &ch_value);
//    if (error != 0) {
//        char error_string[LJM_MAX_NAME_SIZE];
//        LJM_ErrorToString(error, error_string);
//        qDebug() << "LabJack::ReadAOUT Error " << error_string;
//        throw std::runtime_error(error_string);
//    }
//    qDebug() << "LabJack::ReadAOUT " << static_cast<int>(ch) << " " << ch_value;
//    return ch_value;
//}

void LabJack::WriteAOUT(AOUT ch, double voltage) {
    std::string cmd = "DAC" + std::to_string(static_cast<int>(ch));
    int error = LJM_eWriteName(device_handle, cmd.c_str(), voltage);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::WriteAOUT Error " << error_string;
        throw std::runtime_error(error_string);
    }
//    qDebug() << "LabJack::WriteDAC " << static_cast<int>(ch) << " " << voltage;
}

void LabJack::ConfigureDIO(DIO ch, bool output) {
    qDebug() << "LabJack::ConfigureDIO " << static_cast<int>(ch);
    uint32_t directions = ReadDIODirections();
    uint32_t mask = (1 << static_cast<int>(ch));
    uint32_t value = (static_cast<int>(output) << static_cast<int>(ch));
    directions = (directions & ~mask) + value;
    std::string cmd = "DIO_DIRECTION";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), static_cast<double>(directions));
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ConfigureDIO Error " << error_string;
        throw std::runtime_error(error_string);
    }
}

uint32_t LabJack::ReadDIODirections() {
    std::string cmd = "DIO_DIRECTION";
    double direction(0.0);
    int error = LJM_eReadName(device_handle, cmd.c_str(), &direction);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadDIODirections Error " << error_string;
        throw std::runtime_error(error_string);
    }
    return static_cast<uint32_t>(direction);
}

bool LabJack::ReadDIO(DIO ch) {
    std::string cmd = "DIO_STATE";
    double block_value = 0;
    int error = LJM_eReadName(device_handle, cmd.c_str(), &block_value);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadDIO Error " << error_string;
        throw std::runtime_error(error_string);
    }
    uint32_t value = static_cast<uint32_t>(block_value);
    uint32_t mask = (1 << static_cast<int>(ch));
    int ch_value = (value & mask);
//    qDebug() << "LabJack::ReadDIO " << static_cast<int>(ch) << " " << (ch_value > 0);
    return (ch_value > 0);
}

void LabJack::WriteDIO(DIO ch, bool high) {
    uint32_t directions = ReadDIODirections();
    if (!(directions & (1 << static_cast<int>(ch)))) {
        qWarning() << "LabJack::WriteDIO Not allowed on channel " << static_cast<int>(ch) << " Directions " << directions;
        throw std::runtime_error("DIO channel is configured as input, writes are forbidden");
    }
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch));
    int error = LJM_eWriteName(device_handle, cmd.c_str(), static_cast<double>(high));
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::WriteDIO Error " << error_string;
        throw std::runtime_error(error_string);
    }
//    qDebug() << "LabJack::WriteDIO " << static_cast<int>(ch) << " " << high;
}

void LabJack::ConfigureClock(CLOCK clock, int divisor, int rollvalue) {
    const int NUM_FRAMES_CONFIGURE = 3;
    std::string base_string = "DIO_EF_CLOCK" + std::to_string(static_cast<int>(clock));
    std::string enable_string =  base_string + "_ENABLE";
    std::string divisor_string = base_string + "_DIVISOR";
    std::string rollvalue_string = base_string + "_ROLL_VALUE";
    const char * aNamesConfigure[NUM_FRAMES_CONFIGURE] = {
        enable_string.c_str(),
        divisor_string.c_str(),
        rollvalue_string.c_str()
    };
    double aValuesConfigure[NUM_FRAMES_CONFIGURE] = {
        0, // disable
        static_cast<double>(divisor), // -> 10 MHz
        static_cast<double>(rollvalue) // -> rollover time 1ms s
    };

    int error_address = INITIAL_ERR_ADDRESS;
    int error = LJM_eWriteNames(device_handle, NUM_FRAMES_CONFIGURE, aNamesConfigure,
                    aValuesConfigure, &error_address);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ConfigureClock Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::ConfigureClock " << static_cast<int>(clock);
}

void LabJack::StartClock(CLOCK clock) {
    std::string cmd = "DIO_EF_CLOCK" + std::to_string(static_cast<int>(clock)) + "_ENABLE";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), 1);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::StartClock Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::StartClock " << static_cast<int>(clock);
}

void LabJack::StopClock(CLOCK clock) {
    std::string cmd = "DIO_EF_CLOCK" + std::to_string(static_cast<int>(clock)) + "_ENABLE";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), 0);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::StopClock Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::StopClock " << static_cast<int>(clock);
}

int32_t LabJack::ReadClock(CLOCK clock) {
    double value = 0;
    std::string cmd = "DIO_EF_CLOCK" + std::to_string(static_cast<int>(clock)) + "_COUNT";
    int error = LJM_eReadName(device_handle, cmd.c_str(), &value);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadClock Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::ReadClock " << static_cast<int>(clock) << " " << value;
    return static_cast<int32_t>(value);
}

void LabJack::ConfigurePulseOut(PULSEOUT ch, CLOCK clock, int clockCount_highToLow, int clockCount_lowToHigh, int n_pulses) {
    const int NUM_FRAMES_CONFIGURE = 8;
    std::string base_string = "DIO" + std::to_string(static_cast<int>(ch));
    std::string enable_string = base_string + "_EF_ENABLE";
    std::string index_string = base_string + "_EF_INDEX";
    std::string options_string = base_string + "_EF_OPTIONS";
    std::string config_a_string = base_string + "_EF_CONFIG_A";
    std::string config_b_string = base_string + "_EF_CONFIG_B";
    std::string config_c_string = base_string + "_EF_CONFIG_C";
    std::string config_d_string = base_string + "_EF_CONFIG_D";
    const char * aNamesConfigure[NUM_FRAMES_CONFIGURE] = {
        base_string.c_str(),
        enable_string.c_str(),
        index_string.c_str(),
        options_string.c_str(), // which clock source to use
        config_a_string.c_str(), // clock count when transition high -> low
        config_b_string.c_str(), // clock count when transition low -> high
        config_c_string.c_str(), // Number of pulses to generate
        config_d_string.c_str() // Not used
    };
    double aValuesConfigure[NUM_FRAMES_CONFIGURE] = {
        static_cast<double>(0), // line must start low
        static_cast<double>(0),
        static_cast<double>(2), // PulseOut index
        static_cast<double>(clock), // Clock0
        static_cast<double>(clockCount_highToLow), //5000,
        static_cast<double>(clockCount_lowToHigh), //0,
        static_cast<double>(n_pulses), //2,
        static_cast<double>(0)
    };

    int error_address = INITIAL_ERR_ADDRESS;
    int error = LJM_eWriteNames(device_handle, NUM_FRAMES_CONFIGURE, aNamesConfigure,
                    aValuesConfigure, &error_address);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ConfigurePulseOut Error " << error_string;
        throw std::runtime_error(error_string);
        return;
    }
    qDebug() << "LabJack::ConfigurePulseOut ";
}

void LabJack::StartPulseOut(PULSEOUT ch) {
    StopPulseOut(ch);
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch)) + "_EF_ENABLE";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), 1);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::StartPulseOut Error " << error_string;
        throw std::runtime_error(error_string);
    }
//    qDebug() << "LabJack::StartPulseOut " << static_cast<int>(ch);
}

void LabJack::StopPulseOut(PULSEOUT ch) {
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch)) + "_EF_ENABLE";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), 0);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::StopPulseOut Error " << error_string;
        throw std::runtime_error(error_string);
    }
//    qDebug() << "LabJack::StopPulseOut " << static_cast<int>(ch);
}

int LabJack::ReadPulseOutCurrent(PULSEOUT ch) {
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch)) + "_EF_READ_A";
    double value = 0;
    int error = LJM_eReadName(device_handle, cmd.c_str(), &value);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadPulseOutCurrent Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::ReadPulseOutCurrent " << static_cast<int>(ch) << " " << value;
    return static_cast<int>(value);
}

int LabJack::ReadPulseOutTarget(PULSEOUT ch) {
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch)) + "_EF_READ_B";
    double value = 0;
    int error = LJM_eReadName(device_handle, cmd.c_str(), &value);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadPulseOutTarget Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::ReadPulseOutTarget " << static_cast<int>(ch) << " " << value;
    return static_cast<int>(value);
}

void LabJack::UpdatePulseOut(PULSEOUT ch, int pulses) {
    if (pulses < 0) {
        qWarning() << "LabJack::UpdatePulseOut Negative number of pulses";
        throw std::runtime_error("Cannot execute a negative number of pulses");
    }
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch)) + "_EF_CONFIG_C";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), static_cast<double>(pulses));
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::UpdatePulseOut Error " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::UpdatePulseOut " << static_cast<int>(ch) << " " << pulses;
}

void LabJack::DisablePulseOut(PULSEOUT ch) {
    std::string cmd = "DIO" + std::to_string(static_cast<int>(ch)) + "_EF_ENABLE";
    int error = LJM_eWriteName(device_handle, cmd.c_str(), 0);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::DisablePulseOut " << error_string;
        throw std::runtime_error(error_string);
    }
    qDebug() << "LabJack::DisablePulseOut " << static_cast<int>(ch);
}

void LabJack::ReadConfiguration() {
    int error = 0;

    char log_file[LJM_MAX_NAME_SIZE];
    error = LJM_ReadLibraryConfigStringS(LJM_DEBUG_LOG_FILE, log_file);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadConfiguration LJM_DEBUG_LOG_FILE Error " << error_string;
        throw std::runtime_error(error_string);
        return;
    }

    double log_mode = 0;
    error = LJM_ReadLibraryConfigS(LJM_DEBUG_LOG_MODE, &log_mode);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadConfiguration LJM_DEBUG_LOG_MODE Error " << error_string;
        throw std::runtime_error(error_string);
        return;
    }

    double log_level = 0;
    error = LJM_ReadLibraryConfigS(LJM_DEBUG_LOG_LEVEL, &log_level);
    if (error != 0) {
        char error_string[LJM_MAX_NAME_SIZE];
        LJM_ErrorToString(error, error_string);
        qDebug() << "LabJack::ReadConfiguration LJM_DEBUG_LOG_LEVEL Error " << error_string;
        throw std::runtime_error(error_string);
        return;
    }

    qDebug() << "LabJack::ReadConfiguration LJM_DEBUG_LOG_FILE " << QString(log_file);
    qDebug() << "LabJack::ReadConfiguration LJM_DEBUG_LOG_MODE " << log_mode;
    qDebug() << "LabJack::ReadConfiguration LJM_DEBUG_LOG_LEVEL " << log_level;
}
