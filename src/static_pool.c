/**
 * @file:       static_pool.c
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Implementation of the static pool
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

#include "static_pool.h"

int32_t staticPoolInit(staticPool_t *pool, staticPoolList_t *list_array, size_t capacity, size_t item_size, staticPoolItem_t *first_item)
{
    if (pool == NULL || list_array == NULL || capacity == 0 || first_item == NULL) {
        return STATIC_POOL_NULL_ERROR;
    }

    pool->list     = list_array; // The user-provided array
    pool->top      = capacity;   // Start with the pool full of items
    pool->capacity = capacity;

    staticPoolItem_t * item = first_item;
    for (uint32_t i = 0; i < capacity; i++) {
        pool->list[i].original      = item;
        pool->list[i].current       = item;
        pool->list[i].original->out = false;
        item = (staticPoolItem_t*)((uint8_t*)item + item_size);
    }

    return STATIC_POOL_SUCCESS;
}

int32_t staticPoolRestore(staticPool_t *pool)
{
    if (pool == NULL) {
        return STATIC_POOL_NULL_ERROR;
    }

    // Restore the pool to it's original state
    for (uint32_t i = 0; i < pool->capacity; i++) {
        pool->list[i].current = pool->list[i].original;
        pool->list[i].original->out = false;
    }

    pool->top = pool->capacity;

    return STATIC_POOL_SUCCESS;
}

staticPoolItem_t* staticPoolAcquire(staticPool_t *pool)
{
    if (pool == NULL) {
        return NULL;
    }

    // If top == 0, there are no free items left
    if (pool->top == 0) {
        return NULL;
    }

    pool->top--;  // pop

    // Set this item as out
    pool->list[pool->top].current->out = true;

    return pool->list[pool->top].current;
}

int32_t staticPoolRelease(staticPool_t *pool, staticPoolItem_t *item)
{
    if (pool == NULL || item == NULL) {
        return STATIC_POOL_NULL_ERROR;
    }

    // Check if this item is allready returned
    if (!item->out) {
        return STATIC_POOL_DOUBLE_RELEASE;
    }

    // If there's space, push it back
    if (pool->top < pool->capacity) {
        item->out = false;
        pool->list[pool->top++].current = item;
    } else {
        // This would be bad, would indicate missuse of the pool
        return STATIC_POOL_OVERFLOW;
    }

    return STATIC_POOL_SUCCESS;
}

int32_t staticPoolAvailable(staticPool_t *pool) {
    if (pool == NULL) {
        return STATIC_POOL_NULL_ERROR;
    }

    return (int32_t)pool->top;
}