#include "TestBoundary.h"

void TestBoundary::Comparison() {
    std::vector<std::pair<int, int>> coordinates1;
    coordinates1.push_back(std::make_pair(3,10));
    coordinates1.push_back(std::make_pair(2,11));
    Boundary boundary1(coordinates1);

    std::vector<std::pair<int, int>> coordinates2;
    coordinates2.push_back(std::make_pair(3,10));
    coordinates2.push_back(std::make_pair(4,11));
    Boundary boundary2(coordinates2);

    QCOMPARE(boundary1, boundary1);
    QVERIFY(boundary1 != boundary2);

}

void TestBoundary::CropY() {
    QCOMPARE(Boundary().CropY(0, true), Boundary());

    std::vector<std::pair<int, int>> coordinates;
    coordinates.push_back(std::make_pair(3,10));
    coordinates.push_back(std::make_pair(2,11));
    coordinates.push_back(std::make_pair(1,12));
    coordinates.push_back(std::make_pair(4,13));
    coordinates.push_back(std::make_pair(5,14));
    Boundary boundary(coordinates);

    Boundary cropped1(boundary.CropY(12, true));
    std::vector<std::pair<int, int>> exp_coord1;
    exp_coord1.push_back(std::make_pair(1,12));
    exp_coord1.push_back(std::make_pair(4,13));
    exp_coord1.push_back(std::make_pair(5,14));
    Boundary exp_boundary1(exp_coord1);
    QCOMPARE(cropped1, exp_boundary1);

    Boundary cropped2(boundary.CropY(10, true));
    QCOMPARE(cropped2, boundary);

    Boundary cropped3(boundary.CropY(15, true));
    Boundary exp_boundary3;
    QCOMPARE(cropped3, exp_boundary3);

    Boundary cropped4(boundary.CropY(12, false));
    std::vector<std::pair<int, int>> exp_coord4;
    exp_coord4.push_back(std::make_pair(3,10));
    exp_coord4.push_back(std::make_pair(2,11));
    exp_coord4.push_back(std::make_pair(1,12));
    Boundary exp_boundary4(exp_coord4);
    QCOMPARE(cropped4, exp_boundary4);
}

void TestBoundary::CropX() {
    QCOMPARE(Boundary().CropX(0, true), Boundary());

    std::vector<std::pair<int, int>> coordinates;
    coordinates.push_back(std::make_pair(3,10));
    coordinates.push_back(std::make_pair(2,11));
    coordinates.push_back(std::make_pair(1,12));
    coordinates.push_back(std::make_pair(4,13));
    coordinates.push_back(std::make_pair(5,14));
    Boundary boundary(coordinates);

    Boundary cropped1(boundary.CropX(12, true));
    QCOMPARE(cropped1, boundary);

    Boundary emptyBoundary;
    Boundary cropped2(boundary.CropX(12, false));
    QCOMPARE(cropped2, emptyBoundary);

    Boundary cropped3(boundary.CropX(3, true));
    std::vector<std::pair<int, int>> exp_coord3;
    exp_coord3.push_back(std::make_pair(3,10));
    exp_coord3.push_back(std::make_pair(2,11));
    exp_coord3.push_back(std::make_pair(1,12));
    QCOMPARE(cropped3, Boundary(exp_coord3));

    Boundary cropped4(boundary.CropX(3, false));
    std::vector<std::pair<int, int>> exp_coord4;
    exp_coord4.push_back(std::make_pair(3,10));
    exp_coord4.push_back(std::make_pair(4,13));
    exp_coord4.push_back(std::make_pair(5,14));
    QCOMPARE(cropped4, Boundary(exp_coord4));
}

void TestBoundary::InvertY() {
    Boundary boundary;
    boundary.InvertY();
    QCOMPARE(boundary, Boundary());

    std::vector<std::pair<int, int>> coord1;
    coord1.push_back(std::make_pair(3,10));
    coord1.push_back(std::make_pair(2,-11));
    Boundary boundary1(coord1);

    boundary1.InvertY();

    std::vector<std::pair<int, int>> exp_coord1;
    exp_coord1.push_back(std::make_pair(3,-10));
    exp_coord1.push_back(std::make_pair(2,11));
    Boundary exp_boundary1(exp_coord1);

    QCOMPARE(boundary1, exp_boundary1);
}

void TestBoundary::ScaleY() {
    Boundary boundary;
    boundary.ScaleY(1);
    QCOMPARE(boundary, Boundary());

    std::vector<std::pair<int, int>> coord1;
    coord1.push_back(std::make_pair(3,10));
    coord1.push_back(std::make_pair(2,-11));
    Boundary boundary1(coord1);
    boundary1.ScaleY(2);

    std::vector<std::pair<int, int>> exp_coord1;
    exp_coord1.push_back(std::make_pair(3,20));
    exp_coord1.push_back(std::make_pair(2,-22));
    Boundary exp_boundary1(exp_coord1);

    QCOMPARE(boundary1, exp_boundary1);

    Boundary boundary2(coord1);
    boundary2.ScaleY(2.5);

    std::vector<std::pair<int, int>> exp_coord2;
    exp_coord2.push_back(std::make_pair(3,25));
    exp_coord2.push_back(std::make_pair(2,-27));
    Boundary exp_boundary2(exp_coord2);

    QCOMPARE(boundary2, exp_boundary2);
}

void TestBoundary::MinY() {    
    std::vector<std::pair<int, int>> coord1;
    coord1.push_back(std::make_pair(3,10));
    coord1.push_back(std::make_pair(2,-11));
    Boundary boundary1(coord1);
    QCOMPARE(boundary1.MinY(), -11);

    std::vector<std::pair<int, int>> coord2;
    coord2.push_back(std::make_pair(3,-10));
    coord2.push_back(std::make_pair(2,11));
    Boundary boundary2(coord2);
    QCOMPARE(boundary2.MinY(), -10);
}

void TestBoundary::MaxY() {
    std::vector<std::pair<int, int>> coord1;
    coord1.push_back(std::make_pair(3,10));
    coord1.push_back(std::make_pair(2,-11));
    Boundary boundary1(coord1);
    QCOMPARE(boundary1.MaxY(), 10);

    std::vector<std::pair<int, int>> coord2;
    coord2.push_back(std::make_pair(3,-10));
    coord2.push_back(std::make_pair(2,11));
    Boundary boundary2(coord2);
    QCOMPARE(boundary2.MaxY(), 11);
}

void TestBoundary::SetMinY() {
    Boundary boundary;
    boundary.SetMinY(0);
    QCOMPARE(boundary, Boundary());

    std::vector<std::pair<int, int>> coord1;
    coord1.push_back(std::make_pair(3,10));
    coord1.push_back(std::make_pair(2,-11));
    Boundary boundary1(coord1);
    boundary1.SetMinY(5);

    std::vector<std::pair<int, int>> exp_coord1;
    exp_coord1.push_back(std::make_pair(3,26));
    exp_coord1.push_back(std::make_pair(2,5));
    Boundary exp_boundary1(exp_coord1);
    QCOMPARE(boundary1, exp_boundary1);

    std::vector<std::pair<int, int>> coord2;
    coord2.push_back(std::make_pair(3,10));
    coord2.push_back(std::make_pair(2,-11));
    Boundary boundary2(coord2);
    boundary2.SetMinY(-5);

    std::vector<std::pair<int, int>> exp_coord2;
    exp_coord2.push_back(std::make_pair(3,16));
    exp_coord2.push_back(std::make_pair(2,-5));
    Boundary exp_boundary2(exp_coord2);
    QCOMPARE(boundary2, exp_boundary2);

    std::vector<std::pair<int, int>> coord3;
    coord3.push_back(std::make_pair(3,10));
    coord3.push_back(std::make_pair(2,-11));
    Boundary boundary3(coord3);
    boundary3.SetMinY(-15);

    std::vector<std::pair<int, int>> exp_coord3;
    exp_coord3.push_back(std::make_pair(3,6));
    exp_coord3.push_back(std::make_pair(2,-15));
    Boundary exp_boundary3(exp_coord3);
    QCOMPARE(boundary3, exp_boundary3);
}

void TestBoundary::SetMaxY() {
    Boundary boundary;
    boundary.SetMaxY(0);
    QCOMPARE(boundary, Boundary());

    std::vector<std::pair<int, int>> coord1;
    coord1.push_back(std::make_pair(3,10));
    coord1.push_back(std::make_pair(2,-11));
    Boundary boundary1(coord1);
    boundary1.SetMaxY(15);

    std::vector<std::pair<int, int>> exp_coord1;
    exp_coord1.push_back(std::make_pair(3,15));
    exp_coord1.push_back(std::make_pair(2,-6));
    Boundary exp_boundary1(exp_coord1);
    QCOMPARE(boundary1, exp_boundary1);

    std::vector<std::pair<int, int>> coord2;
    coord2.push_back(std::make_pair(3,10));
    coord2.push_back(std::make_pair(2,-11));
    Boundary boundary2(coord2);
    boundary2.SetMaxY(5);

    std::vector<std::pair<int, int>> exp_coord2;
    exp_coord2.push_back(std::make_pair(3,5));
    exp_coord2.push_back(std::make_pair(2,-16));
    Boundary exp_boundary2(exp_coord2);
    QCOMPARE(boundary2, exp_boundary2);

    std::vector<std::pair<int, int>> coord3;
    coord3.push_back(std::make_pair(3,10));
    coord3.push_back(std::make_pair(2,-11));
    Boundary boundary3(coord3);
    boundary3.SetMaxY(-15);

    std::vector<std::pair<int, int>> exp_coord3;
    exp_coord3.push_back(std::make_pair(3,-15));
    exp_coord3.push_back(std::make_pair(2,-36));
    Boundary exp_boundary3(exp_coord3);
    QCOMPARE(boundary3, exp_boundary3);
}

void TestBoundary::StepFormat() {
    std::vector<std::pair<int, int>> coordinates;
    coordinates.push_back(std::make_pair(0,12));
    coordinates.push_back(std::make_pair(2,11));
    coordinates.push_back(std::make_pair(4,10));
    coordinates.push_back(std::make_pair(7,7));
    coordinates.push_back(std::make_pair(10,6));
    coordinates.push_back(std::make_pair(12,3));
    coordinates.push_back(std::make_pair(16,0));
    Boundary boundary(coordinates);

    Boundary stepFormat(boundary.StepFormat());

    std::vector<std::pair<int, int>> exp_coord;
    exp_coord.push_back(std::make_pair(0,12));
    exp_coord.push_back(std::make_pair(1,12));
    exp_coord.push_back(std::make_pair(1,11));
    exp_coord.push_back(std::make_pair(3,11));
    exp_coord.push_back(std::make_pair(3,10));
    exp_coord.push_back(std::make_pair(5,10));
    exp_coord.push_back(std::make_pair(5,7));
    exp_coord.push_back(std::make_pair(8,7));
    exp_coord.push_back(std::make_pair(8,6));
    exp_coord.push_back(std::make_pair(11,6));
    exp_coord.push_back(std::make_pair(11,3));
    exp_coord.push_back(std::make_pair(14,3));
    exp_coord.push_back(std::make_pair(14,0));
    exp_coord.push_back(std::make_pair(16,0));
    Boundary exp_boundary(exp_coord);

    QCOMPARE(stepFormat, exp_boundary);

    try {
        Boundary small_boundary;
        small_boundary.Add(std::make_pair(1, 1));
        small_boundary.StepFormat();
        QFAIL("TestBoundary::StepFormat Test should have thrown an exception");
    }
    catch(...) {}
}

void TestBoundary::MirrorOverYAxis() {
    QCOMPARE(Boundary().MirrorOverYAxis(), Boundary());

    std::vector<std::pair<int, int>> coordinates;
    coordinates.push_back(std::make_pair(0,12));
    coordinates.push_back(std::make_pair(2,11));
    coordinates.push_back(std::make_pair(4,10));
    Boundary boundary(coordinates);

    std::vector<std::pair<int, int>> exp_coord;
    exp_coord.push_back(std::make_pair(-4,10));
    exp_coord.push_back(std::make_pair(-2,11));
    exp_coord.push_back(std::make_pair(0,12));
    Boundary exp_boundary(exp_coord);

    QCOMPARE(boundary.MirrorOverYAxis(), exp_boundary);

    std::vector<std::pair<int, int>> coordinates2;
    coordinates2.push_back(std::make_pair(0,12));
    coordinates2.push_back(std::make_pair(-2,11));
    coordinates2.push_back(std::make_pair(-4,10));
    Boundary boundary2(coordinates2);

    std::vector<std::pair<int, int>> exp_coord2;
    exp_coord2.push_back(std::make_pair(4,10));
    exp_coord2.push_back(std::make_pair(2,11));
    exp_coord2.push_back(std::make_pair(0,12));
    Boundary exp_boundary2(exp_coord2);

    QCOMPARE(boundary2.MirrorOverYAxis(), exp_boundary2);
}
