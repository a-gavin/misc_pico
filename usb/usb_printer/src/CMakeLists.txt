target_sources(usb_printer
    PRIVATE
        usb_printer.c
        myAssert.c
)
target_include_directories(usb_printer
    PRIVATE
        include
)

target_include_directories(freertos_usb_printer PUBLIC
    PUBLIC
        include
)        