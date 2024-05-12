
with open('MODU_92.DAT') as f1:
    lines = f1.readlines()
    for line in lines:
        splitted = line.split()
        mod100 = splitted[0]
        mod98 = splitted[1]
        parcours = splitted[2]
        numero = splitted[3]
        print 'INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, available) VALUES(' + numero + ',' + mod100 + ',' + mod98 + ',' + parcours + ', true' + ');'

