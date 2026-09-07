SUMMARY = "Cat Couch Guardian virtual-first proof of concept"
DESCRIPTION = "Hardware-independent Cat Couch Guardian core with simulated motion input and deterministic evidence output."

# Private development PoC: no redistribution license is granted at this stage.
LICENSE = "CLOSED"

SRC_URI = "file://cat-guardian-0.1.tar.gz"
SRC_URI[sha256sum] = "5da43633703e3be98de949a6a0214d90065a2cd23a9517df6ad2db47daafd0f7"

inherit systemd

SYSTEMD_SERVICE:${PN} = "cat-guardian.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_compile() {
    oe_runmake \
        -C ${S} \
        BUILD_DIR="${B}" \
        CC="${CC}" \
        CPPFLAGS="${CPPFLAGS}" \
        CFLAGS="${CFLAGS}" \
        LDFLAGS="${LDFLAGS}" \
        all
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
