/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_DORNERWORKS_BSD)
 */

/*
 * ARM Generic Interrupt Controller v3 and v4 (GIC-500)
 */

#ifndef ARCH_MACHINE_GIC500_H
#define ARCH_MACHINE_GIC500_H

#include <stdint.h>
#include <util.h>
#include <linker.h>
#include <mode/smp/smp.h>
#include <model/statedata.h>
#include <armv/machine.h>

typedef uint16_t interrupt_t;
typedef uint16_t irq_t;

enum irqNumbers {
    irqInvalid = (irq_t) - 1
};

/* Special IRQ's */
#define SPECIAL_IRQ_START 1020u
#define IRQ_NONE          1023u

#define NR_GIC_LOCAL_IRQS  32
#define NR_GIC_SGI         16

#define GIC_SGI_START            (0)
#define GIC_SGI_END              (15)
#define GIC_PPI_START            (16)
#define GIC_PPI_END              (31)


#define GIC_PRI_LOWEST     0xf0
#define GIC_PRI_IRQ        0xa0
#define GIC_PRI_HIGHEST    0x80 /* Higher priorities belong to Secure-World */

/* Setters/getters helpers */
#define IRQ_REG(IRQ) ((IRQ) >> 5u)
#define IRQ_BIT(IRQ) ((IRQ) & 0x1f)
#define IRQ_MASK MASK(16u)
#define IS_IRQ_VALID(X) (((X) & IRQ_MASK) < SPECIAL_IRQ_START)

/* Register bits */
#define GICD_CTL_ENABLE 0x1
#define GICD_CTLR_RWP                BIT(31)
#define GICD_CTLR_ARE_NS             BIT(4)
#define GICD_CTLR_ENABLE_G1NS         BIT(1)
#define GICD_CTLR_ENABLE_G0          BIT(0)
#define GICD_IROUTER_SPI_MODE_ANY    BIT(31)

/* Common between GICD_PIDR2 and GICR_PIDR2 */
#define GIC_PIDR2_ARCH_MASK         (0xf0)
#define GIC_PIDR2_ARCH_GICv3        (0x30)
#define GIC_PIDR2_ARCH_GICv4        (0x40)

#define GICD_TYPE_LINESNR 0x01f

#define GICC_SRE_EL1_SRE             BIT(0)

#define GICR_WAKER_ProcessorSleep    BIT(1)
#define GICR_WAKER_ChildrenAsleep    BIT(2)

#define GICC_CTLR_EL1_EOImode_drop   BIT(1)

#define DEFAULT_PMR_VALUE            0xff

/* System registers for GIC CPU interface */
#define ICC_IAR1_EL1    "S3_0_C12_C12_0"
#define ICC_EOIR1_EL1   "S3_0_C12_C12_1"
#define ICC_HPPIR1_EL1  "S3_0_C12_C12_2"
#define ICC_BPR1_EL1    "S3_0_C12_C12_3"
#define ICC_DIR_EL1     "S3_0_C12_C11_1"
#define ICC_PMR_EL1     "S3_0_C4_C6_0"
#define ICC_CTLR_EL1    "S3_0_C12_C12_4"
#define ICC_IGRPEN1_EL1 "S3_0_C12_C12_7"
#define ICC_SRE_EL1     "S3_0_C12_C12_5"

/* Memory map for GIC distributor */
struct gic_dist_map {
    uint32_t ctlr;                /* 0x0000 */
    uint32_t typer;               /* 0x0004 */
    uint32_t iidr;                /* 0x0008 */
    uint32_t res1[13];            /* [0x000C, 0x0040) */
    uint32_t setspi_nsr;          /* 0x0040 */
    uint32_t res2;                /* 0x0044 */
    uint32_t clrspi_nsr;          /* 0x0048 */
    uint32_t res3;                /* 0x004C */
    uint32_t setspi_sr;           /* 0x0050 */
    uint32_t res4;                /* 0x0054 */
    uint32_t clrspi_sr;           /* 0x0058 */
    uint32_t res5[9];             /* [0x005C, 0x0080) */
    uint32_t igrouprn[32];        /* [0x0080, 0x0100) */

    uint32_t enable_set[32];        /* [0x100, 0x180) */
    uint32_t enable_clr[32];        /* [0x180, 0x200) */
    uint32_t pending_set[32];       /* [0x200, 0x280) */
    uint32_t pending_clr[32];       /* [0x280, 0x300) */
    uint32_t active_set[32];        /* [0x300, 0x380) */
    uint32_t active_clr[32];        /* [0x380, 0x400) */

    uint32_t priority[255];         /* [0x400, 0x7FC) */
    uint32_t res6;                  /* 0x7FC */

    uint32_t targets[255];          /* [0x800, 0xBFC) */
    uint32_t res7;                  /* 0xBFC */

    uint32_t config[64];            /* [0xC00, 0xD00) */
    uint32_t group_mod[64];         /* [0xD00, 0xE00) */
    uint32_t nsacr[64];             /* [0xE00, 0xF00) */
    uint32_t sgir;                  /* 0xF00 */
    uint32_t res8[3];               /* [0xF00, 0xF10) */
    uint32_t sgi_pending_clr[4];    /* [0xF10, 0xF20) */
    uint32_t sgi_pending_set[4];    /* [0xF20, 0xF30) */
    uint32_t res9[5235];            /* [0x0F30, 0x6100) */

    uint64_t irouter[960];          /* [0x6100, 0x7F00) */
    uint64_t res10[2080];           /* [0x7F00, 0xC000) */
    uint32_t estatusr;              /* 0xC000 */
    uint32_t errtestr;              /* 0xC004 */
    uint32_t res11[31];             /* [0xC008, 0xC084) */
    uint32_t spisr[30];             /* [0xC084, 0xC0FC) */
    uint32_t res12[4021];           /* [0xC0FC, 0xFFD0) */

    uint32_t pidrn[8];              /* [0xFFD0, 0xFFF0) */
    uint32_t cidrn[4];              /* [0xFFD0, 0xFFFC] */
};

/* Memory map for GIC Redistributor Registers for control and physical LPI's */
struct gic_rdist_map {          /* Starting */
    uint32_t    ctlr;           /* 0x0000 */
    uint32_t    iidr;           /* 0x0004 */
    uint64_t    typer;          /* 0x008 */
    uint32_t    res0;           /* 0x0010 */ /*Would be the status register, but that is not implemented in the GIC-500*/
    uint32_t    waker;          /* 0x0014 */
    uint32_t    res1[21];       /* 0x0018 */
    uint64_t    propbaser;      /* 0x0070 */
    uint64_t    pendbaser;      /* 0x0078 */
    uint32_t    res2[16340];    /* 0x0080 */
    uint32_t    pidr4;          /* 0xFFD0 */
    uint32_t    pidr5;          /* 0xFFD4 */
    uint32_t    pidr6;          /* 0xFFD8 */
    uint32_t    pidr7;          /* 0xFFDC */
    uint32_t    pidr0;          /* 0xFFE0 */
    uint32_t    pidr1;          /* 0xFFE4 */
    uint32_t    pidr2;          /* 0xFFE8 */
    uint32_t    pidr3;          /* 0xFFEC */
    uint32_t    cidr0;          /* 0xFFF0 */
    uint32_t    cidr1;          /* 0xFFF4 */
    uint32_t    cidr2;          /* 0xFFF8 */
    uint32_t    cidr3;          /* 0xFFFC */
};

/* Memory map for the GIC Redistributor Registers for the SGI and PPI's */
struct gic_rdist_sgi_ppi_map {  /* Starting */
    uint32_t    res0[32];       /* 0x0000 */
    uint32_t    igroupr0;       /* 0x0080 */
    uint32_t    res1[31];       /* 0x0084 */
    uint32_t    isenabler0;     /* 0x0100 */
    uint32_t    res2[31];       /* 0x0104 */
    uint32_t    icenabler0;     /* 0x0180 */
    uint32_t    res3[31];       /* 0x0184 */
    uint32_t    ispendr0;       /* 0x0200 */
    uint32_t    res4[31];       /* 0x0204 */
    uint32_t    icpendr0;       /* 0x0280 */
    uint32_t    res5[31];       /* 0x0284 */
    uint32_t    isactiver0;     /* 0x0300 */
    uint32_t    res6[31];       /* 0x0304 */
    uint32_t    icactiver0;     /* 0x0380 */
    uint32_t    res7[31];       /* 0x0384 */
    uint32_t    ipriorityrn[8]; /* 0x0400 */
    uint32_t    res8[504];      /* 0x0420 */
    uint32_t    icfgrn_ro;      /* 0x0C00 */
    uint32_t    icfgrn_rw;      /* 0x0C04 */
    uint32_t    res9[62];       /* 0x0C08 */
    uint32_t    igrpmodr0;      /* 0x0D00*/
    uint32_t    res10[63];      /* 0x0D04 */
    uint32_t    nsac;           /* 0x0E00 */
    uint32_t    res11[11391];   /* 0x0E04 */
    uint32_t    miscstatsr;     /* 0xC000 */
    uint32_t    res12[31];      /* 0xC004 */
    uint32_t    ppisr;          /* 0xC080 */
    uint32_t    res13[4062];    /* 0xC084 */

};

extern volatile struct gic_dist_map *const gic_dist;
extern volatile struct gic_rdist_map *const gic_rdist;
extern volatile struct gic_rdist_sgi_ppi_map *const gic_rdist_sgi_ppi;
/*
 * The only sane way to get an GIC IRQ number that can be properly
 * ACKED later is through the int_ack register. Unfortunately, reading
 * this register changes the interrupt state to pending so future
 * reads will not return the same value For this reason, we have a
 * global variable to store the IRQ number.
 */
extern uint32_t active_irq[CONFIG_MAX_NUM_NODES];


static inline bool_t is_sgi(irq_t irq)
{
    return (irq >= GIC_SGI_START) && (irq <= GIC_SGI_END);
}

static inline bool_t is_ppi(irq_t irq)
{
    return (irq >= GIC_PPI_START) && (irq <= GIC_PPI_END);
}

/* Helpers */
static inline int is_irq_edge_triggered(irq_t irq)
{
    uint32_t icfgr = 0;
    int word = irq >> 4;
    int bit = ((irq & 0xf) * 2);

    if (is_sgi(irq)) {
        return 0;
    }
    if (is_ppi(irq)) {
        icfgr = gic_rdist_sgi_ppi->icfgrn_rw;
    } else {
        icfgr = gic_dist->config[word];
    }

    return !!(icfgr & BIT(bit + 1));
}

static inline void gic_pending_clr(irq_t irq)
{
    int word = IRQ_REG(irq);
    int bit = IRQ_BIT(irq);
    /* Using |= here is detrimental to your health */
    /* Applicable for SPI and PPIs */
    if (irq < NR_GIC_LOCAL_IRQS) {
        gic_rdist_sgi_ppi->icpendr0 = BIT(bit);
    } else {
        gic_dist->pending_clr[word] = BIT(bit);
    }
}

static inline void gic_enable_clr(irq_t irq)
{
    int word = IRQ_REG(irq);
    int bit = IRQ_BIT(irq);
    /* Using |= here is detrimental to your health */
    if (irq < NR_GIC_LOCAL_IRQS) {
        gic_rdist_sgi_ppi->icenabler0 = BIT(bit);
    } else {
        gic_dist->enable_clr[word] = BIT(bit);
    }

}

static inline void gic_enable_set(irq_t irq)
{
    int word = IRQ_REG(irq);
    int bit = IRQ_BIT(irq);

    if (irq < NR_GIC_LOCAL_IRQS) {
        gic_rdist_sgi_ppi->isenabler0 = BIT(bit);
    } else {
        gic_dist->enable_set[word] = BIT(bit);
    }

}

static inline interrupt_t getActiveIRQ(void)
{
    uint32_t irq;

    if (!IS_IRQ_VALID(active_irq[CURRENT_CPU_INDEX()])) {
        uint32_t val = 0;
        SYSTEM_READ_WORD(ICC_IAR1_EL1, val);
        active_irq[CURRENT_CPU_INDEX()] = val;
    }

    if (IS_IRQ_VALID(active_irq[CURRENT_CPU_INDEX()])) {
        irq = active_irq[CURRENT_CPU_INDEX()] & IRQ_MASK;
    } else {
        irq = irqInvalid;
    }

    return irq;
}

/*
 * GIC has 4 states:
 * seL4 expects two states: active->inactive.
 * We ignore the active state in GIC to conform
 */
static inline bool_t isIRQPending(void)
{
    uint32_t val = 0;
    /* Check for pending IRQs in group 1: ICC_HPPIR1_EL1 */
    SYSTEM_READ_WORD(ICC_HPPIR1_EL1, val);
    return IS_IRQ_VALID(val);
}

static inline void maskInterrupt(bool_t disable, interrupt_t irq)
{
    if (disable) {
        gic_enable_clr(irq);
    } else {
        gic_enable_set(irq);
    }
}

static inline void ackInterrupt(irq_t irq)
{
    assert(IS_IRQ_VALID(active_irq[CURRENT_CPU_INDEX()]) && (active_irq[CURRENT_CPU_INDEX()] & IRQ_MASK) == irq);
    if (is_irq_edge_triggered(irq)) {
        gic_pending_clr(irq);
    }

    /* Set End of Interrupt for active IRQ: ICC_EOIR1_EL1 */
    SYSTEM_WRITE_WORD(ICC_EOIR1_EL1, active_irq[CURRENT_CPU_INDEX()]);
    SYSTEM_WRITE_WORD(ICC_DIR_EL1, active_irq[CURRENT_CPU_INDEX()]);
    active_irq[CURRENT_CPU_INDEX()] = IRQ_NONE;

}

static inline void handleSpuriousIRQ(void)
{
}

void initIRQController(void);

#ifdef ENABLE_SMP_SUPPORT
void ipiBroadcast(irq_t irq, bool_t includeSelfCPU);
void ipi_send_target(irq_t irq, word_t cpuTargetList);
#endif /* ENABLE_SMP_SUPPORT */

#endif /* ARCH_MACHINE_GIC500_H */