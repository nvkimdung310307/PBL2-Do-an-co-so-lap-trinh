/*
  UU TIEN 4 + 5: DONG BO TEN STORED PROCEDURE VA CHUAN HOA THONG KE.
  Chay sau cac script 01-12 (dac biet sau 04, 07 va 08).
  Khong xoa bang hay du lieu. Cac procedure cu dat ban khong duoc code C++ goi
  se duoc loai bo de tranh goi nham logic cu.
*/
USE RestaurantManagement;
GO

/* C++ hien dung cac procedure dbo.sp_DatBan_* trong 07_priority1_datban.sql.
   Khong con service nao goi cac procedure dat ban cu nay. */
IF OBJECT_ID(N'dbo.sp_TaoDatBan', N'P') IS NOT NULL DROP PROCEDURE dbo.sp_TaoDatBan;
GO
IF OBJECT_ID(N'dbo.sp_HuyDatBan', N'P') IS NOT NULL DROP PROCEDURE dbo.sp_HuyDatBan;
GO
IF OBJECT_ID(N'dbo.sp_KiemTraDatBan', N'P') IS NOT NULL DROP PROCEDURE dbo.sp_KiemTraDatBan;
GO
IF OBJECT_ID(N'dbo.sp_GetDanhSachDatBan', N'P') IS NOT NULL DROP PROCEDURE dbo.sp_GetDanhSachDatBan;
GO

/* Doanh thu la gia tri hang da ban sau giam gia, khong phai chi so tien thu them luc thanh toan.
   Tien coc la khoan tra truoc, nen khong duoc lam doanh thu bi thieu khi tru khoi ThanhTien. */
CREATE OR ALTER PROCEDURE dbo.sp_DoanhThuTheoNgay
    @Ngay DATE
AS
BEGIN
    SET NOCOUNT ON;
    IF @Ngay IS NULL THROW 51301, N'Ngày thống kê không được để trống.', 1;

    SELECT @Ngay AS Ngay,
           COUNT_BIG(h.MaHD) AS SoHoaDon,
           COALESCE(SUM(t.TongTien - COALESCE(h.TienGiam,0)),0) AS DoanhThu
    FROM dbo.HoaDon h
    OUTER APPLY (
        SELECT COALESCE(SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)),0) AS TongTien
        FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD
    ) t
    WHERE h.NgayLap >= @Ngay
      AND h.NgayLap < DATEADD(DAY,1,@Ngay)
      AND h.TrangThai=N'Đã thanh toán';
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DoanhThuTheoThang
    @Nam INT,
    @Thang INT
AS
BEGIN
    SET NOCOUNT ON;
    IF @Nam IS NULL OR @Nam < 1900 OR @Nam > 9999
        THROW 51302, N'Năm thống kê không hợp lệ.', 1;
    IF @Thang IS NULL OR @Thang < 1 OR @Thang > 12
        THROW 51303, N'Tháng thống kê phải từ 1 đến 12.', 1;

    DECLARE @TuNgay DATE = DATEFROMPARTS(@Nam,@Thang,1);
    DECLARE @DenNgay DATE = DATEADD(MONTH,1,@TuNgay);

    SELECT @Nam AS Nam, @Thang AS Thang,
           COUNT_BIG(h.MaHD) AS SoHoaDon,
           COALESCE(SUM(t.TongTien - COALESCE(h.TienGiam,0)),0) AS DoanhThu
    FROM dbo.HoaDon h
    OUTER APPLY (
        SELECT COALESCE(SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)),0) AS TongTien
        FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD
    ) t
    WHERE h.NgayLap >= @TuNgay
      AND h.NgayLap < @DenNgay
      AND h.TrangThai=N'Đã thanh toán';
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_TopMonAnBanChay
    @SoLuong INT = 5
AS
BEGIN
    SET NOCOUNT ON;
    IF @SoLuong IS NULL OR @SoLuong <= 0
        THROW 51304, N'Số lượng món cần thống kê phải lớn hơn 0.', 1;

    SELECT TOP (@SoLuong)
        m.MaMon, m.TenMon,
        SUM(ct.SoLuong) AS SoLuongBan,
        SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)) AS DoanhThu
    FROM dbo.ChiTietHoaDon ct
    INNER JOIN dbo.HoaDon h ON h.MaHD=ct.MaHD
    INNER JOIN dbo.MonAn m ON m.MaMon=ct.MaMon
    WHERE h.TrangThai=N'Đã thanh toán'
    GROUP BY m.MaMon,m.TenMon
    ORDER BY SUM(ct.SoLuong) DESC,m.TenMon ASC;
END;
GO

/* Chi tinh ban da/duoc dat va da su dung; loai lich huy va khach khong den. */
CREATE OR ALTER PROCEDURE dbo.sp_TopBanDuocDat
    @SoLuong INT = 5
AS
BEGIN
    SET NOCOUNT ON;
    IF @SoLuong IS NULL OR @SoLuong <= 0
        THROW 51305, N'Số lượng bàn cần thống kê phải lớn hơn 0.', 1;

    SELECT TOP (@SoLuong)
        b.MaBan,b.TenBan,
        COUNT(db.MaDatBan) AS SoLanDat
    FROM dbo.Ban b
    LEFT JOIN dbo.DatBan db
      ON db.MaBan=b.MaBan
     AND db.TrangThai IN (N'Đã đặt',N'Đã nhận bàn',N'Hoàn thành',N'Da dat',N'Da nhan ban',N'Hoan thanh')
    GROUP BY b.MaBan,b.TenBan
    ORDER BY COUNT(db.MaDatBan) DESC,b.MaBan ASC;
END;
GO

PRINT N'Da dong bo procedure dat ban cu/moi va cap nhat cac procedure thong ke.';
GO
