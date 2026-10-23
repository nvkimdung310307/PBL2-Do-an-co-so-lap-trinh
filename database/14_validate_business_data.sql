/*
  KIEM TRA TOAN VEN DU LIEU NGHIEP VU (READ-ONLY)
  Chay sau 13_priority5_stats_and_legacy_cleanup.sql.
  Script chi SELECT, khong UPDATE/INSERT/DELETE va khong sua du lieu.
  Moi result set la mot nhom can doi soat; khong co dong nao nghia la chua phat hien loi theo quy tac do.
*/
USE RestaurantManagement;
GO
SET NOCOUNT ON;

PRINT N'1. DAT BAN CO TRANG THAI KHONG HOP LE';
SELECT MaDatBan, TrangThai
FROM dbo.DatBan
WHERE TrangThai NOT IN (N'Đã đặt',N'Đã nhận bàn',N'Đã hủy',N'Không đến',N'Hoàn thành',
                        N'Da dat',N'Da nhan ban',N'Da huy',N'KhongDen',N'Hoan thanh');
GO

PRINT N'2. DAT BAN DANG HOAT DONG BI TRUNG KHOANG 120 PHUT TREN CUNG BAN';
SELECT a.MaDatBan AS MaDatBan1, b.MaDatBan AS MaDatBan2,
       a.MaBan, a.NgayDat AS Ngay1, a.GioDat AS Gio1,
       b.NgayDat AS Ngay2, b.GioDat AS Gio2
FROM dbo.DatBan a
JOIN dbo.DatBan b
  ON b.MaBan=a.MaBan AND b.MaDatBan>a.MaDatBan
 AND a.TrangThai IN (N'Đã đặt',N'Đã nhận bàn',N'Da dat',N'Da nhan ban')
 AND b.TrangThai IN (N'Đã đặt',N'Đã nhận bàn',N'Da dat',N'Da nhan ban')
CROSS APPLY (SELECT DATEADD(SECOND,DATEDIFF(SECOND,CAST('00:00:00' AS TIME),a.GioDat),CAST(a.NgayDat AS DATETIME2(0))) AS BatDau1,
                    DATEADD(SECOND,DATEDIFF(SECOND,CAST('00:00:00' AS TIME),b.GioDat),CAST(b.NgayDat AS DATETIME2(0))) AS BatDau2) t
WHERE t.BatDau1 < DATEADD(MINUTE,120,t.BatDau2)
  AND t.BatDau2 < DATEADD(MINUTE,120,t.BatDau1);
GO

PRINT N'3. TRANG THAI BAN KHONG KHOP VOI LICH DAT HOAT DONG';
SELECT b.MaBan,b.TenBan,b.TrangThai AS TrangThaiBan,
       CASE WHEN EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.TrangThai IN (N'Đã nhận bàn',N'Da nhan ban')) THEN N'Đang sử dụng'
            WHEN EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.TrangThai IN (N'Đã đặt',N'Da dat')) THEN N'Đã đặt'
            ELSE N'Trống' END AS TrangThaiDuKien
FROM dbo.Ban b
WHERE b.TrangThai NOT IN (N'Bảo trì',N'Bao tri')
  AND ((EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.TrangThai IN (N'Đã nhận bàn',N'Da nhan ban')) AND b.TrangThai NOT IN (N'Đang sử dụng',N'Dang su dung'))
    OR (NOT EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.TrangThai IN (N'Đã nhận bàn',N'Da nhan ban'))
        AND EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.TrangThai IN (N'Đã đặt',N'Da dat'))
        AND b.TrangThai NOT IN (N'Đã đặt',N'Da dat'))
    OR (NOT EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.TrangThai IN (N'Đã nhận bàn',N'Da nhan ban',N'Đã đặt',N'Da dat'))
        AND b.TrangThai NOT IN (N'Trống',N'Trong')));
GO

PRINT N'4. HOA DON CO TIEN GIAM/TIEN THANH TOAN BAT HOP LE';
SELECT h.MaHD,h.TrangThai,h.TongTien,h.TienGiam,h.ThanhTien,t.TongChiTiet
FROM dbo.HoaDon h
OUTER APPLY (SELECT COALESCE(SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)),0) AS TongChiTiet
             FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD) t
WHERE h.TienGiam<0 OR h.TienGiam>t.TongChiTiet OR h.ThanhTien<0
   OR (h.TrangThai=N'Đã thanh toán' AND NOT EXISTS (SELECT 1 FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD));
GO

PRINT N'5. HOA DON DA THANH TOAN NHUNG LICH DAT CHUA HOAN THANH';
SELECT h.MaHD,h.MaDatBan,h.TrangThai AS TrangThaiHoaDon,d.TrangThai AS TrangThaiDatBan
FROM dbo.HoaDon h
JOIN dbo.DatBan d ON d.MaDatBan=h.MaDatBan
WHERE h.TrangThai=N'Đã thanh toán' AND d.TrangThai NOT IN (N'Hoàn thành',N'Hoan thanh');
GO

PRINT N'6. TIEN COC DA TRU NHUNG KHONG CO HOA DON DA THANH TOAN';
SELECT dc.MaDatCoc,dc.MaDatBan,dc.TienCoc,dc.TrangThai
FROM dbo.DatCoc dc
WHERE dc.TrangThai='DaTru'
  AND NOT EXISTS (SELECT 1 FROM dbo.HoaDon h WHERE h.MaDatBan=dc.MaDatBan AND h.TrangThai=N'Đã thanh toán');
GO

PRINT N'7. LICH DA HUY/KHONG DEN NHUNG VAN CON TIEN COC CHUA XU LY';
SELECT dc.MaDatCoc,dc.MaDatBan,dc.TienCoc,dc.TrangThai,d.TrangThai AS TrangThaiDatBan
FROM dbo.DatCoc dc
JOIN dbo.DatBan d ON d.MaDatBan=dc.MaDatBan
WHERE dc.TrangThai='DaCoc' AND d.TrangThai IN (N'Đã hủy',N'Không đến');
GO

PRINT N'8. CAC THONG KE DOANH THU HIEN CO (DOI CHIEU THU CONG)';
SELECT CONVERT(date,h.NgayLap) AS Ngay,
       COUNT_BIG(h.MaHD) AS SoHoaDonDaThanhToan,
       SUM(COALESCE(t.TongChiTiet,0)-COALESCE(h.TienGiam,0)) AS DoanhThuSauGiam
FROM dbo.HoaDon h
OUTER APPLY (SELECT SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)) AS TongChiTiet
             FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD) t
WHERE h.TrangThai=N'Đã thanh toán'
GROUP BY CONVERT(date,h.NgayLap)
ORDER BY Ngay DESC;
GO
