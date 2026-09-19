SUMMARY = "Cat Couch Guardian educational virtual-first proof of concept"
DESCRIPTION = "Educational hardware-independent Cat Couch Guardian core with simulated motion input, a semantic deterrent-request boundary, and deterministic evidence output."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3cd96fedaeaeb7aee5da5f747c337ea0"

SRC_URI = "file://cat-guardian-0.1.tar.gz"
SRC_URI[sha256sum] = "b7fcd728404145305b8bb1a198441b1827a2ed83547736031e8d4381a3e964d5"

EXTRA_OEMAKE = " \
    -C ${S} \
    BUILD_DIR=${B} \
    'CC=${CC}' \
    'CPPFLAGS=${CPPFLAGS}' \
    'CFLAGS=${CFLAGS}' \
    'LDFLAGS=${LDFLAGS}' \
"

inherit systemd

SYSTEMD_SERVICE:${PN} = "cat-guardian.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_compile() {
    oe_runmake
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/cat-guardian \
        ${D}${bindir}/cat-guardian

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 \
        ${S}/packaging/systemd/cat-guardian.service \
        ${D}${systemd_system_unitdir}/cat-guardian.service
}

FILES:${PN} += " \
    ${bindir}/cat-guardian \
    ${systemd_system_unitdir}/cat-guardian.service \
"
