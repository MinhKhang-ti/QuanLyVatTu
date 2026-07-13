#ifndef VATTUPAGE_H
#define VATTUPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

class QLineEdit;
class QLabel;
class QPushButton;
class QTableWidget;

class VatTuPage : public QWidget
{
    Q_OBJECT
public:
    explicit VatTuPage(TreeVT &rootRef, QWidget *parent = nullptr);

private slots:
    void validateForm();
    void onThemClicked();
    void onXoaClicked();

private:
    TreeVT &root; // tham chieu toi cay dung chung, khong so huu rieng nua

    QLineEdit *maVTEdit;
    QLineEdit *tenVTEdit;
    QLineEdit *dvtEdit;
    QLineEdit *soLuongEdit;
    QLabel *errorLabel;
    QPushButton *themButton;
    QPushButton *xoaButton;
    QTableWidget *table;

    void lamMoiBang();
};

#endif // VATTUPAGE_H