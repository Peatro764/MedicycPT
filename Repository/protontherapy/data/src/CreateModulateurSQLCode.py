import sys
from os import listdir
import os.path
from os.path import isfile, join

def createPoint(step, weight):
    thickness = (int(step) - 1) * 800
    return '\"(' + str(thickness) + ',' + weight + ')\"'

def createWeightedThicknessVector(mod_file) :
    wt_vector = '\'{'
    with open(mod_file) as f:
        lines = f.readlines()
        for line in lines:
            (step, weight) = line.split()
            if (weight != '0' and float(weight) > 0.00001) :
                if wt_vector == '\'{' :
                    wt_vector = wt_vector + createPoint(step, weight
)
                else :
                    wt_vector = wt_vector + ',' + createPoint(step,weight)
    wt_vector = wt_vector + '}\''
    return wt_vector


def getModulatorFiles(path) :
    return [join(path, f) for f in listdir(path) if isfile(join(path, f))]

def getModulatorId(path) :
    filename = os.path.basename(path)
    return filename.split(".")[0]

modulator_files = getModulatorFiles("../files/modulateurs/")
modulator_sql = []
for filepath in modulator_files :
    mod_id = getModulatorId(filepath)
    db_part1 = 'INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_plexi_weight, available)'
    db_part2 = 'VALUES(' + mod_id + ', 100.0, 98.0, 30.0,' + createWeightedThicknessVector(filepath) + ', true);'
    modulator_sql.append(db_part1 + ' ' + db_part2)

for mod_sql in modulator_sql :
    print(mod_sql)

