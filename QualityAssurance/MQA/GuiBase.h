#ifndef QUALITYASSURANCE_GUIBASE_H
#define QUALITYASSURANCE_GUIBASE_H

#include <QWidget>
#include <QMessageBox>
#include <QSettings>
#include <QTableWidgetItem>

class MQA;

class AlignedQTableWidgetItem : public QTableWidgetItem {
public:
    AlignedQTableWidgetItem(QString text, Qt::Alignment alignment)
        : QTableWidgetItem(text) {
        this->setTextAlignment(alignment);
    }
};

class GuiBase : public QObject
{
    Q_OBJECT

public:
    GuiBase(MQA* parent);
    ~GuiBase();

public slots:
    virtual void Configure() = 0;
    virtual void CleanUp() = 0;

signals:

protected slots:    
    void DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon);
    void DisplayInfo(QString msg);
    void DisplayWarning(QString msg);
    void DisplayError(QString msg);
    void DisplayCritical(QString msg);

protected:
    QString ButtonStyleSheet(QString image);
    QVariant GetSetting(QString variable) const;

    MQA* parent_ = nullptr;
    bool message_active_ = false;
    QSettings settings_;
};

#endif
