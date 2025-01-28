/**
 * @file:       static_pool.h
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Header file for the static pool
 *
 * @license: MIT License
 *
 * Copyright (c) 2024 Lucas Wennerholm
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/**
 * To use the pool do the following:
 * Create an item struct that contains both the
 * staticPoolItem_t and your custom data struct
 *
 * typedef struct {
 *   myCustomData_t   data;
 *   staticPoolItem_t node;
 * } myPoolItem_t;
 *
 * Create a array of myPoolItems_t's and an array of staticPoolItem_t of the same size, and a staticPool_t
 *
 * myPoolItem_t      _pkt_pool[POOL_SIZE];
 * staticPoolItem_t*  _pool_array[POOL_SIZE];
 * staticPool_t       packet_pool;
 *
 * Create access functions:
 *
 *   static int32_t releasePacket(staticPool_t *inst, myCustomData_t *data) {
 *       // Get the data item assosiated with this data
 *       myPoolItem_t * data_item = CONTAINER_OF(data, myPoolItem_t, data);
 *       // Return the node of the packet item to the pool
 *       return staticPoolRelease(inst, &data_item->node);
 *   }
 *
 *   static myCustomData_t * aquirePacket(staticPool_t *inst) {
 *       staticPoolItem_t* pool_item = staticPoolAcquire(inst);
 *       if (pool_item == NULL) {
 *           return NULL;
 *       }
 *       myPoolItem_t * data_item = CONTAINER_OF(pool_item, myPoolItem_t, node);
 *       return &data_item->data;
 *   }
 *
 *  Make sure to initialize the pool and all items in it
 *
 *   int32_t res = 0;
 *   // Initialize the packet pool, and fill it with available packets
 *   if ((res = staticPoolInit(&packet_pool, _pool_array, POOL_SIZE)) != STATIC_POOL_SUCCESS) {
 *       return POOL_ERROR;
 *   }
 *
 *   for (int32_t i = 0; i < POOL_SIZE; i++) {
 *       if ((res = staticPoolRegister(&packet_pool, _pkt_pool[i].node)) != STATIC_POOL_SUCCESS) {
 *           return POOL_ERROR;
 *       }
 *   }
 *
 */

#ifndef STATIC_POOL_H
#define STATIC_POOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member)	(type *)((char *)(ptr) - offsetof(type,member))
#endif

typedef enum {
    STATIC_POOL_SUCCESS,
    STATIC_POOL_NULL_ERROR     = -101,
    STATIC_POOL_EMPTY          = -102,
    STATIC_POOL_FULL           = -103,
    STATIC_POOL_OVERFLOW       = -104,
    STATIC_POOL_DOUBLE_RELEASE = -105,
} staticPoolErr_t;

/**
 * This item should be embedded into what ever struct 
 */
typedef struct staticPoolItem {
    bool out;
} staticPoolItem_t;

typedef struct staticPoolList {
    staticPoolItem_t* original;
    staticPoolItem_t* current;
} staticPoolList_t;


/**
 * This is the actuall pool object
 */
typedef struct staticPool {
    staticPoolList_t  *list;    // Pointer to an array of pool item pointers
    size_t            top;       // Next free slot in items[]
    size_t            capacity; // Next free slot in items[]
} staticPool_t;

/**
 * Initialize the static pool
 * Input: Pointer to a static pool instance
 * Input: Pointer to the static pool iteams array
 * Input: Size of the pool array
 * Returns: staticPoolErr_t
 */
int32_t staticPoolInit(staticPool_t *pool, staticPoolList_t *list_array, size_t capacity, size_t item_size, staticPoolItem_t *first_item);

/**
 * Restore the pool to it's original state
 * Input: Pointer to a static pool instance
 * Returns: staticPoolErr_t
 */
int32_t staticPoolRestore(staticPool_t *pool);

/**
 * Aquire an item from the pool
 * Input: Pointer to a static pool instance
 * Returns: An available item or NULL if empty
 */
staticPoolItem_t* staticPoolAcquire(staticPool_t *pool);

/**
 * Return an item to the pool
 * Input: Pointer to a static pool instance
 * Input: Item to release
 * Returns: staticPoolErr_t
 */
int32_t staticPoolRelease(staticPool_t *pool, staticPoolItem_t *item);

/**
 * Check item availablitiy
 * Input: Pointer to a static pool instance
 * Returns: Number of items or staticPoolErr_t
 */
int32_t staticPoolAvailable(staticPool_t *pool);

/**
 * This is a macro that makes it more safe to initialize a static pool
 */
#define STATIC_POOL_INIT(pool, list, length, item_array) \
    staticPoolInit(&(pool), &((list)[0]), (length), sizeof((item_array)[0]), &item_array->node);

#endif /* STATIC_POOL_H */