#include <stdlib.h>
#include <pciaccess.h>

#include "ddekit/assert.h"
#include "ddekit/printf.h"
#include "ddekit/pci.h"
#include "config.h"

#define dbg_this 0

#define MAX_PCI_DEVS 32

/** PCI descriptor */
typedef struct ddekit_pci_dev {
	int bus;                      /**< bus ID */
	int slot;                     /**< slot ID */
	int func;                     /**< function */
	struct pci_device *dev;
	struct ddekit_pci_dev *next;  /**< chaining info */
} ddekit_pci_dev_t;

static ddekit_pci_dev_t ddekit_pci_bus[MAX_PCI_DEVS];

static inline int invalid_device(ddekit_pci_dev_t *d)
{
	return d->slot == -1;
}


/** Initialize DDEKit PCI module.
 *
 * \ingroup DDEKit_pci
 *
 * This function builds a list of devices by querying L4IO.
 */
void ddekit_pci_init(void)
{
	int slots_found = 0;
	int i;
	struct pci_device *pci_dev;
	struct pci_device_iterator *dev_iter;

	/* Init device list */
	for (i = 0; i < MAX_PCI_DEVS; i++)
		ddekit_pci_bus[i].slot = -1;

	dev_iter = pci_slot_match_iterator_create (NULL);
	while ((pci_dev = pci_device_next (dev_iter)) != NULL) {
		if (slots_found == MAX_PCI_DEVS) {
			ddekit_printf ("find more than %d pci devices",
				       slots_found);
			break;
		}
		/* Pretend all our devices are chained to exactly one bus. */
		ddekit_pci_bus[slots_found].bus   = 0; /*l4dev.bus;*/
		ddekit_pci_bus[slots_found].slot  = slots_found;
		ddekit_pci_bus[slots_found].func  = 0;
		ddekit_pci_bus[slots_found].dev = pci_dev;

		++slots_found;
	}
	pci_iterator_destroy (dev_iter);
}


/**
 * Get PCI device (bus, slot, func) for handle.
 *
 * \ingroup DDEKit_pci
 */
int ddekit_pci_get_device(int nr, int *bus, int *slot, int *func)
{
	ddekit_pci_dev_t *dev;

	ddekit_printf ("searching for dev #%d", nr);

	if (nr >= 0 && nr < MAX_PCI_DEVS && !invalid_device(&ddekit_pci_bus[nr])) {
		dev = &ddekit_pci_bus[nr];
		*bus = dev->bus;
		*slot = dev->slot;
		*func = dev->func;
		return 0;
	}

	return -1;
}


ddekit_pci_dev_t *ddekit_pci_find_device(int *bus, int *slot, int *func,
                                         ddekit_pci_dev_t *start)
{
	Assert(bus);
	Assert(slot);
	Assert(func);

	ddekit_printf ("start %p (slot %d)",
		       start, start ? start->slot : -1);
	int idx = start ? start->slot + 1 : 0;
	
	for ( ; idx < MAX_PCI_DEVS; ++idx) {
	    ddekit_pci_dev_t *dev = &ddekit_pci_bus[idx];
		if (!invalid_device(dev) &&
			(*bus == DDEKIT_PCI_ANY_ID || dev->bus == *bus) &&
		    (*slot == DDEKIT_PCI_ANY_ID || dev->slot == *slot) &&
		    (*func == DDEKIT_PCI_ANY_ID || dev->func == *func)) {
			*bus = dev->bus;
			*slot = dev->slot;
			*func = dev->func;
			return dev;
		}
	}

	return NULL;
}

int ddekit_pci_read(int bus, int slot, int func, int pos, int len, ddekit_uint32_t *val)
{
	switch(len)
	{
		case 1:
			return ddekit_pci_readb(bus, slot, func, pos, (ddekit_uint8_t *)val);
		case 2:
			return ddekit_pci_readw(bus, slot, func, pos, (ddekit_uint16_t *)val);
		case 4:
			return ddekit_pci_readl(bus, slot, func, pos, val);
	}

	return -1;
}

int ddekit_pci_write(int bus, int slot, int func, int pos, int len, ddekit_uint32_t val)
{
	switch(len)
	{
		case 1:
			return ddekit_pci_writeb(bus, slot, func, pos, val);
		case 2:
			return ddekit_pci_writew(bus, slot, func, pos, val);
		case 4:
			return ddekit_pci_writel(bus, slot, func, pos, val);
	}

	return -1;
}


/** Find PCI device for a given (bus, slot, func) tuple.
*
* \ingroup DDEKit_pci
 *
 * This function is only used internally by the pci_{read,write}{b,l,w} functions.
 * It was once called pci_find_device, but this caused confusion with the "real"
 * find_device() functionality provided by L4IO and the Linux PCI subsystem.
 */
static inline ddekit_pci_dev_t *ddekit_pci_find_device_fixed(int bus, int slot, int func)
{
	return &ddekit_pci_bus[slot];
}


int ddekit_pci_readb (int bus, int slot, int func, int pos, ddekit_uint8_t  *val)
{
	ddekit_pci_dev_t *dev = ddekit_pci_find_device_fixed(bus, slot, func);
	if (dev)
		return pci_device_cfg_read_u8 (dev->dev, val, pos);
	else
		return -1;
}


int ddekit_pci_readw (int bus, int slot, int func, int pos, ddekit_uint16_t *val)
{
	ddekit_pci_dev_t *dev = ddekit_pci_find_device_fixed(bus, slot, func);
	if (dev)
		return pci_device_cfg_read_u16 (dev->dev, val, pos);
	else
		return -1;
}


int ddekit_pci_readl (int bus, int slot, int func, int pos, ddekit_uint32_t *val)
{
	ddekit_pci_dev_t *dev = ddekit_pci_find_device_fixed(bus, slot, func);
	if (dev)
		return pci_device_cfg_read_u32 (dev->dev, val, pos);
	else
		return -1;
}


int ddekit_pci_writeb(int bus, int slot, int func, int pos, ddekit_uint8_t   val)
{
	ddekit_pci_dev_t *dev = ddekit_pci_find_device_fixed(bus, slot, func);
	if (dev)
		return pci_device_cfg_write_u8 (dev->dev, val, pos);
	else
		return -1;
}


int ddekit_pci_writew(int bus, int slot, int func, int pos, ddekit_uint16_t  val)
{
	ddekit_pci_dev_t *dev = ddekit_pci_find_device_fixed(bus, slot, func);
	if (dev)
		return pci_device_cfg_write_u16 (dev->dev, val, pos);
	else
		return -1;
}


int ddekit_pci_writel(int bus, int slot, int func, int pos, ddekit_uint32_t  val)
{
	ddekit_pci_dev_t *dev = ddekit_pci_find_device_fixed(bus, slot, func);
	if (dev)
		return pci_device_cfg_write_u32 (dev->dev, val, pos);
	else
		return -1;
}

int ddekit_pci_enable_device(struct ddekit_pci_dev *dev)
{
	pci_device_enable (dev->dev);
	return 0;
}

int ddekit_pci_disable_device(struct ddekit_pci_dev *dev)
{
	// TODO
	return -1;
}

/********************************************************************************
 ** Accessor functions for the various kinds of information that are available **
 ** from struct l4io_pci_dev (which is not handed through to the DDEs, because **
 ** it still contains L4 types).                                               **
 ********************************************************************************/

/**
 * Get PCI device vendor ID.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
unsigned short ddekit_pci_get_vendor(struct ddekit_pci_dev *dev)
{
	return dev->dev->vendor_id;
}


/**
 * Get PCI device ID.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
unsigned short ddekit_pci_get_device_id(struct ddekit_pci_dev *dev)
{
	return dev->dev->device_id;
}


/**
 * Get PCI device subvendor ID.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
unsigned short ddekit_pci_get_sub_vendor(struct ddekit_pci_dev *dev)
{
	return dev->dev->subvendor_id;
}


/**
 * Get PCI device subdevice ID.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
unsigned short ddekit_pci_get_sub_device(struct ddekit_pci_dev *dev)
{
	return dev->dev->subdevice_id;
}


/**
 * Get PCI device class ID.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
unsigned       ddekit_pci_get_dev_class(struct ddekit_pci_dev *dev)
{
	return dev->dev->device_class;
}


/**
 * Get PCI device IRQ number.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
unsigned long  ddekit_pci_get_irq(struct ddekit_pci_dev *dev)
{
	return dev->dev->irq;
}


/**
 * Get PCI device name.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
char *ddekit_pci_get_name(struct ddekit_pci_dev *dev)
{
	// TODO
	return NULL;
}


/**
 * Get PCI device slot name.
 *
 * \param dev     device
 *
 * \ingroup DDEKit_pci
 */
char *ddekit_pci_get_slot_name(struct ddekit_pci_dev *dev)
{
	// TODO
	return NULL;
}


/**
 * Get PCI device resource at index.
 *
 * \param dev     device
 * \param idx     resource index
 *
 * \ingroup DDEKit_pci
 */
ddekit_pci_res_t *ddekit_pci_get_resource(struct ddekit_pci_dev *dev, unsigned int idx)
{
	// TODO
//	if (idx > L4IO_PCIDEV_RES)
		return NULL;

//	return (ddekit_pci_res_t *)(&(dev->l4dev.res[idx]));
}


/**
 * Set PCI bus mastering for device.
 *
 * \ingroup DDEKit_pci
 */
void ddekit_pci_set_master(struct ddekit_pci_dev *dev)
{
	//TODO l4io_pci_set_master(dev->l4dev.handle);
}

int ddekit_pci_irq_enable(int bus, int slot, int func, int pin, int *irq)
{
	return 0;
}
