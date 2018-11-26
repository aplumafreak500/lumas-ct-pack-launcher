	.section .rodata
	.global console_gx_elf
console_gx_elf:
	.incbin "brainslug-modules/modules/console-gx/bin/console-gx.mod"
	.size console_gx_elf, . - console_gx_elf
	.global console_sd_elf
console_sd_elf:
	.incbin "brainslug-modules/modules/console-sd/bin/console-sd.mod"
	.size console_sd_elf, . - console_sd_elf
	.global gecko_elf
gecko_elf:
	.incbin "brainslug-modules/modules/gecko/bin/gecko.mod"
	.size gecko_elf, . - gecko_elf
	.global https_elf
https_elf:
	.incbin "brainslug-modules/modules/https/bin/https.mod"
	.size https_elf, . - https_elf
	.global libfat_elf
libfat_elf:
	.incbin "brainslug-modules/modules/libfat/bin/libfat.mod"
	.size libfat_elf, . - libfat_elf
	.global libfat_sd_elf
libfat_sd_elf:
	.incbin "brainslug-modules/modules/libfat-sd/bin/libfat-sd.mod"
	.size libfat_sd_elf, . - libfat_sd_elf
	.global libsd_elf
libsd_elf:
	.incbin "brainslug-modules/modules/libsd/bin/libsd.mod"
	.size libsd_elf, . - libsd_elf
	.global modules_end
modules_end:
	.align 4, 0
