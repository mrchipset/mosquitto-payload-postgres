<div align="center">

# mosquitto-payload-postgres
[English](./README.md) / 简体中文

</div>

用于Mosquitto的PostgreSQL消息持久化插件
Mosquitto payload postgres是[Mosquitto](https://mosquitto.org/) MQTT中继器的一个消息持久性插件，它是一个众所周知的开源MQTT中继器。它将把 mqtt 客户端发布的消息保存到PostgreSQL中。

### 方法
将整个软件仓库复制到`mosquitto`插件源文件夹。如果一切顺利，只需在 `mosquitto-payload-postgres` 源文件夹中输入 `make`。

仓库中还提供了一个`payload.conf`配置文件。
将插件 `.so` 和 `.conf` 文件复制到 mosquitto 配置文件夹。然后在 mosquitto 主配置文件中添加配置设置。
```
include_dir /etc/mosquitto/conf.d
```
