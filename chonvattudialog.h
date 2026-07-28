#ifndef CHONVATTUDIALOG_H
#define CHONVATTUDIALOG_H

#include <QDialog>
#include "cautrucdulieu.h"

namespace Ui {
class ChonVatTuDialog;
}

class ChonVatTuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChonVatTuDialog(TreeVT root, QWidget *parent = nullptr);
    ~ChonVatTuDialog();

    VATTU getSelectedVatTu() const;
    QString getSelectedMaVT() const;

private slots:
    void onSearchTextChanged(const QString &text);
    void onTableDoubleClicked(int row, int column);
    void onChonClicked();
    void onHuyClicked();

private:
    Ui::ChonVatTuDialog *ui;
    TreeVT rootTree;
    VATTU selectedVT;

    void napDanhSachVatTu(const QString &filter = "");
    void chonHangCurrent();
};

#endif // CHONVATTUDIALOG_H
