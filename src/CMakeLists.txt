add_executable(${ProjectName}
    main.c
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}/lib/OLED
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
        hardware_i2c
    myOLED
        pico_cyw43_arch_none
    FreeRTOS-Kernel-Heap3
    )

pico_add_extra_outputs(${ProjectName})
