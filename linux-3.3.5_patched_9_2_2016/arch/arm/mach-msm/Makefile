obj-y += io.o idle.o timer.o modem_notifier.o
obj-y += clock.o clock-voter.o clock-dummy.o
obj-$(CONFIG_DEBUG_FS) += clock-debug.o

obj-$(CONFIG_MSM_VIC) += irq-vic.o
obj-$(CONFIG_MSM_IOMMU) += devices-iommu.o

obj-$(CONFIG_ARCH_MSM7X00A) += dma.o irq.o acpuclock-arm11.o
obj-$(CONFIG_ARCH_MSM7X30) += dma.o
obj-$(CONFIG_ARCH_QSD8X50) += dma.o sirc.o

obj-$(CONFIG_MSM_PROC_COMM) += proc_comm.o clock-pcom.o vreg.o

obj-$(CONFIG_MSM_SMD) += smd.o smd_debug.o smd_private.o remote_spinlock.o
obj-$(CONFIG_MSM_SCM) += scm.o scm-boot.o

CFLAGS_scm.o :=$(call as-instr,.arch_extension sec,-DREQUIRES_SEC=1)

obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
obj-$(CONFIG_SMP) += headsmp.o platsmp.o

obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o board-trout-mmc.o devices-msm7x00.o
obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o board-trout-mmc.o board-trout-panel.o devices-msm7x00.o
obj-$(CONFIG_MACH_HALIBUT) += board-halibut.o devices-msm7x00.o
obj-$(CONFIG_ARCH_MSM7X30) += board-msm7x30.o devices-msm7x30.o
obj-$(CONFIG_ARCH_QSD8X50) += board-qsd8x50.o devices-qsd8x50.o
obj-$(CONFIG_ARCH_MSM8X60) += board-msm8x60.o
obj-$(CONFIG_ARCH_MSM8960) += board-msm8960.o devices-msm8960.o

obj-$(CONFIG_ARCH_MSM7X30) += gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_QSD8X50) += gpiomux-8x50.o gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_MSM8X60) += gpiomux-8x60.o gpiomux-v2.o gpiomux.o

obj-$(CONFIG_ARCH_IPQ806X) += board-ipq806x.o clock-ipq806x.o
obj-$(CONFIG_ARCH_IPQ806X) += dma.o gpiomux-v2.o gpiomux.o platsmp.o
obj-$(CONFIG_ARCH_IPQ806X) += spm-ipq806x.o mpm.o
obj-$(CONFIG_ARCH_IPQ806X) += rpm-ipq806x.o
obj-$(CONFIG_ARCH_IPQ806X) += acpuclock.o
obj-$(CONFIG_ARCH_IPQ806X) += acpuclock-krait.o
obj-$(CONFIG_ARCH_IPQ806X) += acpuclock-ipq806x.o
obj-$(CONFIG_ARCH_IPQ806X) += msm-krait-l2-accessors.o
obj-$(CONFIG_ARCH_IPQ806X) += clock-local.o clock-rpm.o clock-pll.o
obj-$(CONFIG_ARCH_IPQ806X) += rb3011-voltage-regulator.o

obj-$(CONFIG_CPU_FREQ_MSM) += cpufreq.o
obj-$(CONFIG_MSM_RPM) += rpm.o rpm_resources.o rpm-debug.o
obj-$(CONFIG_MSM_SPM_V2) += spm-v2.o spm_devices.o
obj-$(CONFIG_MSM_PCIE) += pcie.o pcie_irq.o pcie-ipq806x.o
obj-$(CONFIG_CPU_V7) += idle-v7.o

obj-$(CONFIG_MSM_BUS_SCALING) += msm_bus/
