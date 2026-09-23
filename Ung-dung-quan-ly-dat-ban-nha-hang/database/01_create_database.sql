USE master;
GO
-- Xóa database cũ nếu tồn tại
IF DB_ID(N'RestaurantManagement') IS NOT NULL
BEGIN
    ALTER DATABASE RestaurantManagement
    SET SINGLE_USER
    WITH ROLLBACK IMMEDIATE;

    DROP DATABASE RestaurantManagement;
END
GO
  
CREATE DATABASE RestaurantManagement;
GO

USE RestaurantManagement;
GO

PRINT N'Tạo database RestaurantManagement thành công!';
GO
