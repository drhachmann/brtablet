
//evtouch
//inputattach --microsoft /dev/ttyS0
/*
 *  Copyright (c) 1999-2001 Vojtech Pavlik
 */
	
/*
 *  Serial mouse driver for Linux
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Simunkova 1594, Prague 8, 182 00 Czech Republic
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/device.h>

#include "brtablet-driver.h"

#define DRIVER_DESC	"Serial Tablet Tupiniquim(brTablet) driver"

MODULE_AUTHOR("diego_hachmann@hotmail.com");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");


/* Attach the sysfs write method */
DEVICE_ATTR(data, 0644, NULL, data_store);
DEVICE_ATTR(operation, 0644, NULL, operation_store);
DEVICE_ATTR(point, 0644, point_show, NULL);
/* Attribute Descriptor */
static struct attribute *brtablet_attrs[] = {
	&dev_attr_data.attr,
	&dev_attr_operation.attr,
	&dev_attr_point.attr,
  NULL
};
/* Attribute group */
static struct attribute_group brtablet_attr_group = {
  .attrs = brtablet_attrs,
};
            
static ssize_t
point_show(struct device *dev,  struct device_attribute *attr,  char *buffer){
	struct private_data *priv= dev_get_drvdata(dev);
	int a = sprintf(buffer, "%d %d %d", priv->point_raw.x, priv->point_raw.y, priv->point_raw.z);
	buffer[a]='\0';
	return a+1;
}
      
static ssize_t
operation_store(struct device *dev, struct device_attribute *attr, const  char *buffer, size_t count){
	struct private_data *priv= dev_get_drvdata(dev);
	sscanf(buffer, "%d %d", &priv->op_code, &priv->op_value);	
	debug(2, HEAD, "operation_store %d %d\n", priv->op_code, priv->op_value);
	return count;
}
      
static ssize_t
data_store(struct device *dev, struct device_attribute *attr, const  char *buffer, size_t count){
	int j, w;
	struct private_data *priv= dev_get_drvdata(dev);
	debug(2, HEAD, "data_store ");
	switch(priv->op_code){
		case OP_MTX:
			for(j=0; j<3; j++){
				for(w=0; w<3; w++){
					sscanf(buffer, "%d", &priv->coef[priv->op_value][j][w]);	
					debug(2, NO_HEAD,"%d ",priv->coef[priv->op_value][j][w]); 
					next_int(buffer,1);
				}
			}
			break;
	
		case OP_POINT:
			for(j=0; j<5; j++){
				for(w=0; w<5; w++){
					sscanf(buffer, "%d %d %d", &priv->points[j][w].x, &priv->points[j][w].y, &priv->points[j][w].z);
					debug(2, NO_HEAD, "%d %d %d ", priv->points[j][w].x, priv->points[j][w].y, priv->points[j][w].z);
					next_int(buffer, 3);		
				}
			}
			break;

		default:
			debug(2, NO_HEAD, "Operation don't reconigzed");
			break;
			
	}
	debug(2, NO_HEAD, "\n");
 	return count;
}


static void calib(struct private_data *priv){
	int i, j, pos=0;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(point_in_triangle((priv->point_raw), priv->points[i][j], priv->points[i+1][j], priv->points[i+1][j+1])){
				pos = (i*4+j)*2;
				goto fim;
			}
			if(point_in_triangle((priv->point_raw), priv->points[i+1][j+1], priv->points[i][j+1], priv->points[i][j])){
				pos = (i*4+j)*2+1;
				goto fim;
			}
		}
	}
fim:
		priv->point_calib.x = (priv->coef[pos][0][0]*priv->point_raw.x + priv->coef[pos][0][1]*priv->point_raw.y + priv->coef[pos][0][2])>>13;
		priv->point_calib.y = (priv->coef[pos][1][0]*priv->point_raw.x + priv->coef[pos][1][1]*priv->point_raw.y + priv->coef[pos][1][2])>>13;
		priv->point_calib.z = priv->point_raw.z;
}

static irqreturn_t priv_interrupt(struct serio *serio,
		unsigned char data, unsigned int flags)
{
	static int ult=0;
	static int pos=0;
	static int vet[15];


	struct private_data *priv = serio_get_drvdata(serio);
	struct input_dev *dev = priv->dev;

	debug(8, HEAD, "interrupt data : %d\n", data);	

	vet[pos]=data;
	if(pos<4){
		pos++;
	 	return IRQ_HANDLED;
		
	}else{

		int crc = vet[4]&0x3F;
		if(((vet[0]+vet[1]+vet[2]+vet[3]+(vet[4]&192))&63) != crc){
			memmove(vet,&vet[1],4);
			return IRQ_HANDLED;			
		}else{
			debug(5, HEAD, "Error CRC");
		}
		priv->point_raw.x = vet[0]*16+(vet[1]>>4);
	  	priv->point_raw.y = (vet[1]&0xF)*256+vet[2];
		priv->point_raw.z = vet[3]*4+(vet[4]>>6);
		debug(5, HEAD, "<1>RAW %d %d %d",  priv->point_raw.x, priv->point_raw.y, priv->point_raw.z);
		if(priv->calib){
			calib(priv);
			input_report_abs(dev, ABS_X, priv->point_calib.x);
			input_report_abs(dev, ABS_Y, priv->point_calib.y);

			if(priv->point_calib.z>650 && ult==0){
				input_report_key(dev, BTN_TOUCH, 1);
				ult=1;
			}else if(priv->point_calib.z<650 && ult==1){
				input_report_key(dev, BTN_TOUCH, 0);
				ult=0;
			}
			input_sync(dev);
			debug(5, HEAD,"CALIB %d %d %d",  priv->point_calib.x, priv->point_calib.y, priv->point_calib.z);
		}else{
			debug(5, HEAD, "Don't Calibrate");	
		}
		
		
    	pos=0;
	}
	return IRQ_HANDLED;
}

/*
 * priv_disconnect() cleans up after we don't want talk
 * to the mouse anymore.
 */

static void priv_disconnect(struct serio *serio)
{
	struct private_data *priv = serio_get_drvdata(serio);
	serio_close(serio);
	serio_set_drvdata(serio, NULL);
	input_unregister_device(priv->dev);
	sysfs_remove_group(&priv->platform_dev->dev.kobj, &brtablet_attr_group);
  	platform_device_unregister(priv->platform_dev);
	kfree(priv);
	info("disconnect");

}

/*
 * priv_connect() is a callback form the serio module when
 * an unhandled serio port is found.
 */
//printk(KERN_DEBUG " - baud rate = %d", tty_get_baud_rate(tty));
static int priv_connect(struct serio *serio, struct serio_driver *drv)
{
	struct private_data *priv = NULL;
	struct input_dev *input_dev = NULL;
	unsigned char c = serio->id.extra;
	int err = -ENOMEM;

	priv = kzalloc(sizeof(struct private_data), GFP_KERNEL);
	if(!priv)
		goto fail0;

	input_dev = input_allocate_device();
	if (!input_dev)
		goto fail1;

	priv->dev = input_dev;
	snprintf(priv->phys, sizeof(priv->phys), "%s/input0", serio->phys);
	priv->type = serio->id.proto;
	priv->calib = 0;
	input_dev->name = "brTablet";
	input_dev->phys = priv->phys;
	input_dev->id.bustype = BUS_RS232;
	input_dev->id.vendor  = priv->type;
	input_dev->id.product = c;
	input_dev->id.version = 0x0100;
	input_dev->dev.parent = &serio->dev;

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	
	input_set_abs_params(input_dev, ABS_X, 0, 1024, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, 768, 0, 0);

	priv->platform_dev = platform_device_register_simple("brtablet", -1, NULL, 0);

	dev_set_drvdata(&priv->platform_dev->dev, priv);
	serio_set_drvdata(serio, priv);

	if (IS_ERR(priv->platform_dev)) {
		PTR_ERR(priv->platform_dev);
		info("brtablet_init: error\n");
  }
  /* Create a sysfs node to read simulated coordinates */
	sysfs_create_group(&priv->platform_dev->dev.kobj, &brtablet_attr_group);
	

	err = serio_open(serio, drv);
	if (err)
		goto fail2;

	err = input_register_device(priv->dev);
	if (err)
		goto fail3;

	info("connect");
	return 0;

	fail3:	serio_close(serio);
	fail2:	serio_set_drvdata(serio, NULL);
	fail1:	input_free_device(input_dev);
	fail0:	
	kfree(priv);	

	return err;
}

static struct serio_device_id priv_serio_ids[] = {
	{
		.type	= SERIO_RS232,
		.proto	= SERIO_MS,//SERIO_H3600,
		.id	= SERIO_ANY,
		.extra	= SERIO_ANY,
	},
	{ 0 }
};

MODULE_DEVICE_TABLE(serio, priv_serio_ids);

static struct serio_driver priv_drv = {
	.driver		= {
		.name	= "brTablet",
	},
	.description	= DRIVER_DESC,
	.id_table	= priv_serio_ids,
	.interrupt	= priv_interrupt,
	.connect	= priv_connect,
	.disconnect	= priv_disconnect,
};

static int __init priv_init(void)
{
	info("init");
	return serio_register_driver(&priv_drv);
}

static void __exit priv_exit(void)
{
	info("exit");
	serio_unregister_driver(&priv_drv);
}

module_init(priv_init);
module_exit(priv_exit);

