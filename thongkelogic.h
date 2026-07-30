#ifndef THONGKELOGIC_H
#define THONGKELOGIC_H

#include "cautrucdulieu.h"
#include <string>
#include <vector>

// So sanh ngay: <0 neu a truoc b, 0 neu bang, >0 neu a sau b
int soSanhNgay(const Date& a, const Date& b);
bool ngayTrongKhoang(const Date& ngay, const Date& tu, const Date& den);

// TODO (buoi sau): cau truc + ham cho chuc nang (g) thong ke hoa don theo thoi gian
// TODO (buoi sau): cau truc + ham cho chuc nang (h) top 10 vat tu doanh thu cao nhat
struct DongThongKeHoaDon {
    char soHD[21];
    Date ngayLap;
    char loai;
    char maNV[11];
    std::string hoTenNV;
    double triGia;
};

struct DongTopVatTuDoanhThu {
    char maVT[11];
    std::string tenVT;
    int soLuong;
    double doanhThu;
};

std::vector<DongThongKeHoaDon> thongKeHoaDonTheoThoiGian(const DS_NHANVIEN& dsnv,
                                                          const Date& tuNgay,
                                                          const Date& denNgay);

std::vector<DongTopVatTuDoanhThu> topVatTuDoanhThu(TreeVT root,
                                                    const DS_NHANVIEN& dsnv,
                                                    const Date& tuNgay,
                                                    const Date& denNgay,
                                                    int gioiHan = 10);

#endif // THONGKELOGIC_H
