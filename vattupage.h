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

private:
    Ui::VatTuPage *ui;
    TreeVT &root; // tham chieu toi cay dung chung, khong so huu rieng nua

    void lamMoiBang();
};

#endif // VATTUPAGE_H