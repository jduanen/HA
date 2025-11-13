import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    ICON_MOTION_SENSOR,
)

CODEOWNERS = ["@jduanen"]
DEPENDENCIES = ["i2c"]

person_sensor_ns = cg.esphome_ns.namespace("person_sensor")
PersonSensor = person_sensor_ns.class_(
    "PersonSensor", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        PersonSensor,
        icon=ICON_MOTION_SENSOR,
        accuracy_decimals=4)
    .extend (
        i2c.i2c_device_schema(0x62)
    )
    .extend (
       cv.polling_component_schema("60s")
    )
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
