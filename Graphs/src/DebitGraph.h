#ifndef DEBITGRAPH_H
#define DEBITGRAPH_H

#include "Debit.h"
#include "qcustomplot.h"

#include <vector>

class DebitGraph : public QObject
{
    Q_OBJECT

public:
    DebitGraph(QCustomPlot* customplot);
    ~DebitGraph();
    void Register(QString name);
    void SetData(QString name, const std::vector<Debit>& debit);
    void AddPoint(QString name, const Debit& debit);
    void RemoveAll();
    void Clear();
    void Clear(QString name);

private slots:
    void mousePress();
    void mouseWheel();

private:
    bool Exists(QString name) const;
    void GenerateColors();
    QColor GetColor() const;
    void Plot();
    QCustomPlot* customplot_;
    std::map<QString, QCPGraph*> debits_graph_;
    std::vector<QColor> colors_;
};

#endif //DEBITGRAPH_H

