/*
  KIEM TRA SCHEMA SAU KHI CHAY CAC SCRIPT DATABASE THEO THU TU README.
  Script chi doc metadata, khong sua/xoa du lieu.
*/
USE RestaurantManagement;
GO
SET NOCOUNT ON;

PRINT N'=== KIEM TRA CAC BANG NGHIEP VU BAT BUOC ===';
WITH RequiredTables AS (
    SELECT TableName FROM (VALUES
        (N'KhuVuc'), (N'Ban'), (N'KhachHang'), (N'NhanVien'),
        (N'TaiKhoan'), (N'DatBan'), (N'MonAn'), (N'KhuyenMai'),
        (N'HoaDon'), (N'ChiTietHoaDon'), (N'DatCoc'), (N'KhachCho')
    ) AS T(TableName)
)
SELECT R.TableName AS BangThieu
FROM RequiredTables R
LEFT JOIN INFORMATION_SCHEMA.TABLES I
    ON I.TABLE_SCHEMA = N'dbo' AND I.TABLE_NAME = R.TableName AND I.TABLE_TYPE = N'BASE TABLE'
WHERE I.TABLE_NAME IS NULL;
IF @@ROWCOUNT = 0 PRINT N'Tat ca bang bat buoc deu ton tai.';
GO

PRINT N'=== KIEM TRA CAC STORED PROCEDURE NGHIEP VU BAT BUOC ===';
WITH RequiredProcedures AS (
    SELECT ProcedureName FROM (VALUES
        (N'sp_DangNhap'),
        (N'sp_GetDanhSachBan'), (N'sp_TimBanPhuHop'), (N'sp_TimKhachHang'),
        -- Hai thu tuc cu da duoc loai bo trong script 13; source dung sp_DatBan_* thay the.
        (N'sp_DatBan_Tao'), (N'sp_DatBan_Sua'), (N'sp_DatBan_Xoa'),
        (N'sp_DatBan_Huy'), (N'sp_DatBan_XacNhanDen'), (N'sp_DatBan_KhongDen'),
        (N'sp_DatBan_HoanThanh'), (N'sp_DatBan_ChuyenBan'),
        (N'sp_TinhTongHoaDon'), (N'sp_ThanhToanHoaDon'), (N'sp_LamMoiTienHoaDonTheoDatBan'),
        (N'sp_DoanhThuTheoNgay'), (N'sp_DoanhThuTheoThang'),
        (N'sp_TopMonAnBanChay'), (N'sp_TopBanDuocDat'),
        (N'sp_ThemDatCoc'), (N'sp_HoanDatCoc'), (N'sp_CapNhatTienCoc'),
        (N'sp_DanhSachDatCoc'), (N'sp_ThemKhachCho'),
        (N'sp_DanhSachKhachCho'), (N'sp_GoiKhachCho'), (N'sp_HuyKhachCho'),
        (N'sp_TachNhomBan')
    ) AS P(ProcedureName)
)
SELECT R.ProcedureName AS ThuTucThieu
FROM RequiredProcedures R
LEFT JOIN sys.procedures P ON P.name = R.ProcedureName AND SCHEMA_NAME(P.schema_id) = N'dbo'
WHERE P.object_id IS NULL;
IF @@ROWCOUNT = 0 PRINT N'Tat ca stored procedure bat buoc deu ton tai.';
GO

PRINT N'=== CAC BANG HIEN CO ===';
SELECT TABLE_SCHEMA, TABLE_NAME
FROM INFORMATION_SCHEMA.TABLES
WHERE TABLE_TYPE = N'BASE TABLE'
ORDER BY TABLE_SCHEMA, TABLE_NAME;
GO

PRINT N'=== CAC STORED PROCEDURE HIEN CO ===';
SELECT SCHEMA_NAME(schema_id) AS SchemaName, name AS ProcedureName
FROM sys.procedures
WHERE is_ms_shipped = 0
ORDER BY SchemaName, ProcedureName;
GO
