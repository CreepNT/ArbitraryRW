
#include <psp2kern/kernel/cpu.h>
#include <psp2kern/kernel/dmac.h>
#include <psp2kern/kernel/sysmem.h>
#include <psp2kern/kernel/sysclib.h>
#include <psp2kern/kernel/threadmgr.h>
#include <psp2kern/kernel/modulemgr.h>
#include <taihen.h>

#include "ArbitraryRW.h"

static char buf[0x1000];
static SceBool dbg = 0;

#define LOG(fmt, ...)\
    if (dbg) ksceDebugPrintf("ArbitraryRW | " fmt, ##__VA_ARGS__)

void ARBRW_enable_debug_log(SceBool enable){
    int state;
    ENTER_SYSCALL(state);
    dbg = enable;
    EXIT_SYSCALL(state);
    return;
}

int ARBRW_memcpy_RXBypass(const void* src, void* dst, SceSize len){
    int state, ret;
    SceUID pid;
    ENTER_SYSCALL(state);
    pid = ksceKernelGetProcessId();
    char* pbuf = buf;

    LOG("Entering syscall !\n src = %p, dst = %p, len = 0x%X | process PID = 0x%08X\n",src,dst,len,pid);
    if (len > sizeof(buf)){ //More than 4Kib to copy -> do a chunked copy instead
        LOG("Got size 0x%X > max size 0x%X. Performing chunked copy...\n",len, sizeof(buf));

        unsigned int count = len / sizeof(buf); //Number of full 4Kib chunks
        SceSize last_len = len - (count * sizeof(buf)); //Remaining bytes
        LOG("Number of buffer fills needed : %u | remains 0x%X bytes to copy\n", count, last_len);

        for (int i = 0; i < count; i++, pbuf += sizeof(buf)){ //Copy in chuncks of 4KiB (0x1000 = 4096)
            LOG("Iter #%u - pbuf = %p - ",i, pbuf);
            ret = ksceKernelMemcpyUserToKernelForPid(pid, pbuf, (uintptr_t)src, len);
            LOG("UserToKernel ret = 0x%08X - ", ret);
            if (ret < 0){
                EXIT_SYSCALL(state);
                return ret;
            }
            ret = ksceKernelRxMemcpyKernelToUserForPid(pid, (uintptr_t)dst, buf, len);
            LOG("RxKernelToUser ret = 0x%08X\n", ret);
            if (ret < 0){
                EXIT_SYSCALL(state);
                return ret;
            }
        }
        LOG("Copied %u chunks successfully ! remaining bytes : 0x%X",count, last_len);
        if (last_len != 0) len = last_len; //Let the "regular case" handle the copy
    }
    LOG("Copying 0x%X bytes from %p to %p...", len, pbuf, dst);
    ret = ksceKernelMemcpyUserToKernelForPid(pid, pbuf, (uintptr_t)src, len);
    LOG("UserToKernel ret = 0x%08X\n", ret);
    if (ret < 0){
        EXIT_SYSCALL(state);
        return ret;
    }
    ret = ksceKernelRxMemcpyKernelToUserForPid(pid, (uintptr_t)dst, buf, len);
    LOG("RxKernelToUser ret = 0x%08X\nDone ! Returning...\n",ret);
    EXIT_SYSCALL(state);
    return ret;
}

void _start() __attribute__((weak, alias("module_start")));
int module_start(SceSize argc, const void* args){
    return SCE_KERNEL_START_RESIDENT;
}

int module_stop(SceSize argc, const void* args){
    return SCE_KERNEL_STOP_SUCCESS;
}
