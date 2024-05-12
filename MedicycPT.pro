QMAKE_CXXFLAGS += -Wno-deprecated-copy

TEMPLATE = subdirs

SUBDIRS = \
       External \
       DataStructures \
       Graphs \
       Printing \
       Repository \
       Communications \
       Util \
       plugins \
       EyeMonitoring \
       RadiationMonitor \
       MillingMachine \
       TreatmentPlanning \
       TreatmentDelivery \
       QualityAssurance

External.depends = Util
DataStructures.depends = External
Graphs.depends = DataStructures
Communications.depends = DataStructures
Printing.depends = Graphs Repository
Repository.depends = DataStructures
TreatmentPlanning.depends = External DataStructures Repository Printing Graphs Util Communications MillingMachine
TreatmentDelivery.depends = External DataStructures Repository Graphs Util RadiationMonitor Communications plugins
QualityAssurance.depends = External DataStructures Repository Printing Util RadiationMonitor
RadiationMonitor.depends = External DataStructures Repository Util Graphs
MillingMachine.depends = External DataStructures Util Repository
EyeMonitoring.depends = DataStructures External Repository Util RadiationMonitor

launch_scripts.path = $$OUT_PWD/LaunchScripts/
launch_scripts.files = LaunchScripts/*
icons.path = $$OUT_PWD/Icons
icons.files = Icons/*
desktops.path = $$OUT_PWD/Desktops
desktops.files = Desktops/*
config.path = $$OUT_PWD/Config
config.files = Config/*

INSTALLS += launch_scripts \
            icons \
            desktops \
            config











