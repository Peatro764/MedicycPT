
with open('DEGRA_92.DAT') as f1:
    lines = f1.readlines()
    for idx, line in enumerate(lines, 0):
        splitted = line.split()
        um_plexi = splitted[0]
        id = str(int(1000*float(um_plexi)))
        print 'INSERT INTO Degradeur(degradeur_id, um_plexi, available) VALUES(' + id + ',' + id + ',true);'


