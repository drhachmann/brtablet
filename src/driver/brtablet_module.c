#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>

//http://projects.qi-hardware.com/index.php/p/qi-kernel/source/tree/jz-2.6.35/drivers/macintosh/mac_hid.c
//depmod -a
//modprobe "NOME"
MODULE_LICENSE("Dual BSD/GPL");

#define X_AXIS_MAX 3830
#define X_AXIS_MIN 150
#define Y_AXIS_MAX 3830
#define Y_AXIS_MIN 190
#define PRESSURE_MIN 0
#define PRESSURE_MAX 10

static int cnt_coef=0;
static int cnt_points=0;
int coef[4*4*2][3][3];
int val;
int temp;

typedef struct Point{
	int x, y, z;
}Point;

Point points[5][5];
int j, w;

struct input_dev *vms_input_dev;        /* Representation of an input device */
static struct platform_device *vms_dev; /* Device structure */
                                        /* Sysfs method to input simulated
                                            coordinates to the virtual
                                            mouse driver */
static ssize_t
write_vms(struct device *dev,
          struct device_attribute *attr,
          const  char *buffer, size_t count){

	//if(count==4 && buffer[0]==(char)255 && buffer[1]==(char)255 && buffer[2]==(char)255 && buffer[3]==(char)255){
		printk("<1>K");
	//	cnt=0;
	//}else{
	//	printk("%s\n", buffer);

		if(cnt_coef<4*4*2){
			for(j=0; j<3; j++){
				for(w=0; w<3; w++){
					sscanf(buffer, "%d", &coef[cnt_coef][j][w]);		
					while(*buffer!=' ')buffer++;
					buffer++;
				}
			}
			//printk("<1>%d %d %d\n", coef[cnt_coef][0][0], coef[cnt_coef][0][1], coef[cnt_coef][0][2]);
			cnt_coef++;
		}else if(cnt_points<25){
			for(j=0; j<5; j++){
				for(w=0; w<5; w++){
					sscanf(buffer, "%d %d %d", &points[j][w].x, &points[j][w].y, &points[j][w].z);
					temp=0;

					while(temp<3){
						buffer++;
						if(*buffer==' ')temp++;;
					}
					buffer++;		
					//printk("<1>P %d %d %d\n", points[j][w].x, points[j][w].y, points[j][w].z);		
				}
			}	
		cnt_points+=25;		
	}
  return count;
}

static ssize_t
read_vms(struct device *dev,
          struct device_attribute *attr,
          char *buffer){
	printk("<1> READ");
	sprintf("DIEGO", buffer);
  return 5;
}

/*
int sinal_Z(Point *p1,  Point *p2, Point *p3){
	if((p1->x*p2->y + p2->x*p3->y + p3->x*p1->y - p3->x*p2->y - p2->x*p1->y - p1->x*p3->y)>0)return 1;
	else return -1;
}*/

/*Soma dos triângulos internos igual a soma do triângulo externo*/
/*int point_in_triangle(Point *point,  Point *p1, Point *p2,  Point *p3){
		if(sinal_Z(point, p1, p2)>0 && sinal_Z(point, p2, p3) >0 &&sinal_Z(point, p3, p1)>0)
//	if(ABS_(get_area(point, p1, p2) + get_area(point, p2, p3) + get_area(point, p3, p1) - get_area(p1, p2, p3)) < EPSILON)
		return(1);
	return(0);
}*/
/*
void calib(Point *uv, Point *xy){
	int i, j, k, pos=0;
	for(i=0; i<4; i++){
		for(i=0; i<4; i++){
			if(point_in_triangle(uv,points[i][j], points[i+1][j], points[i+1][j+1])){
				pos = (i*4+j)*2;
				goto fim;
			}
			if(point_in_triangle(uv, points[i+1][j+1], points[i][j+1], points[i][j])){
				pos = (i*4+j)*2+1;
				goto fim;
			}
		}
	}
fim:
	
		xy->x = (coef[pos][0][0]*uv->x + coef[pos][0][1]*uv->y + coef[pos][0][2])<<13;
		xy->y = (coef[pos][1][0]*uv->x + coef[pos][1][1]*uv->y + coef[pos][1][2])<<13;

}
*/

ssize_t memory_read(struct file *filp, char *buf,
                    size_t count, loff_t *f_pos) {
  //  static int write=0;
	printk("READ");
//   printk("<1>memory_read %d %lld\n", count, *f_pos);
//	 printk("<1>Val : %d\n", buff[count]);
  // buf[0]=0;
   return (2);
  
}

/* Attach the sysfs write method */
DEVICE_ATTR(coordinates3, 0644, read_vms, write_vms);
/* Attribute Descriptor */
static struct attribute *vms_attrs[] = {
  &dev_attr_coordinates3.attr,
  NULL
};
/* Attribute group */
static struct attribute_group vms_attr_group = {
  .attrs = vms_attrs,
};



/* Driver Initialization */
int __init
vms_init(void)
{
	
	printk("<1>vms_init\n");
  /* Register a platform device */
  vms_dev = platform_device_register_simple("vms", -1, NULL, 0);
  if (IS_ERR(vms_dev)) {
    PTR_ERR(vms_dev);
    printk("vms_init: error\n");
  }
  /* Create a sysfs node to read simulated coordinates */
  sysfs_create_group(&vms_dev->dev.kobj, &vms_attr_group);
  /* Allocate an input device data structure */
  vms_input_dev = input_allocate_device();
  if (!vms_input_dev) {
    printk("Bad input_alloc_device()\n");
  }


  vms_input_dev->name = "vms";


  /* Register with the input subsystem */
  input_register_device(vms_input_dev);
	
  printk("Virtual Mouse Driver Initialized.\n");
  return 0;
}
/* Driver Exit */
void
vms_cleanup(void)
{
  /* Unregister from the input subsystem */
  input_unregister_device(vms_input_dev);
  /* Cleanup sysfs node */
  sysfs_remove_group(&vms_dev->dev.kobj, &vms_attr_group);
  /* Unregister driver */
  platform_device_unregister(vms_dev);
  return;
}




module_init(vms_init);
module_exit(vms_cleanup);

