#ifndef THONGKELOGIC_H
#define THONGKELOGIC_H

#include "cautrucdulieu.h"
#include <vector>

// So sanh ngay: <0 neu a truoc b, 0 neu bang, >0 neu a sau b
int soSanhNgay(const Date& a, const Date& b);
bool ngayTrongKhoang(const Date& ngay, const Date& tu, const Date& den);

// TODO (buoi sau): cau truc + ham cho chuc nang (g) thong ke hoa don theo thoi gian
// TODO (buoi sau): cau truc + ham cho chuc nang (h) top 10 vat tu doanh thu cao nhat

#endif // THONGKELOGIC_H