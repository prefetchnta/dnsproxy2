/* vi:set sw=4 sts=4 expandtab: */
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <grp.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cutils/log.h>

#include "DnsProxyListener.h"
#include "resolv_iface.h"

#define SOCKPATH        "/dev/socket/dnsproxyd"

/* default: assume the UID of the caller */
int dns_uid = -1;

static int restore_oldsock;

static void handle_signal(int sig)
{
    if (restore_oldsock) {
        /* re-enable the old proxy, then exit */
        unlink(SOCKPATH);
        rename(SOCKPATH ".bak", SOCKPATH);
    }
    exit(0);
}

static void setup_resolver(const char *server)
{
    long    sz;
    char*   ip;
    FILE*   fp;

    fp = fopen(server, "rb");
    if (fp == NULL)
    {
        /* IP address */
        _resolv_set_default_iface("eth0");
        _resolv_set_nameservers_for_iface("eth0", (char **)&server, 1, "");
    }
    else
    {
        /* load DNS file */
        fseek(fp, 0, SEEK_END);
        sz = ftell(fp);
        if (sz <= 16) {
            ALOGE("invalid dns file (%s)\n", server);
            fclose(fp);
            exit(1);
        }
        ip = (char*)malloc(sz + 1);
        if (ip == NULL) {
            ALOGE("malloc() failure\n");
            fclose(fp);
            exit(1);
        }
        memset(ip, 0, sz + 1);
        rewind(fp);
        if (fread(ip, 1, sz, fp) != sz) {
            ALOGE("fread() failure\n");
            fclose(fp);
            free(ip);
            exit(1);
        }
        fclose(fp);

        char*   tmp;
        char**  lst;
        size_t  idx = 1;
        size_t  cnt = 1;

        /* split IP strings */
        tmp = ip;
        while (*tmp != 0) {
            if (*tmp++ == '|')
                cnt++;
        }
        lst = (char**)malloc(cnt, char*);
        if (lst == NULL) {
            ALOGE("malloc() failure\n");
            free(ip);
            exit(1);
        }
        lst[0] = tmp = ip;
        while (*tmp != 0) {
            if (*tmp == '|') {
                *tmp = 0;
                lst[idx++] = tmp + 1;
            }
            tmp++;
        }
        _resolv_set_default_iface("eth0");
        _resolv_set_nameservers_for_iface("eth0", lst, cnt, "");
        free(lst);
        free(ip);
    }
}

static void setup_listener(int do_wait)
{
    int sockfd;
    struct sockaddr_un sock;
    struct stat statbuf;
    char buf[16];
    DnsProxyListener *dpl;

    /* wait for netd to start up, then take over its socket */
    while (do_wait && stat(SOCKPATH, &statbuf) < 0) {
        sleep(1);
    }

    if (stat(SOCKPATH, &statbuf) == 0) {
        unlink(SOCKPATH ".bak");
        rename(SOCKPATH, SOCKPATH ".bak");
        restore_oldsock = 1;
    }

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ALOGE("socket() failed (%s)\n", strerror(errno));
        exit(1);
    }

    memset(&sock, 0, sizeof(sock));
    sock.sun_family = AF_UNIX;
    strcpy(sock.sun_path, SOCKPATH);

    if (bind(sockfd, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
        ALOGE("can't bind (%s)\n", strerror(errno));
        exit(1);
    }

    if (chmod(SOCKPATH, 0660) < 0) {
        ALOGE("can't chmod (%s)\n", strerror(errno));
        exit(1);
    }

    sprintf(buf, "%d", sockfd);
    setenv("ANDROID_SOCKET_dnsproxyd", buf, 1);

    dpl = new DnsProxyListener();
    if (dpl->startListener()) {
        ALOGE("Unable to start DnsProxyListener (%s)", strerror(errno));
        exit(1);
    }
}

static void usage(void)
{
    puts("usage: dnsproxy2 [ -w ] [ -v ] [ -u <uid> ] [ -l <hostname> ]");
    exit(1);
}

int main(int argc, char **argv)
{
    int do_wait = 0, val;
    struct group *grp;
    char *lookup_host = NULL;

    while ((val = getopt(argc, argv, "hwvu:l:")) != -1) {
        switch (val) {
        case 'w':
            do_wait = 1;
            break;
        case 'v':
            setenv("RES_OPTIONS", "debug", 1);
            break;
        case 'u':
            dns_uid = atoi(optarg);
            break;
        case 'l':
            lookup_host = optarg;
            break;
        case 'h':
        default:
            usage();
        }
    }

    grp = getgrnam("inet");
    if (!grp || setgid(grp->gr_gid)) {
        ALOGW("Unable to change to 'inet' group (%s)", strerror(errno));
    }

    setenv("ANDROID_DNS_MODE", "local", 1);

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);

    if (optind < argc)
        setup_resolver(argv[optind]);
    else
        setup_resolver("8.8.8.8");

    if (lookup_host) {
        /* perform one lookup and then exit */
        struct hostent *hp = gethostbyname2(lookup_host, AF_INET);
        if (hp && hp->h_length) {
            struct in_addr **addr_list = (struct in_addr **)hp->h_addr_list;
            puts(inet_ntoa(*addr_list[0]));
            return 0;
        }
        return 1;
    }

    setup_listener(do_wait);

    while(1) {
        sleep(1000);
    }

    return 0;
}
