#####
# 'Stm32f4Deployment' Deployment:
#
# This registers the 'Stm32f4Deployment' deployment to the build system. 
# Custom components that have not been added at the project-level should be added to 
# the list below.
#
#####

restrict_platforms(stm32f4)

###
# Topology and Components
###

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")

# Add custom components to this specific deployment here
# add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MyComponent/")

register_fprime_deployment(
    Stm32f4Deployment
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
        "${FPRIME_STM32_STARTUP_SOURCE}"
        "${FPRIME_STM32_IT_SOURCE}"
    DEPENDS
        Stm32f4Deployment_Top
        FprimeStm32
        FprimeStm32Config
        FprimeStm32Allocator
        Os_Baremetal_OverrideNewDelete
)

target_include_directories(Stm32f4Deployment PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/.."
    "${CMAKE_CURRENT_BINARY_DIR}/.."
)

target_link_options(Stm32f4Deployment PRIVATE
    "-T${FPRIME_STM32_LINKER_SCRIPT}"
    "-Wl,--gc-sections"
    "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/Stm32f4Deployment.map"
    "-Wl,--undefined=_Znwj"
    "-Wl,--undefined=_Znaj"
    "-Wl,--undefined=_Znwjl"
    "-Wl,--undefined=_Znajl"
    "-Wl,--undefined=_ZnwjRKSt9nothrow_t"
    "-Wl,--undefined=_ZnajRKSt9nothrow_t"
    "-Wl,--undefined=_ZdlPv"
    "-Wl,--undefined=_ZdaPv"
    "-Wl,--undefined=_ZdlPvl"
    "-Wl,--undefined=_ZdaPvl"
    "-Wl,--undefined=_ZdlPvj"
    "-Wl,--undefined=_ZdaPvj"
    "--specs=nosys.specs"
)