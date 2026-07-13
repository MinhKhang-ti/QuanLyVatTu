#ifndef HOADONLOGIC_H
#define HOADONLOGIC_H

#include "cautrucdulieu.h"
#include <string>

nodeHD* taoHoaDon(const char* soHD, Date ngay, char loai);
void themHoaDonVaoDS(PTRHD& dshd, nodeHD* hdMoi);
nodeHD* timHoaDon(PTRHD dshd, const char* soHD);

bool themCTHD(DS_CTHD& ds, const char* mavt, int soLuong, float donGia, float vat, std::string& loi);
bool xoaCTHD(DS_CTHD& ds, const char* mavt, std::string& loi);

double tinhTriGiaDong(const CT_HOADON& ct);
double tinhTongTriGiaHD(const DS_CTHD& ds);

void huyDSHoaDon(PTRHD& dshd);

#endif // HOADONLOGIC_H