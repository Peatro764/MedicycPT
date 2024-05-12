#ifndef QUALITYASSURANCE_DEPTHDOSERESULTS_H
#define QUALITYASSURANCE_DEPTHDOSERESULTS_H

#include <QDateTime>

class DepthDoseResults {
public:
    DepthDoseResults()
        : timestamp_(QDateTime::currentDateTime()),
          parcours_(0.0),
          penumbra_left_(0.0),
          penumbra_right_(0.0),
          modulation100_left_(0.0),
          modulation100_right_(0.0),
          modulation98_left_(0.0),
          modulation98_right_(0.0),
          width50_left_(0.0),
          width50_right_(0.0),
          is_braggpeak_(false) {}

    DepthDoseResults(QDateTime timestamp,
                     double parcours,
                     double penumbra_left, double penumbra_right,
                     double modulation100_left, double modulation100_right,
                     double modulation98_left, double modulation98_right,
                     double width50_left, double width50_right,
                     bool is_braggpeak)
        : timestamp_(timestamp),
          parcours_(parcours),
          penumbra_left_(penumbra_left), penumbra_right_(penumbra_right),
          modulation100_left_(modulation100_left), modulation100_right_(modulation100_right),
          modulation98_left_(modulation98_left), modulation98_right_(modulation98_right),
          width50_left_(width50_left), width50_right_(width50_right),
          is_braggpeak_(is_braggpeak) {}
    QDateTime timestamp() const { return timestamp_; }
    double width50() const { return std::max(0.0, width50_right_ - width50_left_); }
    double penumbra() const { return std::max(0.0, penumbra_right_ - penumbra_left_); }
    double parcours() const { return parcours_; }
    double mod98() const { return std::max(0.0, modulation98_right_ - modulation98_left_); }
    double mod100() const { return std::max(0.0, modulation100_right_ - modulation100_left_); }
    double width50left() const { return width50_left_; }
    double width50right() const { return width50_right_; }
    double mod98left() const { return modulation98_left_; }
    double mod98right() const { return modulation98_right_; }
    double mod100left() const { return modulation100_left_; }
    double mod100right() const { return modulation100_right_; }
    double penumbraleft() const { return penumbra_left_; }
    double penumbraright() const { return penumbra_right_; }
    bool IsBraggPeak() const { return is_braggpeak_; }

private:
    QDateTime timestamp_;
    double parcours_;
    double penumbra_left_;
    double penumbra_right_;
    double modulation100_left_;
    double modulation100_right_;
    double modulation98_left_;
    double modulation98_right_;
    double width50_left_;
    double width50_right_;
    bool is_braggpeak_;
};


#endif
