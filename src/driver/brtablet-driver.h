#ifndef _BR_TABLET_DRIVER_
#define _BR_TABLET_DRIVER_

#define HEAD 1
#define NO_HEAD 2



#define info(...)\
	printk("brTablet: ");\
	printk(__VA_ARGS__);\
	printk("\n");

#ifdef DEBUG 
  #define debug(_HEAD, _LEVEL, ...)\
    if(_LEVEL <= DEBUG_LEVEL){\
      if(_HEAD==HEAD)\
	      printk("brTablet DEBUG: ");\
	    printk(__VA_ARGS__);\
    }
#else
 #define debug(...) while(0); 
#endif


#define sinal_Z(p1,p2,p3)\
	(((p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p3.x*p2.y - p2.x*p1.y - p1.x*p3.y)>0)?(1):(-1))

#define point_in_triangle(point,p1,p2,p3)\
	(sinal_Z(point,p1,p2)>0 && sinal_Z(point,p2,p3)>0 && sinal_Z(point,p3,p1)>0)

#define next_int(vet, num){\
	int i;\
	for(i=0; i<num; i++){\
		while((*vet>='0' && *vet<='9')||*vet=='-')vet++;\
	vet++;}}

#define OP_MTX 1
#define OP_POINT 2
#define OP_CALIBRATED 3

typedef struct Point{
	int x, y, z;
}Point;


struct private_data {
	struct input_dev *dev;
	struct platform_device *platform_dev; 
	signed char buf[8];
	unsigned char count;
	unsigned char type;
	unsigned long last;
	char phys[32];
	int coef[4*4*2][3][3];
	Point points[5][5];
	Point point_raw;
	Point point_calib;
	int calib;
	int op_code;
	int op_value;
};

static void calib(struct private_data *priv);

static ssize_t
data_store(struct device *dev, struct device_attribute *attr,  const  char *buffer, size_t count);

static ssize_t
operation_store(struct device *dev, struct device_attribute *attr, const  char *buffer, size_t count);

static ssize_t
point_show(struct device *dev,  struct device_attribute *attr,  char *buffer);


#endif

//priv->point_raw.x = 2000; priv->point_raw.y=2000;
		//calib(priv);
		/*printk("%d %d\n", priv->point_calib.x, priv->point_calib.y);
		printk("%d %d\n", priv2->points[0][0].x, priv2->points[0][0].y);
		printk("%d %d\n", priv->points[0][0].x, priv->points[0][0].y);*/
		//calib2(&priv->point_raw, &priv->point_calib);
		//printk("%d %d\n", priv->point_calib.x, priv->point_calib.y);
		//cnt_points+=25;		
		//priv->calib=1;
