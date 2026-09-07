SUMMARY = "Cat Couch Guardian virtual-first QEMU image"
DESCRIPTION = "Minimal Poky image extended only with the Cat Couch Guardian application."

require recipes-core/images/core-image-minimal.bb

IMAGE_INSTALL:append = " cat-guardian"
