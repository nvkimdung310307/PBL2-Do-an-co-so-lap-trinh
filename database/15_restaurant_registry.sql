/*
  Registry foundation for custom restaurant name/code.
  Run once on RestaurantManagement after backing up the database.
  NOTE: This script creates the restaurant registry only. Full multi-tenant isolation
  requires adding MaNhaHang foreign keys to every business table and filtering every query.
*/
USE RestaurantManagement;
GO
IF OBJECT_ID(N'dbo.NhaHang', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.NhaHang
    (
        MaNhaHang VARCHAR(20) NOT NULL PRIMARY KEY,
        TenNhaHang NVARCHAR(150) NOT NULL,
        NgayTao DATETIME2 NOT NULL CONSTRAINT DF_NhaHang_NgayTao DEFAULT SYSDATETIME(),
        TrangThai NVARCHAR(20) NOT NULL CONSTRAINT DF_NhaHang_TrangThai DEFAULT N'Hoạt động',
        CONSTRAINT CK_NhaHang_TrangThai CHECK (TrangThai IN (N'Hoạt động', N'Tạm khóa'))
    );
END;
GO
