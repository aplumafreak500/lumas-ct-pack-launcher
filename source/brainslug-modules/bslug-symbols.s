	.section .rodata
	.global cache_xml
cache_xml:
	.incbin "brainslug-modules/symbols/cache.xml"
	.align 4, 0
	.global ctype_xml
ctype_xml:
	.incbin "brainslug-modules/symbols/ctype.xml"
	.align 4, 0
	.global dwc_xml
dwc_xml:
	.incbin "brainslug-modules/symbols/dwc.xml"
	.align 4, 0
	.global GXFrameBuf_xml
GXFrameBuf_xml:
	.incbin "brainslug-modules/symbols/GXFrameBuf.xml"
	.align 4, 0
	.global GXTransform_xml
GXTransform_xml:
	.incbin "brainslug-modules/symbols/GXTransform.xml"
	.align 4, 0
	.global hook_xml
hook_xml:
	.incbin "brainslug-modules/symbols/hook.xml"
	.align 4, 0
	.global ipc_xml
ipc_xml:
	.incbin "brainslug-modules/symbols/ipc.xml"
	.align 4, 0
	.global md5_xml
md5_xml:
	.incbin "brainslug-modules/symbols/md5.xml"
	.align 4, 0
	.global OSMutex_xml
OSMutex_xml:
	.incbin "brainslug-modules/symbols/OSMutex.xml"
	.align 4, 0
	.global OSThread_xml
OSThread_xml:
	.incbin "brainslug-modules/symbols/OSThread.xml"
	.align 4, 0
	.global OSTime_xml
OSTime_xml:
	.incbin "brainslug-modules/symbols/OSTime.xml"
	.align 4, 0
	.global OSUtf_xml
OSUtf_xml:
	.incbin "brainslug-modules/symbols/OSUtf.xml"
	.align 4, 0
	.global stdio_xml
stdio_xml:
	.incbin "brainslug-modules/symbols/stdio.xml"
	.align 4, 0
	.global string_xml
string_xml:
	.incbin "brainslug-modules/symbols/string.xml"
	.align 4, 0
	.global vi_xml
vi_xml:
	.incbin "brainslug-modules/symbols/vi.xml"
	.global symbols_end
symbols_end:
	.align 4, 0
