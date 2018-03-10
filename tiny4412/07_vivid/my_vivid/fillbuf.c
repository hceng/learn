
static struct v4l2_format my_vivid_format;

/* Bars and Colors should match positions */
enum colors {
	WHITE,
	AMBAR,
	CYAN,
	GREEN,
	MAGENTA,
	RED,
	BLUE,
	BLACK,
};

/* R   G   B */
#define COLOR_WHITE	{204, 204, 204}
#define COLOR_AMBAR	{208, 208,   0}
#define COLOR_CIAN	{  0, 206, 206}
#define	COLOR_GREEN	{  0, 239,   0}
#define COLOR_MAGENTA	{239,   0, 239}
#define COLOR_RED	{205,   0,   0}
#define COLOR_BLUE	{  0,   0, 255}
#define COLOR_BLACK	{  0,   0,   0}

struct bar_std {
	u8 bar[8][3];
};

/* Maximum number of bars are 10 - otherwise, the input print code should be modified */
static struct bar_std bars[] = {
	{	/* Standard ITU-R color bar sequence */
		{
			COLOR_WHITE,
			COLOR_AMBAR,
			COLOR_CIAN,
			COLOR_GREEN,
			COLOR_MAGENTA,
			COLOR_RED,
			COLOR_BLUE,
			COLOR_BLACK,
		}
	}, {
		{
			COLOR_WHITE,
			COLOR_AMBAR,
			COLOR_BLACK,
			COLOR_WHITE,
			COLOR_AMBAR,
			COLOR_BLACK,
			COLOR_WHITE,
			COLOR_AMBAR,
		}
	}, {
		{
			COLOR_WHITE,
			COLOR_CIAN,
			COLOR_BLACK,
			COLOR_WHITE,
			COLOR_CIAN,
			COLOR_BLACK,
			COLOR_WHITE,
			COLOR_CIAN,
		}
	}, {
		{
			COLOR_WHITE,
			COLOR_GREEN,
			COLOR_BLACK,
			COLOR_WHITE,
			COLOR_GREEN,
			COLOR_BLACK,
			COLOR_WHITE,
			COLOR_GREEN,
		}
	},
};

#define NUM_INPUTS ARRAY_SIZE(bars)

#define TO_Y(r, g, b) \
	(((16829 * r + 33039 * g + 6416 * b  + 32768) >> 16) + 16)
/* RGB to  V(Cr) Color transform */
#define TO_V(r, g, b) \
	(((28784 * r - 24103 * g - 4681 * b  + 32768) >> 16) + 128)
/* RGB to  U(Cb) Color transform */
#define TO_U(r, g, b) \
	(((-9714 * r - 19070 * g + 28784 * b + 32768) >> 16) + 128)

static unsigned char myvivi_cur_bars[8][3];

/* precalculate color bar values to speed up rendering */
static void my_vivid_precalculate_bars(int input)
{
	unsigned char r, g, b;
	int k, is_yuv;


	for (k = 0; k < 8; k++) {
		r = bars[input].bar[k][0];
		g = bars[input].bar[k][1];
		b = bars[input].bar[k][2];
		is_yuv = 0;

		switch (my_vivid_format.fmt.pix.pixelformat) {
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_UYVY:
			is_yuv = 1;
			break;
		case V4L2_PIX_FMT_RGB565:
		case V4L2_PIX_FMT_RGB565X:
			r >>= 3;
			g >>= 2;
			b >>= 3;
			break;
		case V4L2_PIX_FMT_RGB555:
		case V4L2_PIX_FMT_RGB555X:
			r >>= 3;
			g >>= 3;
			b >>= 3;
			break;
		}

		if (is_yuv) {
			myvivi_cur_bars[k][0] = TO_Y(r, g, b);	/* Luma */
			myvivi_cur_bars[k][1] = TO_U(r, g, b);	/* Cb */
			myvivi_cur_bars[k][2] = TO_V(r, g, b);	/* Cr */
		} else {
			myvivi_cur_bars[k][0] = r;
			myvivi_cur_bars[k][1] = g;
			myvivi_cur_bars[k][2] = b;
		}
	}

}


static void my_vivid_gen_twopix(unsigned char *buf, int colorpos)
{
	unsigned char r_y, g_u, b_v;
	unsigned char *p;
	int color;

	printk("enter %s\n", __func__);
	
	r_y = myvivi_cur_bars[colorpos][0]; /* R or precalculated Y */
	g_u = myvivi_cur_bars[colorpos][1]; /* G or precalculated U */
	b_v = myvivi_cur_bars[colorpos][2]; /* B or precalculated V */

	for (color = 0; color < 4; color++) {
		p = buf + color;

		switch (my_vivid_format.fmt.pix.pixelformat) {
		case V4L2_PIX_FMT_YUYV:
			switch (color) {
			case 0:
			case 2:
				*p = r_y;
				break;
			case 1:
				*p = g_u;
				break;
			case 3:
				*p = b_v;
				break;
			}
			break;
		case V4L2_PIX_FMT_UYVY:
			switch (color) {
			case 1:
			case 3:
				*p = r_y;
				break;
			case 0:
				*p = g_u;
				break;
			case 2:
				*p = b_v;
				break;
			}
			break;
		case V4L2_PIX_FMT_RGB565:
			switch (color) {
			case 0:
			case 2:
				*p = (g_u << 5) | b_v;
				break;
			case 1:
			case 3:
				*p = (r_y << 3) | (g_u >> 3);
				break;
			}
			break;
		case V4L2_PIX_FMT_RGB565X:
			switch (color) {
			case 0:
			case 2:
				*p = (r_y << 3) | (g_u >> 3);
				break;
			case 1:
			case 3:
				*p = (g_u << 5) | b_v;
				break;
			}
			break;
		case V4L2_PIX_FMT_RGB555:
			switch (color) {
			case 0:
			case 2:
				*p = (g_u << 5) | b_v;
				break;
			case 1:
			case 3:
				*p = (r_y << 2) | (g_u >> 3);
				break;
			}
			break;
		case V4L2_PIX_FMT_RGB555X:
			switch (color) {
			case 0:
			case 2:
				*p = (r_y << 2) | (g_u >> 3);
				break;
			case 1:
			case 3:
				*p = (g_u << 5) | b_v;
				break;
			}
			break;
		}
	}
}

static void my_vivid_gen_line(char *basep, int inipos, int wmax,
		int hmax, int line, int count)
{
	int  w;
	int pos = inipos;

	printk("enter %s\n", __func__);
	
	/* We will just duplicate the second pixel at the packet */
	wmax /= 2;

	/* Generate a standard color bar pattern */
	for (w = 0; w < wmax; w++) {
		int colorpos = ((w + count) * 8/(wmax + 1)) % 8;

		my_vivid_gen_twopix(basep + pos, colorpos);
		pos += 4; /* only 16 bpp supported for now */
	}
	return;
}

static void my_vivid_fillbuff(struct videobuf_buffer *vb)
{
	int h , pos = 0;
	int hmax  = vb->height;
	int wmax  = vb->width;
	char *tmpbuf;
	void *vbuf = videobuf_to_vmalloc(vb);
    static int mv_count = 0;

	printk("enter %s\n", __func__);
	
	if (!vbuf)
		return;

	tmpbuf = kmalloc(wmax * 2, GFP_ATOMIC);
	if (!tmpbuf)
		return;

	for (h = 0; h < hmax; h++) {
		my_vivid_gen_line(tmpbuf, 0, wmax, hmax, h, mv_count);
		memcpy(vbuf + pos, tmpbuf, wmax * 2);
		pos += wmax*2;
	}

	mv_count++;

	kfree(tmpbuf);
}


