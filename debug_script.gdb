target remote :3333
file /home/mjs/CROSSCON/TEEsinMCUs/CROSSCON-Hypervisor/bin/lpc55s69/example/crossconhyp.elf
add-symbol-file /home/mjs/CROSSCON/TEEsinMCUs/freertos-over-bao/build/lpc55s69/freertos.elf
set $pc=_reset_handler
b main
b vRestoreContextOfFirstTask
b SysTick_Handler
b _systick_handler
c
