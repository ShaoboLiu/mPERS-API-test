#CC=arm-elf-gcc
CC=$1

#Defaults to 'gcc' if no compiler is given
if [ "$CC" = "" ]; then
CC=gcc
fi

$CC -lpthread -I . -o halo_udp_dbg_ublox debug_app_linux/client_mode.c \
debug_app_linux/client_test_impl.c \
debug_app_linux/client_tests.c \
debug_app_linux/main.c \
debug_app_linux/server_mode.c \
debug_app_linux/server_tests.c \
General/genericStaticArrayBuffer.c \
General/str_utils.c \
General/GenericStaticArrayQueue.c \
Test/tests.c \
mpers/DynamicVitalsMsg.c \
mpers/HaloMsgHelperFunctions.c \
UdpLib/halo_udp_tx_mgmt.c \
UdpLib/halo_udp_stats.c \
UdpLib/tx_mgmt_buffer.c \
UdpLib/halo_udp_comm.c \
UdpLib/tx_buffer_static.c \
UdpLib/crc16.c \
Ublox/ublox_modem_udp.c \
Ublox/ublox_str_utils.c \
Uart/posix_uart.c \
UdpLibImpl/udp_lib_ublox_modem_impl.c \
Modem/at_command_state.c \
Modem/at_mgmt.c


