#include "thongkelogic.h"

int soSanhNgay(const Date& a, const Date& b) {
    if (a.nam != b.nam) return a.nam - b.nam;
    if (a.thang != b.thang) return a.thang - b.thang;
    return a.ngay - b.ngay;
}

bool ngayTrongKhoang(const Date& ngay, const Date& tu, const Date& den) {
    return soSanhNgay(ngay, tu) >= 0 && soSanhNgay(ngay, den) <= 0;
}