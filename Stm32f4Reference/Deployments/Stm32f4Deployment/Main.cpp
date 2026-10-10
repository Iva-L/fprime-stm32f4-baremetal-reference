// ======================================================================
// \title  Main.cpp
// \brief Bare-metal cyclic executive entry point for the STM32 deployment.
// ======================================================================
// Used to access topology functions and component instances
#include <Stm32f4Project/Deployments/Stm32f4Deployment/Top/Stm32f4DeploymentTopology.hpp>
#include <Stm32f4Project/Deployments/Stm32f4Deployment/Top/Stm32f4DeploymentTopologyAc.hpp>

#include <fprime-stm32/Allocator/BootstrapAllocator.hpp>
#include <Os/Os.hpp>
#include <fprime-baremetal/Os/TaskRunner/TaskRunner.hpp>
#include <fprime-stm32/Drv/STM32Timer/STM32Timer.hpp>
#include <fprime-stm32/Drv/STM32UartDriver/Stm32UartDriver.hpp>
#include <main.h>
#include <stm32f4_clock.h>
#include <tim2_clock.h>

int main() {
    
    HAL_Init();

    // Initialize the system clocks, including the HSE->PLL1 clock configuration.
    FprimeStm32_ClockInit();
    Stm32_Tim2ClockInit();

    Os::init();
    Os::Baremetal::TaskRunner& taskRunner = Os::Baremetal::TaskRunner::getSingleton();

    Deployments::TopologyState inputs = {};
    Deployments::setupTopology(inputs);
    Stm32::lockBootstrapAllocator();

    while (true) {
        // Run a cooperative state machine step for each active registered component.
        taskRunner.runAll();

        // Poll the TIM2 CH2 hardware tick source and the UART driver.
        Deployments::timer.poll();
        Deployments::comDriver.poll();
    }
}
