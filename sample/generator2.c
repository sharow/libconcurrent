/* -*- Mode: c; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include <stdio.h>
#include <stdlib.h>
#include "concurrent/concurrent.h"

/* short name API alias */
#include "concurrent/short_lower_case_api.h"

typedef struct {
    int mA;
    int mB;
} UserData;

void generator(ConcurrentContext *aContext)
{
    UserData *user_data;

    yield_void(aContext);
    yield_str(aContext, "string");
    yield_int(aContext, 12345);
    yield_int(aContext, -12345);
    yield_uint(aContext, 12345U);
    yield_ulong(aContext, 0xffffffff);

    user_data = ctx_get_userptr(aContext);
    user_data->mA = 123;
    user_data->mB = 123;
    yield_voidptr(aContext, (void *)user_data);
    printf("generator(): finish\n");
} /* return == yield_void() */

int main(void)
{
    ConcurrentContext *ctx;
    unsigned char *stack;
    const int stack_size = 1024 * 2;
    UserData user_data;

    concurrent_init();
    ctx = malloc(ctx_sizeof());
    stack = malloc(sizeof(*stack) * stack_size);
    ctx_construct(ctx, stack, stack_size, generator, &user_data);
    while (!ctx_is_done(ctx)) {
        ConcurrentGeneratorValueType type;
        ConcurrentStatus status;
        status = gen_value(ctx, &type);
        if (status != ConcurrentStatus_SUCCESS) {
            printf("gen_value returned %s\n", concurrent_get_status_str(status));
            break;
        }
        switch (type) {
        case ConcurrentGeneratorValueType_VOID:
            printf("yield void\n");
            break;
        case ConcurrentGeneratorValueType_INT:
            printf("yield int: %d\n", ctx_get_int(ctx));
            break;
        case ConcurrentGeneratorValueType_UINT:
            printf("yield uint: %u\n", ctx_get_uint(ctx));
            break;
        case ConcurrentGeneratorValueType_LONG:
            printf("yield long: %ld\n", ctx_get_long(ctx));
            break;
        case ConcurrentGeneratorValueType_ULONG:
            printf("yield ulong: %lu\n", ctx_get_ulong(ctx));
            break;
        case ConcurrentGeneratorValueType_CONSTSTRING:
            printf("yield str: %s\n", ctx_get_str(ctx));
            break;
        case ConcurrentGeneratorValueType_VOIDPTR:
            {
                UserData *user_datap = ctx_get_voidptr(ctx);
                printf("yield voidptr(UserData): %d %d\n", user_datap->mA, user_datap->mB);
            }
            break;
        default:
            printf("yield unknown\n");
            break;
        }
    }
    ctx_destruct(ctx);

    free(stack);
    free(ctx);
    concurrent_fin();
    return 0;
}

