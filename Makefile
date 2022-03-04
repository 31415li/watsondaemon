RUTOS_ROOT_DIR=~/proj/rutos

iotp: install
	make -C $(RUTOS_ROOT_DIR) package/libibm-iotp/compile

install:
	cp -rv libibm-iotp $(RUTOS_ROOT_DIR)/package