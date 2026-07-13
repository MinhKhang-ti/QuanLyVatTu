#include "fileio.h"
// TODO (code sau): implement doc/ghi file cho VatTu va NhanVien
// - luuVatTu: duyet cay (recursion bat ky thu tu nao), ghi moi node 1 dong,
//   dinh dang: MAVT|TENVT|DVT|SoLuongTon
// - docVatTu: doc tung dong, tach chuoi theo '|', goi themVT() de nap lai vao cay
// - luuNhanVien: ghi moi nhan vien 1 dong (MANV|HO|TEN|PHAI), sau do ghi tiep
//   cac dong con cho tung hoa don trong dshd cua nhan vien do, roi tiep tuc
//   cac dong con cho tung chi tiet hoa don trong dscthd cua hoa don do
//   (giong cach lam ben ban console truoc day)
// - docNhanVien: doc nguoc lai theo dung thu tu da ghi