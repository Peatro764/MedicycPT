from unittest import TestCase
import CreateModulateurSQLCode as creator

class TestCreateWeightedThicknessVector(TestCase):
    def test_modStartingAtZeroThickness(self):
        sql_code = creator.createWeightedThicknessVector("./files/StartingAtZero.dat")
        self.assertEqual("'{\"(0,1)\",\"(800,0.421)\",\"(1600,0.339)\"}'", sql_code)

    def test_modSingleStep(self):
        sql_code = creator.createWeightedThicknessVector("./files/SingleStep.dat")
        self.assertEqual("'{\"(0,1)\"}'", sql_code)

    def test_trailingZerosAreOmitted(self):
        sql_code = creator.createWeightedThicknessVector("./files/TrailingZeros.dat")
        self.assertEqual("'{\"(0,1)\",\"(800,0.421)\",\"(1600,0.339)\"}'", sql_code)

class TestGetModulatorFiles(TestCase):
    def test_nonEmptyDirectory(self):
        hej = creator.getModulatorFiles("./files/")
        self.assertEqual(["./files/StartingAtZero.dat","./files/TrailingZeros.dat","./files/SingleStep.dat"],
                         creator.getModulatorFiles("./files/"))


class TestGetModulatorId(TestCase):
    def test_validFileName(self):
        self.assertEqual("110", creator.getModulatorId("110.dat"))
    def test_anotherDir(self):
        self.assertEqual("110", creator.getModulatorId("mypath/110.dat"))
    def test_sameDir(self):
        self.assertEqual("110", creator.getModulatorId("./110.dat"))