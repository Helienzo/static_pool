#include "static_pool.h"
#include <stdio.h>

typedef struct {
    int32_t number;
    staticPoolItem_t node;
} myItem_t;

static int32_t releaseByItem(staticPool_t* pool, myItem_t *my_item) {
    return staticPoolRelease(pool, &my_item->node);
}

static myItem_t* aquireItem(staticPool_t *pool) {
    staticPoolItem_t* pool_item = staticPoolAcquire(pool);
    if (pool_item == NULL) {
        return NULL;
    }

    myItem_t* my_item = CONTAINER_OF(pool_item, myItem_t, node);
    return my_item;
}

#define POOL_SIZE 4

myItem_t           _item_array[POOL_SIZE] = {0};
staticPoolList_t   _pool_list_array[POOL_SIZE] = {0};
staticPool_t       pool = {0};

int main() {
    // Initialize the pool
    int32_t res = STATIC_POOL_INIT(pool, _pool_list_array, POOL_SIZE, _item_array);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Static pool init failed %i\n", res);
        return 1;
    }

    myItem_t* first_item = aquireItem(&pool);
    if (first_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        printf("Aquired item success\n");
        first_item->number = 1;
    }

    myItem_t* second_item = aquireItem(&pool);
    if (second_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        printf("Aquired item success\n");
        second_item->number = 2;
    }

    myItem_t* third_item = aquireItem(&pool);
    if (third_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        printf("Aquired item success\n");
        third_item->number = 3;
    }

    myItem_t* fourth_item = aquireItem(&pool);
    if (fourth_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        printf("Aquired item success\n");
        fourth_item->number = 4;
    }

    myItem_t* fith_item = aquireItem(&pool);
    if (fith_item != NULL) {
        printf("Static pool ERROR could aquire more items than is possible\n");
        return 1;
    } else {
        printf("Over aquire test success\n");
    }

    res = releaseByItem(&pool, first_item);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Failed to return item %i\n", res);
        return 1;
    } else {
        printf("Release item success\n");
    }

    res = staticPoolAvailable(&pool);
    if (res != 1) {
        printf("There should be exactly one item left %i\n", res);
        return 1;
    }

    res = releaseByItem(&pool, first_item);
    if (res != STATIC_POOL_DOUBLE_RELEASE) {
        printf("Error Return item twice possible %i\n", res);
        return 1;
    } else {
        printf("Double Release test success\n");
    }

    first_item = aquireItem(&pool);
    if (first_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (first_item->number != 1) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    res = releaseByItem(&pool, third_item);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Failed to return item %i\n", res);
        return 1;
    } else {
        printf("Release item success\n");
    }

    res = releaseByItem(&pool, fourth_item);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Failed to return item %i\n", res);
        return 1;
    } else {
        printf("Release item success\n");
    }

    res = releaseByItem(&pool, first_item);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Failed to return item %i\n", res);
        return 1;
    } else {
        printf("Release item success\n");
    }

    res = releaseByItem(&pool, second_item);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Failed to return item %i\n", res);
        return 1;
    } else {
        printf("Release item success\n");
    }

    res = staticPoolAvailable(&pool);
    if (res != POOL_SIZE) {
        printf("Availability missmatch %i\n", res);
    }

    first_item = aquireItem(&pool);
    if (first_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (first_item->number != 2) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    second_item = aquireItem(&pool);
    if (second_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (second_item ->number != 1) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    third_item = aquireItem(&pool);
    if (third_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (third_item->number != 4) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    fourth_item = aquireItem(&pool);
    if (fourth_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (fourth_item->number != 3) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    res = staticPoolRestore(&pool);
    if (res != STATIC_POOL_SUCCESS) {
        printf("Static pool restore failed %i\n", res);
        return 1;
    } else {
        printf("Restore success\n");
    }

    first_item = aquireItem(&pool);
    if (first_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (first_item->number != 1) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    second_item = aquireItem(&pool);
    if (second_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (second_item ->number != 2) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    third_item = aquireItem(&pool);
    if (third_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (third_item->number != 3) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    fourth_item = aquireItem(&pool);
    if (fourth_item == NULL) {
        printf("Static pool failed to aquire item\n");
        return 1;
    } else {
        // Note this is not how it is intended to be used, but it is a way to test
        if (fourth_item->number != 4) {
            printf("Bad item order %i\n", first_item->number);
        } else {
            printf("Aquire success\n");
        }
    }

    return 0;
}