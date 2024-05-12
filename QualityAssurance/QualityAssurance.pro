TEMPLATE = subdirs
SUBDIRS = MQA StateMachines DataAcquisition

StateMachines.depends = DataAcquisition
MQA.depends = StateMachines DataAcquisition





  
