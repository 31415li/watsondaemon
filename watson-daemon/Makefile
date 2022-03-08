include $(TOPDIR)/rules.mk

PKG_NAME:=watson-daemon
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/watson-daemon
	CATEGORY:=Base system
	TITLE:=watson-daemon
	DEPENDS:=+libuci +libibm-iotp +libubus +libubox +libblobmsg-json
endef

define Package/watson-daemon/description
	Daemon for reporting router status to IBM Watson IoT Platform
endef

define Package/watson-daemon/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/watson-daemon $(1)/usr/bin/
	$(INSTALL_BIN) ./files/watson-daemon.init $(1)/etc/init.d
	$(INSTALL_CONF) ./files/config $(1)/etc/config/watson-daemon
endef

$(eval $(call BuildPackage,watson-daemon))