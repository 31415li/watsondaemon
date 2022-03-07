#include <iotp_device.h>

int main(void) {
    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;
    
    IoTPConfig_create(&config, "/etc/config/watson-daemon.yaml");
    IoTPDevice_create(&device, config);
    IoTPDevice_connect(device);
    
    // MQTTProperties *properties = (MQTTProperties *)malloc(sizeof(MQTTProperties));
    // MQTTProperty   property;
    // property.identifier = MQTTPROPERTY_CODE_USER_PROPERTY;
    // property.value.data.data = "user defined property";
    // property.value.data.len = (int)strlen(property.value.data.data);
    // property.value.value.data = "user defined property value";
    // property.value.value.len = (int)strlen(property.value.value.data);
    // MQTTProperties_add(properties, &property);

    char *data = "{\"d\" : {\"SensorID\": \"Test\", \"Reading\": 7 }}";
    IOTPRC err = IoTPDevice_sendEvent(device, "status", data, "json", QoS0, NULL);
    printf("%d\n", err);

    IoTPDevice_disconnect(device);
    IoTPDevice_destroy(device);
    IoTPConfig_clear(config);

    return 0;
}