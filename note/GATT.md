#### 一、引言

现在低功耗蓝牙（BLE）连接都是建立在 GATT (Generic Attribute Profile) 协议之上。GATT 是一个在蓝牙连接之上的发送和接收很短的数据段的通用规范，这些很短的数据段被称为属性（Attribute）。

#### 二、常见名词

1、profile

profile可以理解为一种规范，一个标准的通信协议，它存在于从机中。每个profile中会包含多个service，每个service代表从机的一种能力。

2、service

service可以理解为一个服务，在ble从机中，通过有多个服务，例如电量信息服务、系统信息服务等，每个service中又包含多个characteristic特征值。每个具体的characteristic特征值才是ble通信的主题。

3、characteristic

characteristic特征值，ble主从机的通信均是通过characteristic来实现，可以理解为一个标签，通过这个标签可以获取或者写入想要的内容。

4、UUID

UUID，统一识别码，我们刚才提到的service和characteristic，都需要一个唯一的uuid来标识整理一下，每个从机都会有一个叫做profile的东西存在，不管是上面的自定义的simple profile，还是标准的防丢器profile，他们都是由一些列service组成，然后每个service又包含了多个characteristic，主机和从机之间的通信，均是通过characteristic来实现。

#### 三、GAP

 GAP（Generic Access Profile），用来控制设备连接和广播。GAP 使你的设备被其他设备可见，并决定了你的设备是否可以或者怎样与合同设备进行交互。

##### 3.1 设备角色

外围设备：一般是小型简单的低功耗设备，用来提供数据，并连接到中心设备。

中心设备：用来连接其他外围设备，例如手机。

##### 3.2 广播数据

在 GAP 中外围设备通过两种方式向外广播数据： Advertising Data Payload（广播数据）和 Scan Response Data Payload（扫描回复），每种数据最长可以包含 31 byte。这里广播数据是必需的，因为外设必需不停的向外广播，让中心设备知道它的存在。扫描回复是可选的，中心设备可以向外设请求扫描回复，这里包含一些设备额外的信息，例如设备的名字。

##### 3.2 广播流程

![image-20231117092231486](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20231117092231486.png)

外围设备会设定一个广播间隔，每个广播间隔中，它会重新发送自己的广播数据。广播间隔越长，越省电，同时也不太容易扫描到。当中心设备发送扫描请求时，外围设备会回复扫描回复。

##### 3.4 广播的网络拓扑结构

大部分情况下，外设通过广播自己来让中心设备发现自己，并建立 GATT 连接，从而进行更多的数据交换。也有些情况是不需要连接的，只要外设广播自己的数据即可。用这种方式主要目的是让外围设备，把自己的信息发送给多个中心设备。

![image-20231117092729935](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20231117092729935.png)

#### 四、GATT

GATT 的全名是 Generic Attribute Profile（姑且翻译成：普通属性协议），它定义两个 BLE 设备通过叫做 Service 和 Characteristic 的东西进行通信。GATT 就是使用了 ATT（Attribute Protocol）协议，ATT 协议把 Service，Characteristic遗迹对应的数据保存在一个查找表中，次查找表使用 16 bit ID 作为每一项的索引。

一旦两个设备建立起了连接，GATT 就开始起作用了，这也意味着，你必需完成前面的 GAP 协议。这里需要说明的是，GATT 连接，必需先经过 GAP 协议。

GATT 连接需要特别注意的是：GATT 连接是独占的。也就是一个 BLE 外设同时只能被一个中心设备连接。一旦外设被连接，它就会马上停止广播，这样它就对其他设备不可见了。当设备断开，它又开始广播。

##### 4.1 GATT网络拓扑结构

一个外设只能连接一个中心设备，而一个中心设备可以连接多个外设。Connected Topology一旦建立起了连接，通信就是双向的了，对比前面的 GAP 广播的网络拓扑，GAP 通信是单向的。如果你要让两个设备外设能通信，就只能通过中心设备中转。

![image-20231117092817027](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20231117092817027.png)

##### 4.2 GATT通信事务

GATT 通信的双方是 C/S 关系。**外设**作为 **GATT 服务端**（Server），它维持了 ATT 的查找表以及 service 和 characteristic 的定义。**中心设备**是 **GATT 客户端**（Client），它向 Server 发起请求。需要注意的是，所有的通信事件，都是由**客户端**（也叫主设备，Master）**发起**，**接收服务端**（也叫从设备，Slave）**响应**。

![image-20231117093529388](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20231117093529388.png)

可以看到，都是由中心设备（主设备）发起，外围设备（从设备）响应。

##### 4.3 GATT结构

GATT事务是建立在嵌套的Profiles， Services和Characteristics 之上的。Profile说明图如下：

![image-20231117100341054](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20231117100341054.png)

- Profile并不是实际存在，只是被预先定义的Services的集合
- Service是把数据分成一个个独立的逻辑项，它包含一个或多个Characteristics。每个Service都有一个UUID唯一标识，UUID有16bit和128bit的，16bit是通过官方认证，而128bit是自定义的。
- Characteristic是GATT最小的逻辑数据单元，也可能包含一组关联的数据，如加速度计XYZ轴信息。与 Service 类似，每个 Characteristic 用 16 bit 或者 128 bit 的 UUID 唯一标识。

多个Characteristic组成一个Service，一个Service是一个独立的服务单元，或者说service是一个基本的BLE应用。如果某个service是一个蓝牙联盟定义的标准服务，也可以称其为profile，比如HID/心率计/体温计/血糖仪等，都是标准蓝牙服务，因此都有相应的profile规格书。

##### 4.4 Characteristic

一个characteristic包含三种条目：**characteristic声明**（declaration），**characteristic值**（value）以及**characteristic描述符**（descriptor）（可以有多个描述符）：

- Characteristic declaration就是每个 Characteristic 的分界符，解析时一旦遇到Characteristic declaration，就可以认为接下来又是一个新的Characteristic了，同时Characteristic declaration还将包含value的读写属性等。

- Characteristic value就是数据的值了，这个比较好理解就不再说了。

- Characteristic descriptor就是数据的额外信息，比如温度的单位是什么，数据是用小数表示还是百分比表示等之类的数据描述信息。CCCD是一种特殊的Characteristic descriptor，当Characteristic具有notify或者indicate操作功能时，那么必须为其添加相应CCCD，以方便client来使能或者禁止notify或者indicate功能。

实际上characteristic declaration，characteristic value、characteristic descriptor，都是用attribute来表示。

| Attribute Handle | Attribute Type | Attribute Value | Attribute **Permissions** |
| ---------------- | -------------- | --------------- | ------------------------- |

**1.Attribute handle**

Attribute句柄，2字节。Client要访问Server的Attribute，都是通过这个句柄来访问的，也就是说ATT PDU一般都包含handle的值。用户在软件代码添加characteristic的时候，系统会自动按顺序地为相关attribute生成句柄。

**2.Attribute type**

Attribute类型，2字节或16字节。在BLE中我们使用UUID来定义数据的类型，UUID是128 bit的，所以我们有足够的UUID来表达万事万物。其中有一个UUID非常特殊，它被蓝牙联盟采用为官方UUID：0000xxxx-0000-1000-8000-00805F9B34FB

由于这个UUID众所周知，蓝牙联盟将自己定义的attribute或者数据只用16bit UUID来表示，比如0x1234，其实它也是128bit，完整表示为：

　　00001234-0000-1000-8000-00805F9B34FB = 16 bit UUID 0x1234

　Attribute type一般是由service和characteristic规格来定义，站在蓝牙协议栈角度来看，ATT层定义了一个通信的基本框架，数据的基本结构，以及通信的指令，而GATT层就是前文所述的service和characteristic，GATT层用来赋予每个数据一个具体的内涵，让数据变得有结构和意义。换句话说，没有GATT层，低功耗蓝牙也可以通信起来，但会产生兼容性问题以及通信的低效率。

**3.Attribute value**

就是数据真正的值，0到512字节长。

**4.Attribute permissions**

Attribute的权限属性，权限属性不会直接在空中包中体现，而是隐含在ATT命令的操作结果中。目前主要有如下四种权限属性：

- Open，直接可以读或者写；
- No Access，禁止读或者写；Authentication，需要配对才能读或者写；
- Authorization，跟open一样，不过server返回attribute的值之前需要应用先授权，也就是说应用可以在回调函数里面去修改读或者写的原始值；
- Signed，签名后才能读或者写

每个Characteristic都带有Properties，这个 Properties 为蓝牙特征值的读写权限：Read、Write、Notify、Indicate。

#### 五、ESP32 gatt_server例程

例程位置 ./esp-idf/examples/bluetooth/bluedroid/ble/gatt_server

##### 5.1 main分析

1. 初始化蓝牙外设

```
esp_err_t ret;
// Initialize NVS.
ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	ESP_ERROR_CHECK(nvs_flash_erase());
	ret = nvs_flash_init();
}
ESP_ERROR_CHECK( ret );

ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
ret = esp_bt_controller_init(&bt_cfg);
if (ret) {
	ESP_LOGE(GATTS_TAG, "%s initialize controller failed: %s\n", __func__, 	esp_err_to_name(ret));
	return;
}

ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
if (ret) {
	ESP_LOGE(GATTS_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
	return;
}
ret = esp_bluedroid_init();
if (ret) {
	ESP_LOGE(GATTS_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
	return;
}
ret = esp_bluedroid_enable();
if (ret) {
	ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
	return;
}
```

2. 注册GAP时间回调

```
ret = esp_ble_gap_register_callback(gap_event_handler);
if (ret){
	ESP_LOGE(GATTS_TAG, "gap register error, error code = %x", ret);
	return;
}
```

3. 注册GATT事件回调

```
ret = esp_ble_gatts_register_callback(gatts_event_handler);
if (ret){
	ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
	return;
}
```

4. 注册profile

```
ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
if (ret){
	ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
	return;
}
ret = esp_ble_gatts_app_register(PROFILE_B_APP_ID);
if (ret){
	ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
	return;
}
```

5. 设置mtu

MTU: MAXIMUM TRANSMISSION UNIT 最大传输单元， 指在一个PDU 能够传输的最大数据量(多少字节可以一次性传输到对方)。 

PDU：Protocol Data Unit  协议数据单元, 在一个传输单元中的有效传输数据。

```
esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
if (local_mtu_ret){
	ESP_LOGE(GATTS_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
}
```

##### 5.2 回调函数

1、GATT回调 **gatts_event_handler**

```
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    /* 检查是否有GATT注册事件，触发ESP_GATTS_REG_EVT时为每个profile的gatts_if注册 */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
     /* 调用对应profile的事件回调处理事务 */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}
```

2、GAP事件回调 **gap_event_handler**

```
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
#ifdef CONFIG_SET_RAW_ADV_DATA
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done==0){
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done==0){
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#else
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:		/* 广播数据设置完成事件 */
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0){
        	/* 开始广播 */
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:	/* 广播扫描数据设置完成事件 */
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0){
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
#endif
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:	//开始广播事件标志
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "Advertising start failed\n");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:		//停止广播事件标志
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "Advertising stop failed\n");
        } else {
            ESP_LOGI(GATTS_TAG, "Stop adv successfully\n");
        }
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:	// 设备连接事件,可获取当前连接的设备信息
         ESP_LOGI(GATTS_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}
```

3、 profile回调函数 **gatts_profile_a_event_handler**

```
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
    /*
    展示了一个Service的创建
    GATT注册事件，添加 service的基本信息，设置BLE名称
    */
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status, param->reg.app_id);
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_TEST_A;

        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(TEST_DEVICE_NAME);
        if (set_dev_name_ret){
            ESP_LOGE(GATTS_TAG, "set device name failed, error code = %x", set_dev_name_ret);
        }
#ifdef CONFIG_SET_RAW_ADV_DATA 
        esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
        if (raw_adv_ret){
            ESP_LOGE(GATTS_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
        }
        adv_config_done |= adv_config_flag;
        esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
        if (raw_scan_ret){
            ESP_LOGE(GATTS_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
        }
        adv_config_done |= scan_rsp_config_flag;
#else
        //config adv data
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret){
            ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
        }
        adv_config_done |= adv_config_flag;
        //config scan response data
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret){
            ESP_LOGE(GATTS_TAG, "config scan response data failed, error code = %x", ret);
        }
        adv_config_done |= scan_rsp_config_flag;

#endif
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, GATTS_NUM_HANDLE_TEST_A);
        break;
    case ESP_GATTS_READ_EVT: { //GATT读取事件，手机读取开发板的数据
        ESP_LOGI(GATTS_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0xde;
        rsp.attr_value.value[1] = 0xed;
        rsp.attr_value.value[2] = 0xbe;
        rsp.attr_value.value[3] = 0xef;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_WRITE_EVT: { //GATT写事件，手机给开发板的发送数据，不需要回复
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep){
            ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
            if (gl_profile_tab[PROFILE_A_APP_ID].descr_handle == param->write.handle && param->write.len == 2){
                uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                if (descr_value == 0x0001){
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY){
                        ESP_LOGI(GATTS_TAG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i%0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                sizeof(notify_data), notify_data, false);
                    }
                }else if (descr_value == 0x0002){
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_INDICATE){
                        ESP_LOGI(GATTS_TAG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i%0xff;
                        }
                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                sizeof(indicate_data), indicate_data, true);
                    }
                }
                else if (descr_value == 0x0000){
                    ESP_LOGI(GATTS_TAG, "notify/indicate disable ");
                }else{
                    ESP_LOGE(GATTS_TAG, "unknown descr value");
                    esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
                }

            }
        }
        example_write_event_env(gatts_if, &a_prepare_write_env, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT: //GATT写事件，手机给开发板的发送数据，需要回复
        ESP_LOGI(GATTS_TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&a_prepare_write_env, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    //创建 GATT事件,基本参数的设置，将Characteristic加到service中，完成触发下面事件
    case ESP_GATTS_CREATE_EVT: 
        ESP_LOGI(GATTS_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_TEST_A;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);
        a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        a_property,
                                                        &gatts_demo_char1_val, NULL);
        if (add_char_ret){
            ESP_LOGE(GATTS_TAG, "add char failed, error code =%x",add_char_ret);
        }
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    //添加Characteristic事件，添加Characteristic的Descriptor，完成触发下面事件
    case ESP_GATTS_ADD_CHAR_EVT: { 
        uint16_t length = 0;
        const uint8_t *prf_char;

        ESP_LOGI(GATTS_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle,  &length, &prf_char);
        if (get_attr_ret == ESP_FAIL){
            ESP_LOGE(GATTS_TAG, "ILLEGAL HANDLE");
        }

        ESP_LOGI(GATTS_TAG, "the gatts demo char length = %x\n", length);
        for(int i = 0; i < length; i++){
            ESP_LOGI(GATTS_TAG, "prf_char[%x] =%x\n",i,prf_char[i]);
        }
        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (add_descr_ret){
            ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:// 添加描述事件
        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT: { // GATT 连接事件
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT://断开连接事件
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GATTS_CONF_EVT: //GATT配置事件
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}
```

GATT事件流程：

**初始化到未连接过程**：注册->创建->启动->添加特征->添加特征描述

`ESP_GATTS_REG_EVT`—>

`ESP_GATTS_CREATE_EVT`—>

`ESP_GATTS_START_EVT`—>

`ESP_GATTS_START_EVT`—>

`ESP_GATTS_ADD_CHAR_EVT`—>

`ESP_GATTS_ADD_CHAR_DESCR_EVT`

流程说明：

在 Demo 的**ESP_GATTS_REG_EVT**事件中，调用**esp_ble_gap_set_device_name(char *)**来设置蓝牙设备名字；调用**esp_ble_gap_config_adv_data()**来配置广播数据；

最后调用**esp_ble_gatts_create_service()**指定 gatts_if 和 service_id 来创建服务（实际调用 **btc_transfer_context()** 来完成服务的创建和调用回调函数）。

服务创建完成就会触发回调函数向profile报告状态和服务ID。Service_id对于后面添加included serivces 和 characteristics 和 descriptor 都要用到。触发**ESP_GATTS_CREATE_EVT**事件

在Demo的**ESP_GATTS_CREATE_EVT**中调用**esp_ble_gatts_start_service(uint16_t service_handle)**来启动服务；

再调用 **esp_ble_gatts_add_char()** 来添加特性(特征的UUID， 特征值描述符属性权限， 特征属性、特征值、属性响应控制字节)。

触发**ESP_GATTS_START_EVT**和**ESP_GATTS_ADD_CHAR_EVT**事件，在**ESP_GATTS_ADD_CHAR_EVT**事件中，获取特征值调用**esp_err_tesp_ble_gatts_add_char_descr()**来添加特征描述符。

**在连接之后**

`CONNECT_EVT`—>

`ESP_GATTS_MTU_EVT`—>

`GATT_WRITE_EVT`—>

`ESP_GATTS_CONF_EVT`—>

`GATT_READ_EVT`
