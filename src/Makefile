MODULE	:= dnsproxy2
SRC_FILES	:= dnsproxy2.cpp \
	FrameworkListener.cpp \
	FrameworkCommand.cpp \
	DnsProxyListener.cpp \
	SocketListener.cpp \
	SocketClient.cpp \
	NetdCommand.cpp \
	gethnamaddr.c \
	getaddrinfo.c \
	getnameinfo.c \
	getservbyname.c \
	getservbyport.c \
	getservent.c \
	nsdispatch.c \
	logd_write.c \
	atomic.c \
	resolv/__dn_comp.c \
	resolv/herror.c \
	resolv/res_cache.c \
	resolv/__res_close.c \
	resolv/res_compat.c \
	resolv/res_comp.c \
	resolv/res_data.c \
	resolv/res_debug.c \
	resolv/res_init.c \
	resolv/res_mkquery.c \
	resolv/res_query.c \
	resolv/__res_send.c \
	resolv/res_send.c \
	resolv/res_state.c \
	resolv/libc_logging.cpp \
	nameser/ns_name.c \
	nameser/ns_netint.c \
	nameser/ns_parse.c \
	nameser/ns_print.c \
	nameser/ns_samedomain.c \
	nameser/ns_ttl.c

OBJ_FILES	:= dnsproxy2.o \
	FrameworkListener.o \
	FrameworkCommand.o \
	DnsProxyListener.o \
	SocketListener.o \
	SocketClient.o \
	NetdCommand.o \
	gethnamaddr.o \
	getaddrinfo.o \
	getnameinfo.o \
	getservbyname.o \
	getservbyport.o \
	getservent.o \
	nsdispatch.o \
	logd_write.o \
	atomic.o \
	__dn_comp.o \
	herror.o \
	res_cache.o \
	__res_close.o \
	res_compat.o \
	res_comp.o \
	res_data.o \
	res_debug.o \
	res_init.o \
	res_mkquery.o \
	res_query.o \
	__res_send.o \
	res_send.o \
	res_state.o \
	libc_logging.o \
	ns_name.o \
	ns_netint.o \
	ns_parse.o \
	ns_print.o \
	ns_samedomain.o \
	ns_ttl.o

CFLAGS	:= -O2 -fPIC \
	-ffunction-sections \
	-fdata-sections \
	-DHAVE_SYS_UIO_H \
	-DANDROID_SMP=1 \
	-DANDROID_CHANGES \
	-D_LIBC \
	-I. \
	-I./resolv \
	-I./private \
	-I "%NDK_INC%" \
	-include netdb-private.h

LFLAGS	:= -static -fPIC \
	-Wl,--gc-sections \
	-L "%NDK_LIB%" \
	-L "%STL_LIB%"

build:
	$(CC) $(CFLAGS) -c $(SRC_FILES)
	$(CC) $(LFLAGS) -o $(MODULE) $(OBJ_FILES) -lsupc++ -lc
	$(SP) --strip-unneeded $(MODULE)
