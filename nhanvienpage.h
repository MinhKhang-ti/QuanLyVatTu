#ifndef NHANVIENPAGE_H
#define NHANVIENPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;

class NhanVienPage : public QWidget
{
    Q_OBJECT
public:
    explicit NhanVienPage(DS_NHANVIEN &dsRef, QWidget *parent = nullptr);

private slots:
    void validateForm();
    void onThemClicked();
    void onXoaClicked();

private:
    DS_NHANVIEN &dsnv; // tham chieu toi mang dung chung

    QLineEdit *manvEdit;
    QLineEdit *hoEdit;
    QLineEdit *tenEdit;
    QComboBox *phaiCombo;
    QLabel *errorLabel;
    QPushButton *themButton;
    QPushButton *xoaButton;
    QTableWidget *table;

    void lamMoiBang();
};

#endif // NHANVIENPAGE_H