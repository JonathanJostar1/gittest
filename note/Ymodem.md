#### YModem协议：

**1、数据起始帧**

YModem的起始帧并不直接传输文件的数据，而是将文件名与文件的大小放在数据帧中传输，帧长 = 3字节数据首部 + 128字节数据 + 2字节（CRC16校验码）= 133字节。

数据结构如下：**SOH 00 FF filename filezise NUL CRCH CRCL**

**2、数据帧**

数据结构如下：**STX 01 FE data[1024] CRCH CRCL**

最后一包有两种情况：

① 如果文件数据的最后剩余的数据在128~1024之前，则还是使用**STX**的1024字节传输，但是剩余空间全部用0x1A填充；

数据结构如下：**STX 01 FE data[1024] 1A 1A……… CRCH CRCL**

② 如果文件大小 ≤128byte 或者文件数据最后剩余的数据 <128byte，则YModem会选择**SOH**数据帧用128字节来传输数据，如果数据不满128字节，剩余的数据用**0x1A**填充;

数据结构如下：**SOH 01 FE data[ ] 1A …1A CRCH CRCL**

**3、结束帧**

YModem的结束帧数据也采用SOH的128字节数据帧；

数据结构如下：**SOH 00 FF NUL[128] CRCH CRCL**

**4、传输过程**

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< C

发送端 ：SOH 00 FF [55 53…6E 00] [32…30 00] NUL[96] CRC CRC >>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< C

发送端 ：STX 01 FE data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

发送端 ：STX 02 FD data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

发送端 ：STX 03 FC data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

发送端 ：STX 04 FB data[1024] CRC CRC>>>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

发送端 ：SOH 05 FA data[100] 1A[28] CRC CRC>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

发送端 ：EOT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< NAK

发送端 ：EOT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< C

发送端 ：SOH 00 FF NUL[128] CRC CRC >>>>>>>>>>>>>>>>>>>>>>

接收端：<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK



#### Protobuf协议：

安装**protoc**工具：

**ubuntu**：apt install  protobuf-compiler protoc-gen-go protoc-gen-go-grpc

**windows**：https://github.com/protocolbuffers/protobuf/releases



#### Modbus协议：

Modbus协议是一种应用层报文传输协议，协议本身并没有定义物理层，定义了控制器能够认识和使用的消息结构，不管它们是经过何种网络进行通信的。

Modbus是**一主多从**的通信协议，主机在同一时间内只能向一个从机发送请求，总线上每次只有一个数据进行传输，即主机发送,从机应答，主机不发送，总线上就没有数据通信。