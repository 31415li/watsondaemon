RUTOS_ROOT_DIR:=~/proj/rutos
IOTP_ROOT_DIR:= $(RUTOS_ROOT_DIR)/build_dir/target-arm_cortex-a7+neon-vfpv4_musl_eabi/libibm-iotp-1.0.0/ipkg-arm_cortex-a7_neon-vfpv4/libibm-iotp
DAEMON_ROOT_DIR:=$(RUTOS_ROOT_DIR)/build_dir/target-arm_cortex-a7+neon-vfpv4_musl_eabi/watson-daemon-1.0.0/ipkg-arm_cortex-a7_neon-vfpv4/watson-daemon
LUCI_ROOT_DIR:= $(RUTOS_ROOT_DIR)/build_dir/target-arm_cortex-a7+neon-vfpv4_musl_eabi/luci-app-watson-daemon/ipkg-all/luci-app-watson-daemon
CPFLAGS:=--preserve=timestamps

load-iotp:
	cp $(CPFLAGS) -rv libibm-iotp $(RUTOS_ROOT_DIR)/package
	
load-daemon:
	cp $(CPFLAGS) -rv watson-daemon $(RUTOS_ROOT_DIR)/package

load-luci:
	cp $(CPFLAGS) -rv luci-app-watson-daemon $(RUTOS_ROOT_DIR)/package

build-iotp: load-iotp
	make -C $(RUTOS_ROOT_DIR) package/libibm-iotp/compile

build-daemon: load-daemon
	make -C $(RUTOS_ROOT_DIR) package/watson-daemon/compile

build-luci: load-luci
	make -C $(RUTOS_ROOT_DIR) package/luci-app-watson-daemon/compile

upload-iotp: build-iotp
	scp -r $(IOTP_ROOT_DIR)/usr root@192.168.1.1:/

upload-daemon: build-daemon
	scp -r $(DAEMON_ROOT_DIR)/usr $(DAEMON_ROOT_DIR)/etc root@192.168.1.1:/

upload-luci: build-luci
	scp -r $(LUCI_ROOT_DIR)/usr root@192.168.1.1:/

clean-iotp:
	make -C $(RUTOS_ROOT_DIR) package/libibm-iotp/clean

clean-daemon:
	make -C $(RUTOS_ROOT_DIR) package/watson-daemon/clean

clean-luci:
	make -C $(RUTOS_ROOT_DIR) package/luci-app-watson-daemon/clean

format-daemon:
	clang-format -i --style="{BasedOnStyle: llvm, IndentWidth: 4}" watson-daemon/src/*