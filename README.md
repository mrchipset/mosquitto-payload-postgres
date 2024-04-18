<div align="center">

# mosquitto-payload-postgres
English / [简体中文](./README_CN.md)

</div>

Message persistance to postgres for mosquitto

Mosquitto payload postgres is an message persistance plugin for the [Mosquitto](https://mosquitto.org/) MQTT broker, a well knwon opensource MQTT broker.
It will save the message to PostgreSQL published by mqtt clients.


### How-to
Copy the whole repository to `mosquitto` plugin source folder. If everthing goes well, just type `make` in the `mosquitto-payload-postgres` source folder.

Here a `payload.conf` configuration file is also provided.
Copy the plugin `.so` and `.conf` file to mosquitto configuration folder. Then add configuration setup to mosquitto main configuration file.
```
include_dir /etc/mosquitto/conf.d
```