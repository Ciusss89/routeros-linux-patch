obj-y += timer.o
obj-y += clock.o clock-voter.o clock-dummy.o

obj-$(CONFIG_SMP) += headsmp.o platsmp.o

obj-$(CONFIG_ARCH_IPQ806X) += board-ipq806x.o clock-ipq806x.o
obj-$(CONFIG_ARCH_IPQ806X) += dma.o gpiomux-v2.o gpiomux.o platsmp.o
obj-$(CONFIG_ARCH_IPQ806X) += mpm.o
obj-$(CONFIG_ARCH_IPQ806X) += rpm-ipq806x.o
obj-$(CONFIG_ARCH_IPQ806X) += acpuclock.o
obj-$(CONFIG_ARCH_IPQ806X) += acpuclock-krait.o
obj-$(CONFIG_ARCH_IPQ806X) += acpuclock-ipq806x.o
obj-$(CONFIG_ARCH_IPQ806X) += msm-krait-l2-accessors.o
obj-$(CONFIG_ARCH_IPQ806X) += clock-local.o clock-rpm.o clock-pll.o
obj-$(CONFIG_ARCH_IPQ806X) += rb3011-voltage-regulator.o

obj-$(CONFIG_CPU_FREQ_MSM) += cpufreq.o
obj-$(CONFIG_MSM_RPM) += rpm.o rpm_resources.o rpm-debug.o
obj-$(CONFIG_MSM_PCIE) += pcie.o pcie_irq.o pcie-ipq806x.o

obj-$(CONFIG_MSM_BUS_SCALING) += msm_bus/
