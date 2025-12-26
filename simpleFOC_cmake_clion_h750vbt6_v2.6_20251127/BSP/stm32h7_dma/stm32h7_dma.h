//
// Created by xwj on 11/22/25.
//

#ifndef __STM32H7_DMA_H
#define __STM32H7_DMA_H

#if defined( __ICCARM__ )
        #define DMA_BUFFER \
        _Pragma("location=\".dma_buffer\"")
#else
        #define DMA_BUFFER \
        __attribute__((section(".dma_buffer")))
#endif
// add the following content to '.ld' file
// .dma_buffer : /* Space before ':' is critical */
// {
//   *(.dma_buffer)
// } >RAM_D2
#endif // SIMPLEFOC_CMAKE_CLION_H750VBT6_V2_5_20251118_STM32H7_DMA_H
