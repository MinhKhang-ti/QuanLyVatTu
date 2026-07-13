#ifndef NHANVIENLOGIC_H
#define NHANVIENLOGIC_H

#include "cautrucdulieu.h"
#include <string>

void khoiTaoDSNV(DS_NHANVIEN& ds);
int timViTriNV(const DS_NHANVIEN& ds, const char* manv);
bool themNV(DS_NHANVIEN& ds, const char* manv, const char* ho, const char* ten,
            const char* phai, std::string& loi);
bool suaNV(DS_NHANVIEN& ds, const char* manv, const char* hoMoi, const char* tenMoi,
           const char* phaiMoi, std::string& loi);
bool xoaNV(DS_NHANVIEN& ds, const char* manv, std::string& loi);
void huyDSNV(DS_NHANVIEN& ds);

#endif // NHANVIENLOGIC_H