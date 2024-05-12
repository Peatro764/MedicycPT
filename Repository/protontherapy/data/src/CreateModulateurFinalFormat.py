def CreateThicknessWeightPoint(weight, index):
    thickness = index * 800
    return '\"(' + str(thickness) + ',' + weight + ')\"'


def CreateWeightVectorSQLString(weight_vector):
    wt_vector = "\'{"
    for idx, weight in enumerate(weight_vector):
        if wt_vector == '\'{':
            wt_vector = wt_vector + CreateThicknessWeightPoint(weight, idx)
        else:
            wt_vector = wt_vector + ',' + CreateThicknessWeightPoint(weight, idx)
    wt_vector = wt_vector + '}\''
    return wt_vector


def CreateInsertStatement(id, mod100, mod98, parcours, degradeur, weight_vector):
    db_part1 = 'INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available, opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria)'
    db_part2 = 'VALUES(' + id + ', ' + mod100 + ', ' + mod98 + ', ' + parcours + ', ' + degradeur + ', ' + \
               CreateWeightVectorSQLString(weight_vector) + ', true, \'Joel99\', \'Shifted bragg\', 4, 0.8, 0.0, 0.0, 0.4, 0.0001);'
    return (db_part1 + ' ' + db_part2)


def loopOverRows(filename) :
    with open(filename) as f:
        lines = f.readlines()
        for line in lines:
            (id, mod100, mod98, parcours, degradeur, weights) = line.split('\t')
            weight_vector = weights.split(',')
            insert_statement = CreateInsertStatement(id, mod100, mod98, parcours, degradeur, weight_vector)
            print(insert_statement)



loopOverRows("../files/modulateurs/modulateurs.csv")