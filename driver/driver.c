// SPDX-License-Identifier: GPL-2.0+
/*
 * cdc-acm-minimal.c
 *
 * Minimalist driver to send and receive data via USB serial for an Arduino Mega.
 *
 * Based on the original cdc-acm.c driver.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/mutex.h>
#include <linux/usb/ch9.h>
#include <linux/usb/cdc.h>
#include <linux/slab.h> // For kzalloc and kfree
#include <linux/tty_flip.h> // For tty_insert_flip_string and tty_flip_buffer_push

#define DRIVER_AUTHOR "Modified by User"
#define DRIVER_DESC "Minimal USB ACM driver for Arduino Mega"
#define ACM_TTY_MAJOR 166
#define ACM_TTY_MINORS 1

static struct usb_driver acm_driver;
static struct tty_driver *acm_tty_driver;

struct acm {
    struct usb_device *dev;          // USB device
    struct usb_interface *control;  // Control interface
    struct urb *ctrlurb;            // Control URB
    struct urb *read_urb;           // Read URB
    struct urb *write_urb;          // Write URB
    struct usb_endpoint_descriptor *epwrite; // Write endpoint
    struct usb_endpoint_descriptor *epread;  // Read endpoint
    int writesize;                  // Write buffer size
    int readsize;                   // Read buffer size
    char *write_buffer;             // Write buffer
    char *read_buffer;              // Read buffer
    dma_addr_t write_dma;           // DMA for writing
    dma_addr_t read_dma;            // DMA for reading
    struct tty_port tty_port;       // TTY port
};

// Callback for received data
static void acm_read_bulk_callback(struct urb *urb) {
    struct acm *acm = urb->context;
    struct tty_port *port = &acm->tty_port;

    if (urb->status) {
        pr_err("Receive data error: %d\n", urb->status);
        return;
    }

    tty_insert_flip_string(port, urb->transfer_buffer, urb->actual_length);
    tty_flip_buffer_push(port);

    // Resubmit the URB for continuous reception
    usb_submit_urb(urb, GFP_ATOMIC);
}

// Callback for write operation
static void acm_write_bulk_callback(struct urb *urb) {
    if (urb->status) {
        pr_err("Write error: %d\n", urb->status);
    }
}

// Write data to Arduino Mega
static int acm_tty_write(struct tty_struct *tty, const unsigned char *buf, int count) {
    struct acm *acm = tty->driver_data;

    count = min_t(size_t, count, acm->writesize);
    memcpy(acm->write_buffer, buf, count);

    usb_fill_bulk_urb(acm->write_urb, acm->dev,
                      usb_sndbulkpipe(acm->dev, acm->epwrite->bEndpointAddress),
                      acm->write_buffer, count, acm_write_bulk_callback, acm);

    if (usb_submit_urb(acm->write_urb, GFP_KERNEL)) {
        pr_err("Failed to send data to Arduino Mega\n");
        return -EIO;
    }

    return count;
}

// Probe function
static int acm_probe(struct usb_interface *intf, const struct usb_device_id *id) {
    struct usb_endpoint_descriptor *epwrite = NULL, *epread = NULL;
    struct acm *acm;
    int retval, i;

    // Detect endpoints dynamically
    struct usb_host_interface *iface_desc = intf->cur_altsetting;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        struct usb_endpoint_descriptor *endpoint = &iface_desc->endpoint[i].desc;
        if (usb_endpoint_is_bulk_in(endpoint))
            epread = endpoint;
        if (usb_endpoint_is_bulk_out(endpoint))
            epwrite = endpoint;
    }

    if (!epread || !epwrite) {
        pr_err("No valid endpoints found\n");
        return -ENODEV;
    }

    acm = kzalloc(sizeof(*acm), GFP_KERNEL);
    if (!acm)
        return -ENOMEM;

    acm->dev = interface_to_usbdev(intf);
    acm->control = intf;
    acm->epwrite = epwrite;
    acm->epread = epread;
    acm->writesize = usb_endpoint_maxp(epwrite);
    acm->readsize = usb_endpoint_maxp(epread);

    acm->write_buffer = usb_alloc_coherent(acm->dev, acm->writesize, GFP_KERNEL, &acm->write_dma);
    acm->read_buffer = usb_alloc_coherent(acm->dev, acm->readsize, GFP_KERNEL, &acm->read_dma);
    if (!acm->write_buffer || !acm->read_buffer) {
        kfree(acm);
        return -ENOMEM;
    }

    acm->write_urb = usb_alloc_urb(0, GFP_KERNEL);
    acm->read_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!acm->write_urb || !acm->read_urb) {
        usb_free_coherent(acm->dev, acm->writesize, acm->write_buffer, acm->write_dma);
        usb_free_coherent(acm->dev, acm->readsize, acm->read_buffer, acm->read_dma);
        kfree(acm);
        return -ENOMEM;
    }

    usb_fill_bulk_urb(acm->read_urb, acm->dev,
                      usb_rcvbulkpipe(acm->dev, epread->bEndpointAddress),
                      acm->read_buffer, acm->readsize,
                      acm_read_bulk_callback, acm);

    usb_set_intfdata(intf, acm);

    retval = usb_submit_urb(acm->read_urb, GFP_KERNEL);
    if (retval) {
        pr_err("Failed to initialize data reception\n");
        return retval;
    }

    tty_port_init(&acm->tty_port);
    tty_port_register_device(&acm->tty_port, acm_tty_driver, 0, &intf->dev);

    pr_info("Minimal USB ACM driver loaded\n");
    return 0;
}

// Disconnect function
static void acm_disconnect(struct usb_interface *intf) {
    struct acm *acm = usb_get_intfdata(intf);

    usb_set_intfdata(intf, NULL);

    if (acm) {
        usb_kill_urb(acm->read_urb);
        usb_kill_urb(acm->write_urb);
        usb_free_urb(acm->read_urb);
        usb_free_urb(acm->write_urb);
        usb_free_coherent(acm->dev, acm->writesize, acm->write_buffer, acm->write_dma);
        usb_free_coherent(acm->dev, acm->readsize, acm->read_buffer, acm->read_dma);
        tty_unregister_device(acm_tty_driver, 0);
        tty_port_destroy(&acm->tty_port);
        kfree(acm);
    }

    pr_info("Minimal USB ACM driver unloaded\n");
}

// TTY operations
static const struct tty_operations acm_ops = {
    .write = acm_tty_write,
};

// Supported device IDs
static const struct usb_device_id acm_ids[] = {
    { USB_DEVICE(0x2341, 0x0042) }, // Arduino Mega
    {}
};
MODULE_DEVICE_TABLE(usb, acm_ids);

// USB driver configuration
static struct usb_driver acm_driver = {
    .name = "cdc_acm_minimal",
    .probe = acm_probe,
    .disconnect = acm_disconnect,
    .id_table = acm_ids,
};

// Module initialization
static int __init acm_init(void) {
    int retval;

    acm_tty_driver = tty_alloc_driver(ACM_TTY_MINORS, TTY_DRIVER_REAL_RAW);
    if (IS_ERR(acm_tty_driver))
        return PTR_ERR(acm_tty_driver);

    acm_tty_driver->name = "ttyACM";
    acm_tty_driver->major = ACM_TTY_MAJOR;
    acm_tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
    tty_set_operations(acm_tty_driver, &acm_ops);

    retval = tty_register_driver(acm_tty_driver);
    if (retval) {
        tty_driver_kref_put(acm_tty_driver);
        return retval;
    }

    retval = usb_register(&acm_driver);
    if (retval) {
        tty_unregister_driver(acm_tty_driver);
        tty_driver_kref_put(acm_tty_driver);
        return retval;
    }

    pr_info("Minimal USB ACM driver initialized\n");
    return 0;
}

// Module cleanup
static void __exit acm_exit(void) {
    usb_deregister(&acm_driver);
    tty_unregister_driver(acm_tty_driver);
    tty_driver_kref_put(acm_tty_driver);
    pr_info("Minimal USB ACM driver removed\n");
}

module_init(acm_init);
module_exit(acm_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
