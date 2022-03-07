#include <stdio.h>
#include <iotp_device.h>
#include <uci.h>

IoTPConfig *load_iotp_config(void) {
    const char *const config_name = "watson-daemon";
    struct uci_context *context = uci_alloc_context();
    struct uci_package *package;

    uci_load(context, config_name, &package);

    struct uci_section *section = uci_lookup_section(context, package, "global");

    char *org_id = uci_lookup_option_string(context, section, "orgId");
    char *type_id = uci_lookup_option_string(context, section, "typeId");
    char *device_id = uci_lookup_option_string(context, section, "deviceId");
    char *token = uci_lookup_option_string(context, section, "token");
    
    IoTPConfig *config = NULL;
    IoTPConfig_create(&config, NULL);

    IoTPConfig_setProperty(config, "identity.orgId", org_id);
    IoTPConfig_setProperty(config, "identity.typeId", type_id);
    IoTPConfig_setProperty(config, "identity.deviceId", device_id);
    IoTPConfig_setProperty(config, "auth.token", token);

    uci_free_context(context);

    return config;
}

int main(void) {
    IoTPConfig *config = load_iotp_config();
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
    IOTPRC err = IoTPDevice_sendEvent(device, "status", data, "json", QoS1, NULL);
    printf("%d\n", err);

    IoTPDevice_disconnect(device);
    IoTPDevice_destroy(device);
    IoTPConfig_clear(config);

    return 0;
}