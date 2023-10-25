### 一、下载fatfs源码

**官网源码**：http://elm-chan.org/fsw/ff/00index_e.html

**移植Fatfs**：https://blog.csdn.net/weixin_55902760/article/details/132332438?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EYuanLiJiHua%7EPosition-2-132332438-blog-107129578.235%5Ev38%5Epc_relevant_default_base&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EYuanLiJiHua%7EPosition-2-132332438-blog-107129578.235%5Ev38%5Epc_relevant_default_base&utm_relevant_index=5

**Fatfs api详解**：https://blog.csdn.net/qq_39778488/article/details/107129578

### 二、硬件底层API接口函数实现。

#### 2.1 **diskio.c **文件

##### 2.1.1 给不同设备定义不同设备编号

![image-20231025094427623](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20231025094427623.png)

##### 2.1.2 **disk_status ()** 函数

实现 SD 卡或是 spi flash 的设备的标识获取。

```
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case SD_CARD :
        break;
    
	case SPI_FLASH :
        if (SPI_FLASH_ReadID() != W25Q64_ID)
        {
            stat = STA_NOINIT;
        }
		else {
            stat = 0;
        }
        break;
    default:
        break;
    }      
	return stat;
}
```

##### 2.1.3  **disk_initialize()** 函数

实现对不同设备的初始化。函数调用关系：f_mount() ----> disk_initialize() ----> disk_status()

```
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
    uint16_t i = 500;
    
	switch (pdrv) {
	case SD_CARD :
        break;
    
	case SPI_FLASH :
        SPI_FLASH_Init();
        while(i--);
        SPI_Flash_WAKEUP();
        stat = disk_status(SPI_FLASH);
        break;
    
    default:
        break;
	}
	return stat;
}
```

##### 2.1.4 **disk_read()** 函数

函数调用关系：f_read()  ----> disk_read()

```
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv) {
	case SD_CARD :
        break;
    
	case SPI_FLASH :
        SPI_FLASH_BufferRead(buff, 
                             sector * FLASH_FATFS_SECTOR_SIZE, 
                             count * FLASH_FATFS_SECTOR_SIZE);
        res = RES_OK;
        break;
    
    default:
        break;
	}

	return res;
}
```

##### 2.1.5 **disk_write()** 函数

函数调用关系：f_write()  ----> disk_write()

```
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_PARERR;

	switch (pdrv) {
	case SD_CARD :
        break;
    
	case SPI_FLASH :
        SPI_FLASH_Diskio_Write((uint8_t *)buff, 
                              sector * FLASH_FATFS_SECTOR_SIZE, 
                              count * FLASH_FATFS_SECTOR_SIZE);
        res = RES_OK;
        break;
    
    default:
        break;
	}

	return res;
}
```

##### 2.1.6 **disk_ioctl()** 函数

调用disk_ioctl函数以控制设备的特定功能和除通用读/写之外的其他功能。比如通过发送命令来获取该设备的扇区大小、内存大小等相关信息。

```
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;
	int result;

	switch (pdrv) {
	case SD_CARD :
        break;
    
	case SPI_FLASH : 
    {
        switch (cmd) 
        {
            case GET_SECTOR_COUNT:
                *(DWORD *)buff = FLASH_FATFS_SIZE / FLASH_FATFS_SECTOR_SIZE;
                break;
            case GET_SECTOR_SIZE:
                /* 一个扇区大小 */
                *(DWORD *)buff = FLASH_FATFS_SECTOR_SIZE;
                break;
            case GET_BLOCK_SIZE:
                /* 擦除的最小扇区数 */
                *(DWORD *)buff = FLASH_FATFS_BLOCK_SIZE;
                break;
            default:
                break;
        }
        res = RES_OK;
    }
    break;
    default:
        break;
    }

	return res;
}
```

#### 2.2 ff.c 文件

##### 2.2.1 **get_fattime()** 函数

```
__weak DWORD get_fattime(void)
{
    return ((DWORD)(2015 - 1980) << 25) |
            ((DWORD)1 << 21)            |
            ((DWORD)1 << 16)            |
            ((DWORD)0 << 11)            |
            ((DWORD)0 << 5)             |
            ((DWORD)0 << 1);             
}
```

#### 2.3 ffconf.h 配置

**fatfs 配置详解** ：https://blog.csdn.net/shaynerain/article/details/107111154

##### 2.3.1 开启的卷数量配置

```
#define FF_VOLUMES		2
/* Number of volumes (logical drives) to be used. (1-10) */


#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"SD","Flash"
```

##### 2.3.2 配置扇区大小

```
#define FF_MIN_SS		512
#define FF_MAX_SS		512
```

### 三、常见问题

#### 3.1 格式化常见错误

1、在 <ffconf.h> 中的“#define FF_VOLUMES 1”设置不对；

2、在 <ffconf.h> 中的 **#define FF_MAX_SS 512** 设置不对，此处的数值表示所挂的所有设备中，设备扇区的最大值。如w25qxx的扇区是4096，如果此处还是设置成512，就会造成栈溢出而陷入HardFault_Handler的死循环中。同时还有一个变量 **#define FF_MIN_SS 512** 表示所挂的所有设备中，设备扇区的最小值，一般设置为512即可，因为扇区最小值就是512。

3、出现设备上不存在文件系统的错误，即“res == FR_NO_FILESYSTEM”。这是因为我们还没有对设备进行格式化，如果格式化后就会在设备上存储一些文件信息。

```
    ret = f_mount(&fs, "1:", 1);  // step1
    if(ret == FR_NO_FILESYSTEM)
    {
        printf("》spi flash还没有文件系统，即将进行格式化...\r\n");
        ret = f_mkfs("1:", 0, ReadBuffer, sizeof(ReadBuffer)); // step2
        if(ret == FR_OK)
        {
            printf("》spi flash已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
			ret = f_mount(NULL, "1:", 1);			
            /* 重新挂载	*/			
			ret = f_mount(&fs,"1:", 1);		// step3
            if(ret == FR_OK)
            {
                 printf("》文件系统挂载成功，可以进行读写测试\r\n");
            }
        }
        else
        {
            printf("《《格式化失败。》》\r\n");
            while(1);
        }
    }
```

如以上，step1 报错一般是初始化设备与卷符号对应不上；step2 报错一般是 **FF_MAX_SS** 设置过大，工作区缓存 ReadBuffer 过小；step3 报错显示 **FR_NOT_ENABLED** ，一般是串行FLASH进行写操作之前没有进行擦除操作。



