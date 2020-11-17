#ifndef __ARBRW
#define __ARBRW
#include <psp2common/types.h>

/**
 * @brief Enables or disables debug logging
 * 
 * @param enable 1 to enable, 0 to disable
 */
void ARBRW_enable_debug_log(SceBool enable);

/**
 * @brief Special memcpy that can write to RX pages
 * 
 * @param src Source address (Virtual address in process's address space)
 * @param dst Destination address (Virtual address in process's address space)
 * @param len Length of the data to copy (A length > 0x1000 will use chunked copy, might be slower)
 * @return 0 on success, < 0 on error
 * 
 * @note Requires the kmodule to be loaded, or the import will fail (if you don't use weak stubs, this means your module will refuse to load without the kmodule)
 */
int ARBRW_memcpy_RXBypass(const void* src, void* dst, SceSize len);

/**
 * @brief Writes a word to any address in process's address space
 * 
 * @param src Word to write
 * @param dst Destination address (Virtual address in process's address space)
 * @return 0 on success, < 0 on error
 * 
 * @note Wrapper around ARBRW_memcpy_RXBypass
 */
int ARBRW_write_word(unsigned int src, void* dst){
    int tmp = src;
    return ARBRW_memcpy_RXBypass(&tmp, dst, sizeof(tmp));
}

/**
 * @brief Writes a byte to any address in process's address space
 * 
 * @param src Byte
 * @param dst Destination address (Virtual address in process's address space)
 * @return 0 on success, < 0 on error
 * 
 * @note Wrapper around ARBRW_memcpy_RXBypass
 */
int ARBRW_write_byte(unsigned char src, void* dst){
    unsigned char tmp = src;
    return ARBRW_memcpy_RXBypass(&tmp, dst, sizeof(tmp));
}

#endif