RUTOS_ROOT_DIR=~/proj/rutos

iotp: install-iotp
	make -C $(RUTOS_ROOT_DIR) package/libibm-iotp/compile

daemon: install-daemon
	make -C $(RUTOS_ROOT_DIR) package/watson-daemon/compile

install-iotp:
	cp -rv libibm-iotp $(RUTOS_ROOT_DIR)/package
	
install-daemon:
	cp -rv watson-daemon $(RUTOS_ROOT_DIR)/package