import csv

def dbinsert(d_plexi, um, dose_curve, depth_vector):
    depth_dose_curve = "'{"
    for idx in range(len(depth_vector)):
        point = "\"(" + depth_vector[idx] + "," + dose_curve[idx] + ")\""
        if idx == 0:
            depth_dose_curve = depth_dose_curve + point
        else:
            depth_dose_curve = depth_dose_curve + "," + point
    depth_dose_curve = depth_dose_curve + "}'"
    return 'INSERT INTO MCNPXDoseCurve(um_plexi, monitor_units, depth_dose_array) VALUES(' + \
           str(int(1000 * float(d_plexi))) + ', ' + um + ', ' + depth_dose_curve + ');'

#with open('../files/mcnpx/mcnpx_depthdosecurves.csv') as csvfile:
with open('../files/mcnpx/undegraded_braggpeak.csv') as csvfile:
    datareader = csv.reader(csvfile, delimiter=',', quotechar='|')
    d_plexi_line = next(datareader)
    d_plexi_array = d_plexi_line[2:]
    for row in datareader:
        print(dbinsert(row[0], row[1], row[2:], d_plexi_array))
