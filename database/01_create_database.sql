/* ============================================================
   01_create_database.sql
   AN TOAN: KHONG XOA DATABASE CU / KHONG XOA DU LIEU.
   Chay script nay de tao database neu chua ton tai.
   ============================================================ */
USE master;
GO

IF DB_ID(N'RestaurantManagement') IS NULL
BEGIN
    CREATE DATABASE RestaurantManagement;
    PRINT N'Da tao database RestaurantManagement.';
END
ELSE
BEGIN
    PRINT N'Database RestaurantManagement da ton tai; giu nguyen du lieu.';
END;
GO

USE RestaurantManagement;
GO
