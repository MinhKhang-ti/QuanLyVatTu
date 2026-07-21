#ifndef VATTUPAGE_H
#define VATTUPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class VatTuPage;
}

class VatTuPage : public QWidget
{
    Q_OBJECT
public:
    explicit VatTuPage(TreeVT &rootRef, QWidget *parent = nullptr);
    ~VatTuPage();

private slots:
    void validateForm();
    void onThemClicked();
    void onXoaClicked();
    void onTableSelectionChanged();
    void onSuaClicked();
    void resetForm();

private:
    Ui::VatTuPage *ui;
    TreeVT &root; // tham chieu toi cay dung chung, khong so huu rieng nua
    QString maVTDangSua; // rong = dang o che do them moi, khac rong = dang sua dong nay

    void lamMoiBang();
};

#endif // VATTUPAGE_H