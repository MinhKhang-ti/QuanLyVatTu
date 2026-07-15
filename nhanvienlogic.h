#ifndef NHANVIENLOGIC_H
#define NHANVIENLOGIC_H

#include "cautrucdulieu.h"
using namespace std;
#include <string>
using std::string;


void khoiTaoDSNV(DS_NHANVIEN& ds);
int timViTriNV(const DS_NHANVIEN& ds, const char* manv);
int timViTriChen(const DS_NHANVIEN &ds, const char *ten , const char *ho);
bool themNV(DS_NHANVIEN& ds, const char* manv, const char* ho, const char* ten,
            const char* phai, string& loi);
bool suaNV(DS_NHANVIEN& ds, const char* manv, const char* hoMoi, const char* tenMoi,
           const char* phaiMoi, string& loi);
bool xoaNV(DS_NHANVIEN& ds, const char* manv, string& loi);
void huyDSNV(DS_NHANVIEN& ds);

#endif // NHANVIENLOGIC_H
