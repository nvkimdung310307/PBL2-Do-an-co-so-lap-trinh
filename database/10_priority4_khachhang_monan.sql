/*
    UU TIEN 4 - KIEM TRA DU LIEU KHACH HANG VA MON AN
    Script chi doc du lieu, khong xoa/sua du lieu hien co.
    Chay trong database RestaurantManagement sau khi da tao schema.
*/
USE RestaurantManagement;
GO
SET NOCOUNT ON;

PRINT N'1. So luong khach hang';
SELECT COUNT_BIG(*) AS SoLuongKhachHang FROM dbo.KhachHang;
GO

PRINT N'2. Kiem tra so dien thoai bi trung (ket qua mong doi: 0 dong)';
SELECT SoDienThoai, COUNT_BIG(*) AS SoLan
FROM dbo.KhachHang
GROUP BY SoDienThoai
HAVING COUNT_BIG(*) > 1;
GO

PRINT N'3. So luong mon an theo trang thai';
SELECT TrangThai, COUNT_BIG(*) AS SoLuong
FROM dbo.MonAn
GROUP BY TrangThai
ORDER BY TrangThai;
GO

PRINT N'4. Mon an co gia am (ket qua mong doi: 0 dong)';
SELECT MaMon, TenMon, DonGia
FROM dbo.MonAn
WHERE DonGia < 0;
GO

PRINT N'5. Mon an co trang thai khong hop le (ket qua mong doi: 0 dong)';
SELECT MaMon, TenMon, TrangThai
FROM dbo.MonAn
WHERE TrangThai NOT IN (N'Dang ban', N'Đang bán', N'Hết món', N'Ngừng bán');
GO

PRINT N'6. Khach hang duoc tham chieu boi dat ban';
SELECT COUNT_BIG(DISTINCT db.MaKH) AS SoKhachHangCoDatBan
FROM dbo.DatBan AS db
WHERE db.MaKH IS NOT NULL;
GO
