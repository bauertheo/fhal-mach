#include <can.h>
#define CAN_PRV
#include <can_prv.h>
#include <ti/carcan.h>
#include <clock.h>
#include <mux.h>
#include <iomux.h>
#include <vector.h>
#include <stdio.h>

#define CM_WKUPAON_DCAN1_CLKCTRL_ADR        (volatile void *) 0x6ae07888u
#define CM_L4PER2_DCAN2_CLKCLTR_ADR         (volatile void *) 0x6a0098f0u

struct carcan_pins {
	const uint32_t pin;
	const uint32_t ctl;
	const uint32_t extra;
};



int32_t carcan_setupClock(struct can *can){
    struct clock *clock = clock_init();
    can->freq = clock_getPeripherySpeed(clock, 0);

    volatile uint32_t *clkreg;
#ifdef CONFIG_MACH_AM57xx_CARCAN_CAN1
    clkreg = (volatile void *) CM_WKUPAON_DCAN1_CLKCTRL_ADR;
    /* Check DCAN1 Clock is enabled */
    if((*clkreg >> 16) & 0x3u){
        /* enable clock */
        *clkreg |= 0x2u;
        /* wait clock came stable */
        while((*clkreg >> 16) & 0x3u);
    }
#endif

#ifdef CONFIG_MACH_AM57xx_CARCAN_CAN2
    clkreg = (volatile void *) CM_L4PER2_DCAN2_CLKCLTR_ADR;
    /* Check DCAN2 Clock is enabled */
    if((*clkreg >> 16) & 0x3u){
        /* enable clock */
        *clkreg |= 0x2u;
        /* wait clock came stable */
        while((*clkreg >> 16) & 0x3u);
    }
#endif
    return 0;
}

int32_t carcan_setupPins(struct can *can) {
	int32_t ret;
	struct mux *mux = mux_init();
    printf("*pins = can->pins\n");
	struct carcan_pins const *pins = can->pins;
	int i;
	for (i = 0; i < 2; i++) {
        printf("ret = mux_pinctl, i= %i\n", i);
		ret = mux_pinctl(mux, pins[i].pin, pins[i].ctl, pins[i].extra);
		if (ret < 0) {
			return -1;
		}
	}
	return 0;
}

// TODO Check values
static const struct can_bittiming_const carcan_bittimings = {
	.tseg1_min = 4,
	.tseg1_max = 16,
	.tseg2_min = 2,
	.tseg2_max = 8,
	.sjw_max = 4,
	.brp_min = 1,
	.brp_max = 256,
	.brp_inc = 1,
};





#define AM57_CARCAN_1 ((volatile struct carcan_regs *) 0x6ae3c000u)
#define AM57_CARCAN_2 ((volatile struct carcan_regs *) 0x68480000u)

#ifdef CONFIG_MACH_AM57xx_CARCAN_CAN1
struct carcan_filter can_carcan1_filter[CONFIG_MACH_AM57xx_CARCAN_CAN1_MAX_FILTER];
struct can carcan1 = {
    CAN_INIT_DEV(carcan)
    HAL_NAME("CarCAN 1")
    //.clkData = ,
    //.pins = ,
    .base = AM57_CARCAN_1,
    .btc = &carcan_bittimings,
    .filterLength = CONFIG_MACH_AM57xx_CARCAN_CAN1_FILTER_QUEUE_ENTRIES,
    .filterCount = CONFIG_MACH_AM57xx_CARCAN_CAN1_MAX_FILTER,
    //.mb_count = 32 ,
    .filter = can_carcan1_filter,
    //.irqNum = ,
    //irqIDs = ,
};

CAN_ADDDEV(ti, carcan1);

#endif


#ifdef CONFIG_MACH_AM57xx_CARCAN_CAN2

struct carcan_filter can_carcan2_filter[CONFIG_MACH_AM57xx_CARCAN_CAN2_MAX_FILTER];
struct can carcan2 = {
    CAN_INIT_DEV(carcan)
    HAL_NAME("CarCAN 2")
    //.clkData = ,
    //.pins = ,
    .base = AM57_CARCAN_2,
    .btc = &carcan_bittimings,
    .filterLength = CONFIG_MACH_AM57xx_CARCAN_CAN2_FILTER_QUEUE_ENTRIES,
    .filterCount = CONFIG_MACH_AM57xx_CARCAN_CAN2_MAX_FILTER,
    //.mb_count = 32 ,
    .filter = can_carcan2_filter,
    //.irqNum = ,
    //irqIDs = ,
};

CAN_ADDDEV(ti, carcan2);

#endif
