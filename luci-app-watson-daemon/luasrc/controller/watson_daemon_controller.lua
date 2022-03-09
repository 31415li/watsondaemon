module("luci.controller.watson_daemon_controller", package.seeall)

function index()
    entry({"admin", "services", "watson"}, cbi("watson_daemon_model"), "Watson IoTP", 0)
end 