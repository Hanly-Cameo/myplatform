# sonic innovium ONE image installer

SONIC_ONE_IMAGE = sonic-innovium.bin
$(SONIC_ONE_IMAGE)_MACHINE = innovium
$(SONIC_ONE_IMAGE)_INSTALLS += $(SYSTEMD_SONIC_GENERATOR)
$(SONIC_ONE_IMAGE)_IMAGE_TYPE = onie
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(CEL_MIDSTONE_200I_PLATFORM_MODULE)
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(DELTA_PLATFORM_MODULE)
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(CAMEO_ESC601_32Q_PLATFORM_MODULE) $(CAMEO_ESC600_128Q_PLATFORM_MODULE) $(CAMEO_ESQC610_56SQ_PLATFORM_MODULE) $(CAMEO_ESC602_32Q_PLATFORM_MODULE) $(CAMEO_ESCC601_32Q_PLATFORM_MODULE)
$(SONIC_ONE_IMAGE)_INSTALLS += $(INVM_DRV)
$(SONIC_ONE_IMAGE)_DOCKERS  += $(SONIC_INSTALL_DOCKER_IMAGES)
SONIC_INSTALLERS += $(SONIC_ONE_IMAGE)