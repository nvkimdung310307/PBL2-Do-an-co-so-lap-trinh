/* Uu tien 3 - tai khoan / dang nhap
   Ung dung moi xac minh PBKDF2 trong DangNhapService.
   Khong tiep tuc cho phep procedure so sanh mat khau plaintext.
   Chay tren database RestaurantManagement sau khi sao luu database. */
USE RestaurantManagement;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DangNhap
    @TenDangNhap VARCHAR(50),
    @MatKhau VARCHAR(255)
AS
BEGIN
    SET NOCOUNT ON;
    THROW 51001,
        'sp_DangNhap cu khong duoc dung nua. Hay dang nhap qua DangNhapService de xac minh PBKDF2.',
        1;
END;
GO

/* Kiem tra tai khoan / vai tro / trang thai khong hop le. Chi doc du lieu. */
SELECT MaTK, TenDangNhap, VaiTro, TrangThai, MaNV
FROM dbo.TaiKhoan
WHERE VaiTro NOT IN (N'Admin', N'Quản lý', N'Nhân viên')
   OR TrangThai NOT IN (N'Hoạt động', N'Bị khóa');
GO

/* Xem tai khoan van dang luu plaintext (se duoc nang cap sau lan dang nhap thanh cong).
   Khong hien thi gia tri MatKhau de tranh lo thong tin dang nhap. */
SELECT MaTK, TenDangNhap
FROM dbo.TaiKhoan
WHERE MatKhau NOT LIKE 'pbkdf2-sha256$%';
GO
