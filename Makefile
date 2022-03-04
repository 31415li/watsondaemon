RUTOS_ROOT_DIR=~/proj/rutos

iotp: install
	make -C $(RUTOS_ROOT_DIR) package/libibm-iotp/compile

daemon: install
	make -C $(RUTOS_ROOT_DIR) package/watson-daemon/compile

install:
	cp -rv libibm-iotp $(RUTOS_ROOT_DIR)/package
	cp -rv watson-daemon $(RUTOS_ROOT_DIR)/package