map = Map("watson-daemon")

section = map:section(NamedSection, "config", "section", "Watson IoTP Configuration")

flag = section:option(Flag, "enable", "Enable", "Enable service")

orgId = section:option(Value, "orgId", "Organization ID", "IoTP organization ID")
orgId.datatype = "string"

typeId = section:option(Value, "typeId", "Device type", "IoTP device type ID")
typeId.datatype = "string"

deviceId = section:option(Value, "deviceId", "Device ID", "IoTP device ID")
deviceId.datatype = "string"

token = section:option(Value, "token", "Auth token", "IoTP device auth token")
token.datatype = "string"

return map