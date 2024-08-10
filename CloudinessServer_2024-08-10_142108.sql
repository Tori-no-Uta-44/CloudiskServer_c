-- MySQL dump 10.13  Distrib 8.0.39, for Linux (x86_64)
--
-- Host: 192.168.153.129    Database: CloudinessServer
-- ------------------------------------------------------
-- Server version	8.0.39-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `globalFile`
--

DROP TABLE IF EXISTS `globalFile`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `globalFile` (
  `hashId` int NOT NULL AUTO_INCREMENT COMMENT '文件哈希值的ID',
  `hash` varchar(512) DEFAULT NULL COMMENT '哈希值',
  `linkCount` int DEFAULT NULL COMMENT '连接数',
  `lastLinkTime` datetime DEFAULT NULL,
  `isDel` set('true','false') DEFAULT 'false' COMMENT '服务器文件是否删除与否',
  PRIMARY KEY (`hashId`)
) ENGINE=InnoDB AUTO_INCREMENT=1007 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='全局文件';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `globalFile`
--

/*!40000 ALTER TABLE `globalFile` DISABLE KEYS */;
INSERT INTO `globalFile` VALUES (1,'gK0jeOlBr1d0KMO2BICODNYE/23A59EYt3dKg15jz70',1,'2024-08-07 00:00:00','true'),(111,'gK0jeOlBr1d0KMO2BICODNYE/23A59EYt3dKg15jz88',1,NULL,'false'),(123,'gK0jeOlBr1d0KMO2BICODNYE/23A59EYt3dKg15jz99',3,NULL,'false'),(222,'gK0jeOlBr1d0KMO2BICODNYE/23A59EYt3dKg15aaaa',3,NULL,'false'),(333,'gK0jeOlBr1d0KMO2BICODNYEddzd59EYt3dKg15jz70',4,NULL,'false'),(444,'444+444',5,NULL,'false'),(555,'555+555',7,NULL,'false'),(666,'666+666',8,NULL,'false'),(999,'777+777',-1,'2024-08-07 00:00:00','false'),(1005,'75afe042d148616b2e31116a14d3776188534347e25e3f37bb714dbeb2f29698',9,NULL,'false'),(1006,'75afe042d148616b2e31116a14d3776188534347e25e3f37bb714dbeb2f29698',0,'2024-08-10 14:11:59','false');
/*!40000 ALTER TABLE `globalFile` ENABLE KEYS */;

--
-- Table structure for table `log`
--

DROP TABLE IF EXISTS `log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `log` (
  `log_level` varchar(16) NOT NULL COMMENT '日志级别',
  `file` varchar(128) NOT NULL COMMENT '文件位置',
  `line` int NOT NULL COMMENT '行号',
  `times` datetime NOT NULL COMMENT '添加时间',
  `info` varchar(200) DEFAULT NULL COMMENT '详细信息'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='日志';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `log`
--

/*!40000 ALTER TABLE `log` DISABLE KEYS */;
INSERT INTO `log` VALUES ('INFO','main.c',10,'2024-08-06 17:01:53','ls'),('INFO','main.c',23,'2024-08-06 17:01:53','ls'),('INFO','main.c',10,'2024-08-06 17:01:53','ls'),('NOTICE','main.c',24,'2024-08-06 17:01:53','len is 0'),('INFO','bussiness.c',102,'2024-08-08 20:14:17','section1'),('INFO','bussiness.c',106,'2024-08-08 20:14:20','section2'),('INFO','bussiness.c',74,'2024-08-08 20:14:24','ls'),('INFO','bussiness.c',102,'2024-08-08 20:35:45','section1'),('INFO','bussiness.c',106,'2024-08-08 20:35:46','section2'),('INFO','bussiness.c',78,'2024-08-08 20:35:48','rm'),('INFO','bussiness.c',102,'2024-08-08 21:15:10','section1'),('INFO','bussiness.c',106,'2024-08-08 21:15:12','section2'),('INFO','bussiness.c',78,'2024-08-08 21:15:18','rm'),('INFO','bussiness.c',102,'2024-08-08 21:22:52','section1'),('INFO','bussiness.c',106,'2024-08-08 21:22:54','section2'),('INFO','bussiness.c',102,'2024-08-08 21:24:05','section1'),('INFO','bussiness.c',106,'2024-08-08 21:24:08','section2'),('INFO','bussiness.c',78,'2024-08-08 21:24:17','rm'),('INFO','bussiness.c',102,'2024-08-08 21:49:27','section1'),('INFO','bussiness.c',106,'2024-08-08 21:49:29','section2'),('INFO','bussiness.c',78,'2024-08-08 21:49:54','rm'),('INFO','bussiness.c',102,'2024-08-08 21:57:49','section1'),('INFO','bussiness.c',106,'2024-08-08 21:57:51','section2'),('INFO','bussiness.c',78,'2024-08-08 21:58:12','rm'),('INFO','bussiness.c',102,'2024-08-08 22:12:50','section1'),('INFO','bussiness.c',106,'2024-08-08 22:12:52','section2'),('INFO','bussiness.c',78,'2024-08-08 22:13:14','rm'),('INFO','bussiness.c',78,'2024-08-08 22:13:40','rm'),('INFO','bussiness.c',78,'2024-08-08 22:13:59','rm'),('INFO','bussiness.c',102,'2024-08-09 01:19:03','section1'),('INFO','bussiness.c',106,'2024-08-09 01:19:05','section2'),('INFO','bussiness.c',86,'2024-08-09 01:19:51','rmdir'),('INFO','bussiness.c',153,'2024-08-09 11:30:15','section1'),('INFO','bussiness.c',153,'2024-08-09 11:31:33','section1'),('INFO','bussiness.c',153,'2024-08-09 11:34:57','section1'),('INFO','bussiness.c',153,'2024-08-09 11:40:31','section1'),('INFO','bussiness.c',153,'2024-08-09 11:49:02','section1'),('INFO','bussiness.c',157,'2024-08-09 11:49:30','section2'),('INFO','bussiness.c',153,'2024-08-09 11:49:52','section1'),('INFO','bussiness.c',157,'2024-08-09 11:49:55','section2'),('INFO','bussiness.c',141,'2024-08-09 11:50:11','gets'),('INFO','bussiness.c',141,'2024-08-09 11:50:35','gets'),('INFO','bussiness.c',153,'2024-08-09 13:46:29','section1'),('INFO','bussiness.c',157,'2024-08-09 13:46:31','section2'),('INFO','bussiness.c',153,'2024-08-09 13:47:19','section1'),('INFO','bussiness.c',157,'2024-08-09 13:47:21','section2'),('INFO','bussiness.c',137,'2024-08-09 13:47:29','puts'),('INFO','bussiness.c',153,'2024-08-09 13:55:58','section1'),('INFO','bussiness.c',157,'2024-08-09 13:56:00','section2'),('INFO','bussiness.c',137,'2024-08-09 13:56:36','puts'),('INFO','bussiness.c',121,'2024-08-09 13:57:20','rm'),('INFO','bussiness.c',137,'2024-08-09 13:57:56','puts'),('INFO','bussiness.c',153,'2024-08-09 14:13:35','section1'),('INFO','bussiness.c',157,'2024-08-09 14:13:37','section2'),('INFO','bussiness.c',137,'2024-08-09 14:13:48','puts'),('INFO','bussiness.c',121,'2024-08-09 14:14:56','rm'),('DEBUG','main.c',87,'2024-08-09 16:48:44','test'),('INFO','bussiness.c',155,'2024-08-09 16:48:55','section1'),('INFO','bussiness.c',159,'2024-08-09 16:48:58','section2'),('INFO','bussiness.c',139,'2024-08-09 16:50:15','puts'),('INFO','bussiness.c',123,'2024-08-09 16:51:40','rm'),('DEBUG','main.c',87,'2024-08-09 16:52:55','test'),('INFO','bussiness.c',155,'2024-08-09 16:53:04','section1'),('INFO','bussiness.c',159,'2024-08-09 16:53:06','section2'),('INFO','bussiness.c',139,'2024-08-09 16:53:15','puts'),('INFO','bussiness.c',155,'2024-08-09 16:53:46','section1'),('INFO','bussiness.c',159,'2024-08-09 16:53:48','section2'),('INFO','bussiness.c',123,'2024-08-09 16:53:57','rm'),('DEBUG','main.c',87,'2024-08-09 16:56:11','test'),('INFO','bussiness.c',155,'2024-08-09 16:56:17','section1'),('INFO','bussiness.c',159,'2024-08-09 16:56:19','section2'),('INFO','bussiness.c',139,'2024-08-09 16:56:29','puts'),('INFO','bussiness.c',123,'2024-08-09 16:56:38','rm'),('DEBUG','main.c',87,'2024-08-09 17:07:51','test'),('INFO','bussiness.c',155,'2024-08-09 17:08:28','section1'),('INFO','bussiness.c',159,'2024-08-09 17:08:29','section2'),('INFO','bussiness.c',139,'2024-08-09 17:08:41','puts'),('INFO','bussiness.c',155,'2024-08-09 17:09:15','section1'),('INFO','bussiness.c',159,'2024-08-09 17:09:21','section2'),('INFO','bussiness.c',123,'2024-08-09 17:09:28','rm'),('DEBUG','main.c',87,'2024-08-09 17:11:05','test'),('INFO','bussiness.c',155,'2024-08-09 17:11:16','section1'),('INFO','bussiness.c',159,'2024-08-09 17:11:20','section2'),('INFO','bussiness.c',123,'2024-08-09 17:11:26','rm'),('INFO','bussiness.c',123,'2024-08-09 17:14:12','rm'),('DEBUG','main.c',87,'2024-08-09 17:17:40','test'),('INFO','bussiness.c',155,'2024-08-09 17:17:47','section1'),('INFO','bussiness.c',159,'2024-08-09 17:17:49','section2'),('INFO','bussiness.c',123,'2024-08-09 17:17:57','rm'),('DEBUG','main.c',87,'2024-08-09 20:24:49','test'),('DEBUG','main.c',87,'2024-08-09 20:25:30','test'),('DEBUG','main.c',87,'2024-08-09 20:34:19','test'),('INFO','bussiness.c',155,'2024-08-09 20:34:26','section1'),('INFO','bussiness.c',159,'2024-08-09 20:34:29','section2'),('INFO','bussiness.c',127,'2024-08-09 20:34:41','mkdir'),('INFO','bussiness.c',127,'2024-08-09 20:36:25','mkdir'),('INFO','bussiness.c',131,'2024-08-09 20:36:48','rmdir'),('DEBUG','main.c',87,'2024-08-09 20:38:50','test'),('INFO','bussiness.c',155,'2024-08-09 20:38:56','section1'),('INFO','bussiness.c',159,'2024-08-09 20:38:59','section2'),('INFO','bussiness.c',127,'2024-08-09 20:39:11','mkdir'),('INFO','bussiness.c',127,'2024-08-09 20:39:18','mkdir'),('INFO','bussiness.c',131,'2024-08-09 20:39:30','rmdir'),('DEBUG','main.c',87,'2024-08-09 20:41:48','test'),('INFO','bussiness.c',155,'2024-08-09 20:41:52','section1'),('INFO','bussiness.c',159,'2024-08-09 20:41:54','section2'),('INFO','bussiness.c',127,'2024-08-09 20:42:13','mkdir'),('INFO','bussiness.c',131,'2024-08-09 20:42:25','rmdir'),('INFO','bussiness.c',127,'2024-08-09 20:44:03','mkdir'),('DEBUG','main.c',87,'2024-08-09 20:53:59','test'),('INFO','bussiness.c',155,'2024-08-09 20:54:03','section1'),('INFO','bussiness.c',159,'2024-08-09 20:54:05','section2'),('INFO','bussiness.c',127,'2024-08-09 20:54:19','mkdir'),('DEBUG','main.c',87,'2024-08-09 20:57:00','test'),('INFO','bussiness.c',155,'2024-08-09 20:57:07','section1'),('INFO','bussiness.c',159,'2024-08-09 20:57:10','section2'),('INFO','bussiness.c',127,'2024-08-09 20:57:21','mkdir'),('DEBUG','main.c',87,'2024-08-09 20:59:44','test'),('INFO','bussiness.c',155,'2024-08-09 20:59:48','section1'),('INFO','bussiness.c',159,'2024-08-09 20:59:52','section2'),('INFO','bussiness.c',127,'2024-08-09 20:59:58','mkdir'),('DEBUG','main.c',87,'2024-08-09 21:02:24','test'),('INFO','bussiness.c',155,'2024-08-09 21:02:28','section1'),('INFO','bussiness.c',159,'2024-08-09 21:02:30','section2'),('INFO','bussiness.c',127,'2024-08-09 21:02:40','mkdir'),('DEBUG','main.c',87,'2024-08-09 21:07:14','test'),('INFO','bussiness.c',155,'2024-08-09 21:07:18','section1'),('INFO','bussiness.c',159,'2024-08-09 21:07:20','section2'),('INFO','bussiness.c',127,'2024-08-09 21:07:27','mkdir'),('DEBUG','main.c',87,'2024-08-09 21:09:12','test'),('INFO','bussiness.c',155,'2024-08-09 21:09:17','section1'),('INFO','bussiness.c',159,'2024-08-09 21:09:19','section2'),('INFO','bussiness.c',127,'2024-08-09 21:09:29','mkdir'),('DEBUG','main.c',87,'2024-08-09 21:18:11','test'),('INFO','bussiness.c',155,'2024-08-09 21:18:15','section1'),('INFO','bussiness.c',159,'2024-08-09 21:18:17','section2'),('INFO','bussiness.c',127,'2024-08-09 21:18:24','mkdir'),('INFO','bussiness.c',131,'2024-08-09 21:18:38','rmdir'),('DEBUG','main.c',87,'2024-08-09 21:20:34','test'),('INFO','bussiness.c',155,'2024-08-09 21:20:38','section1'),('INFO','bussiness.c',159,'2024-08-09 21:20:40','section2'),('INFO','bussiness.c',155,'2024-08-09 21:20:56','section1'),('INFO','bussiness.c',159,'2024-08-09 21:20:58','section2'),('DEBUG','main.c',87,'2024-08-09 21:21:28','test'),('INFO','bussiness.c',155,'2024-08-09 21:21:32','section1'),('INFO','bussiness.c',159,'2024-08-09 21:21:34','section2'),('INFO','bussiness.c',127,'2024-08-09 21:21:38','mkdir'),('INFO','bussiness.c',131,'2024-08-09 21:21:50','rmdir'),('INFO','bussiness.c',127,'2024-08-09 21:22:02','mkdir'),('INFO','bussiness.c',131,'2024-08-09 21:22:43','rmdir'),('DEBUG','main.c',87,'2024-08-09 21:25:27','test'),('INFO','bussiness.c',155,'2024-08-09 21:25:30','section1'),('INFO','bussiness.c',159,'2024-08-09 21:25:32','section2'),('INFO','bussiness.c',127,'2024-08-09 21:25:40','mkdir'),('INFO','bussiness.c',127,'2024-08-09 21:25:46','mkdir'),('INFO','bussiness.c',131,'2024-08-09 21:26:05','rmdir'),('DEBUG','main.c',87,'2024-08-10 09:31:25','test'),('INFO','bussiness.c',155,'2024-08-10 09:31:39','section1'),('INFO','bussiness.c',159,'2024-08-10 09:31:41','section2'),('INFO','bussiness.c',119,'2024-08-10 09:31:49','ls'),('INFO','bussiness.c',115,'2024-08-10 09:31:59','cd'),('INFO','bussiness.c',119,'2024-08-10 09:32:03','ls'),('INFO','bussiness.c',127,'2024-08-10 09:32:12','mkdir'),('INFO','bussiness.c',115,'2024-08-10 09:32:22','cd'),('INFO','bussiness.c',139,'2024-08-10 09:32:41','puts'),('INFO','bussiness.c',119,'2024-08-10 09:32:42','ls'),('INFO','bussiness.c',115,'2024-08-10 09:33:21','cd'),('DEBUG','main.c',87,'2024-08-10 09:51:43','test'),('INFO','bussiness.c',155,'2024-08-10 09:51:47','section1'),('INFO','bussiness.c',159,'2024-08-10 09:51:49','section2'),('INFO','bussiness.c',119,'2024-08-10 09:51:54','ls'),('INFO','bussiness.c',155,'2024-08-10 09:52:46','section1'),('INFO','bussiness.c',159,'2024-08-10 09:52:48','section2'),('INFO','bussiness.c',119,'2024-08-10 09:52:50','ls'),('DEBUG','main.c',87,'2024-08-10 09:53:07','test'),('INFO','bussiness.c',155,'2024-08-10 09:53:11','section1'),('INFO','bussiness.c',159,'2024-08-10 09:53:13','section2'),('INFO','bussiness.c',119,'2024-08-10 09:53:18','ls'),('INFO','bussiness.c',127,'2024-08-10 09:53:33','mkdir'),('INFO','bussiness.c',115,'2024-08-10 09:53:39','cd'),('INFO','bussiness.c',119,'2024-08-10 09:53:46','ls'),('INFO','bussiness.c',139,'2024-08-10 09:54:58','puts'),('INFO','bussiness.c',119,'2024-08-10 09:55:03','ls'),('INFO','bussiness.c',119,'2024-08-10 09:56:13','ls'),('INFO','bussiness.c',115,'2024-08-10 09:56:39','cd'),('DEBUG','main.c',87,'2024-08-10 10:02:16','test'),('DEBUG','main.c',87,'2024-08-10 10:03:11','test'),('INFO','bussiness.c',155,'2024-08-10 10:03:16','section1'),('INFO','bussiness.c',159,'2024-08-10 10:03:18','section2'),('INFO','bussiness.c',119,'2024-08-10 10:03:20','ls'),('DEBUG','main.c',87,'2024-08-10 10:03:46','test'),('INFO','bussiness.c',155,'2024-08-10 10:03:50','section1'),('INFO','bussiness.c',159,'2024-08-10 10:03:51','section2'),('INFO','bussiness.c',119,'2024-08-10 10:03:53','ls'),('INFO','bussiness.c',127,'2024-08-10 10:03:59','mkdir'),('INFO','bussiness.c',119,'2024-08-10 10:04:01','ls'),('INFO','bussiness.c',115,'2024-08-10 10:04:05','cd'),('INFO','bussiness.c',139,'2024-08-10 10:04:15','puts'),('INFO','bussiness.c',119,'2024-08-10 10:04:20','ls'),('INFO','bussiness.c',115,'2024-08-10 10:04:23','cd'),('INFO','bussiness.c',119,'2024-08-10 10:04:25','ls'),('INFO','bussiness.c',115,'2024-08-10 10:06:00','cd'),('INFO','bussiness.c',127,'2024-08-10 10:06:06','mkdir'),('INFO','bussiness.c',119,'2024-08-10 10:06:13','ls'),('INFO','bussiness.c',119,'2024-08-10 10:06:51','ls'),('INFO','bussiness.c',115,'2024-08-10 10:06:54','cd'),('INFO','bussiness.c',119,'2024-08-10 10:06:56','ls'),('INFO','bussiness.c',131,'2024-08-10 10:07:01','rmdir'),('INFO','bussiness.c',131,'2024-08-10 10:08:00','rmdir'),('INFO','bussiness.c',123,'2024-08-10 10:08:42','rm'),('INFO','bussiness.c',119,'2024-08-10 10:08:52','ls'),('INFO','bussiness.c',127,'2024-08-10 10:08:57','mkdir'),('INFO','bussiness.c',115,'2024-08-10 10:09:21','cd'),('INFO','bussiness.c',127,'2024-08-10 10:09:26','mkdir'),('INFO','bussiness.c',119,'2024-08-10 10:09:38','ls'),('INFO','bussiness.c',139,'2024-08-10 10:09:44','puts'),('DEBUG','main.c',87,'2024-08-10 10:23:37','test'),('INFO','bussiness.c',155,'2024-08-10 10:23:41','section1'),('INFO','bussiness.c',159,'2024-08-10 10:23:44','section2'),('INFO','bussiness.c',119,'2024-08-10 10:23:47','ls'),('DEBUG','main.c',87,'2024-08-10 10:24:19','test'),('INFO','bussiness.c',155,'2024-08-10 10:24:23','section1'),('INFO','bussiness.c',159,'2024-08-10 10:24:24','section2'),('INFO','bussiness.c',119,'2024-08-10 10:24:26','ls'),('INFO','bussiness.c',127,'2024-08-10 10:24:31','mkdir'),('INFO','bussiness.c',119,'2024-08-10 10:24:32','ls'),('INFO','bussiness.c',115,'2024-08-10 10:24:36','cd'),('INFO','bussiness.c',115,'2024-08-10 10:24:39','cd'),('INFO','bussiness.c',119,'2024-08-10 10:24:42','ls'),('INFO','bussiness.c',139,'2024-08-10 10:24:48','puts'),('INFO','bussiness.c',139,'2024-08-10 10:25:03','puts'),('INFO','bussiness.c',119,'2024-08-10 10:25:06','ls'),('INFO','bussiness.c',127,'2024-08-10 10:25:16','mkdir'),('INFO','bussiness.c',119,'2024-08-10 10:25:17','ls'),('INFO','bussiness.c',115,'2024-08-10 10:25:31','cd'),('INFO','bussiness.c',119,'2024-08-10 10:25:32','ls'),('INFO','bussiness.c',131,'2024-08-10 10:25:38','rmdir'),('INFO','bussiness.c',119,'2024-08-10 10:25:42','ls'),('DEBUG','main.c',87,'2024-08-10 11:14:35','test'),('INFO','bussiness.c',155,'2024-08-10 11:14:40','section1'),('INFO','bussiness.c',159,'2024-08-10 11:14:43','section2'),('INFO','bussiness.c',119,'2024-08-10 11:14:49','ls'),('INFO','bussiness.c',127,'2024-08-10 11:15:01','mkdir'),('INFO','bussiness.c',115,'2024-08-10 11:15:18','cd'),('INFO','bussiness.c',115,'2024-08-10 11:15:24','cd'),('INFO','bussiness.c',115,'2024-08-10 11:15:27','cd'),('INFO','bussiness.c',115,'2024-08-10 11:17:13','cd'),('INFO','bussiness.c',127,'2024-08-10 11:17:17','mkdir'),('INFO','bussiness.c',127,'2024-08-10 11:17:33','mkdir'),('INFO','bussiness.c',139,'2024-08-10 11:17:40','puts'),('INFO','bussiness.c',139,'2024-08-10 11:18:29','puts'),('INFO','bussiness.c',119,'2024-08-10 11:18:32','ls'),('INFO','bussiness.c',119,'2024-08-10 11:18:37','ls'),('INFO','bussiness.c',115,'2024-08-10 11:23:32','cd'),('INFO','bussiness.c',131,'2024-08-10 11:23:53','rmdir'),('INFO','bussiness.c',119,'2024-08-10 11:23:55','ls'),('INFO','bussiness.c',119,'2024-08-10 11:24:57','ls'),('INFO','bussiness.c',127,'2024-08-10 11:25:02','mkdir'),('INFO','bussiness.c',115,'2024-08-10 11:25:04','cd'),('INFO','bussiness.c',127,'2024-08-10 11:25:08','mkdir'),('INFO','bussiness.c',127,'2024-08-10 11:25:11','mkdir'),('INFO','bussiness.c',139,'2024-08-10 11:25:28','puts'),('INFO','bussiness.c',139,'2024-08-10 11:25:35','puts'),('INFO','bussiness.c',119,'2024-08-10 11:25:39','ls'),('INFO','bussiness.c',115,'2024-08-10 11:25:42','cd'),('INFO','bussiness.c',119,'2024-08-10 11:25:44','ls'),('INFO','bussiness.c',131,'2024-08-10 11:25:52','rmdir'),('INFO','bussiness.c',119,'2024-08-10 11:25:54','ls'),('DEBUG','main.c',87,'2024-08-10 11:28:17','test'),('INFO','bussiness.c',155,'2024-08-10 11:28:21','section1'),('INFO','bussiness.c',159,'2024-08-10 11:28:23','section2'),('INFO','bussiness.c',119,'2024-08-10 11:28:26','ls'),('INFO','bussiness.c',123,'2024-08-10 11:28:36','rm'),('INFO','bussiness.c',139,'2024-08-10 11:30:18','puts'),('INFO','bussiness.c',119,'2024-08-10 11:30:19','ls'),('INFO','bussiness.c',123,'2024-08-10 11:30:32','rm'),('ERROR','main.c',87,'2024-08-10 14:09:53','test'),('INFO','bussiness.c',155,'2024-08-10 14:10:05','section1'),('INFO','bussiness.c',159,'2024-08-10 14:10:08','section2'),('INFO','bussiness.c',119,'2024-08-10 14:10:14','ls'),('INFO','bussiness.c',127,'2024-08-10 14:10:22','mkdir'),('INFO','bussiness.c',119,'2024-08-10 14:10:24','ls'),('INFO','bussiness.c',115,'2024-08-10 14:10:29','cd'),('INFO','bussiness.c',127,'2024-08-10 14:10:38','mkdir'),('INFO','bussiness.c',127,'2024-08-10 14:10:42','mkdir'),('INFO','bussiness.c',139,'2024-08-10 14:10:48','puts'),('INFO','bussiness.c',119,'2024-08-10 14:10:50','ls'),('INFO','bussiness.c',119,'2024-08-10 14:10:55','ls'),('INFO','bussiness.c',139,'2024-08-10 14:11:16','puts'),('INFO','bussiness.c',119,'2024-08-10 14:11:24','ls'),('INFO','bussiness.c',139,'2024-08-10 14:11:34','puts'),('INFO','bussiness.c',119,'2024-08-10 14:11:37','ls'),('INFO','bussiness.c',119,'2024-08-10 14:11:41','ls'),('INFO','bussiness.c',115,'2024-08-10 14:11:52','cd'),('INFO','bussiness.c',119,'2024-08-10 14:11:55','ls'),('INFO','bussiness.c',131,'2024-08-10 14:11:59','rmdir'),('INFO','bussiness.c',119,'2024-08-10 14:12:01','ls');
/*!40000 ALTER TABLE `log` ENABLE KEYS */;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `user` (
  `userId` int NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `userName` varchar(32) DEFAULT NULL COMMENT '用户名',
  `userPassword` varchar(32) NOT NULL COMMENT '用户密码',
  `salt` varchar(32) NOT NULL COMMENT '盐值',
  `passwordLastChange` datetime NOT NULL DEFAULT (now()) COMMENT '最后一次密码更改时间',
  `fileId` int NOT NULL DEFAULT '0' COMMENT '退出时所在的虚拟路径',
  PRIMARY KEY (`userId`),
  UNIQUE KEY `user_pk` (`userName`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='用户';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user`
--

/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (1,'user0','123456','GdUsV00yIWe85unzxrf00','2024-08-07 15:05:53',0),(2,'will','123456','0ct9v7sDl52WcvpODDbWs/','2024-08-07 15:46:00',2),(3,'root','123456','111111111111111111','2024-08-07 15:46:00',3);
/*!40000 ALTER TABLE `user` ENABLE KEYS */;

--
-- Table structure for table `virtualFile`
--

DROP TABLE IF EXISTS `virtualFile`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `virtualFile` (
  `fileId` int NOT NULL AUTO_INCREMENT COMMENT '文件ID',
  `parentId` int NOT NULL COMMENT '所在上一层目录，0表示顶层目录',
  `fileName` varchar(32) NOT NULL COMMENT '文件名',
  `userId` int NOT NULL COMMENT '文件里属于某一个用户的ID',
  `hashId` int DEFAULT NULL COMMENT '文件对应的哈希在全局文件表中的ID',
  `fileSize` int NOT NULL COMMENT '文件大小',
  `type` set('Directory','File') NOT NULL COMMENT '文件类型',
  `path` varchar(32) NOT NULL COMMENT '完整虚拟路径',
  PRIMARY KEY (`fileId`),
  UNIQUE KEY `path` (`path`),
  KEY `virtualFile_user_userId_fk` (`userId`),
  KEY `virtualFile_globalFile_hashId_fk` (`hashId`),
  CONSTRAINT `virtualFile_globalFile_hashId_fk` FOREIGN KEY (`hashId`) REFERENCES `globalFile` (`hashId`),
  CONSTRAINT `virtualFile_user_userId_fk` FOREIGN KEY (`userId`) REFERENCES `user` (`userId`)
) ENGINE=InnoDB AUTO_INCREMENT=72 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='虚拟文件';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `virtualFile`
--

/*!40000 ALTER TABLE `virtualFile` DISABLE KEYS */;
INSERT INTO `virtualFile` VALUES (1,0,'file',1,NULL,-1,'Directory','/file'),(2,1,'will',2,NULL,-1,'Directory','/file/will'),(3,1,'root',3,123,-1,'Directory','/file/root'),(4,2,'2.txt',2,123,123,'File','/file/will/2.txt'),(6,2,'aaa',2,NULL,-1,'Directory','/file/will/aaa'),(10,6,'cccc',2,NULL,-1,'Directory','/file/will/aaa/cccc'),(12,6,'dddd',2,NULL,-1,'Directory','/file/will/aaa/dddd'),(13,6,'eeee',2,NULL,-1,'Directory','/file/will/aaa/eeee'),(14,6,'text1',2,222,222,'File','/file/will/aaa/text1'),(15,6,'text2',2,333,333,'File','/file/will/aaa/text2'),(16,3,'1111',3,444,-1,'Directory','/file/root/1111');
/*!40000 ALTER TABLE `virtualFile` ENABLE KEYS */;

--
-- Dumping routines for database 'CloudinessServer'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-08-10 14:22:30
