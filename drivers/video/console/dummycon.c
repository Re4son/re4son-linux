/*
 *  linux/drivers/video/dummycon.c -- A dummy console driver
 *
 *  To be used if there's no other console driver (e.g. for plain VGA text)
 *  available, usually until fbcon takes console over.
 */

#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/console.h>
#include <linux/vt_kern.h>
#include <linux/screen_info.h>
#include <linux/init.h>
#include <linux/module.h>

/*
 *  Dummy console driver
 */

#if defined(__arm__)
#define DUMMY_COLUMNS	screen_info.orig_video_cols
#define DUMMY_ROWS	screen_info.orig_video_lines
#else
/* set by Kconfig. Use 80x25 for 640x480 and 160x64 for 1280x1024 */
#define DUMMY_COLUMNS	CONFIG_DUMMY_CONSOLE_COLUMNS
#define DUMMY_ROWS	CONFIG_DUMMY_CONSOLE_ROWS
#endif

#ifdef CONFIG_FRAMEBUFFER_CONSOLE_DEFERRED_TAKEOVER
/* These are both protected by the console_lock */
static RAW_NOTIFIER_HEAD(dummycon_output_nh);
static bool dummycon_putc_called;

void dummycon_register_output_notifier(struct notifier_block *nb)
{
	raw_notifier_chain_register(&dummycon_output_nh, nb);

	if (dummycon_putc_called)
		nb->notifier_call(nb, 0, NULL);
}

void dummycon_unregister_output_notifier(struct notifier_block *nb)
{
	raw_notifier_chain_unregister(&dummycon_output_nh, nb);
}

static void dummycon_putc(struct vc_data *vc, int c, int ypos, int xpos)
{
	raw_notifier_call_chain(&dummycon_output_nh, 0, NULL);
	dummycon_putc_called = true;
}

static void dummycon_putcs(struct vc_data *vc, const unsigned short *s,
			   int count, int ypos, int xpos)
{
	raw_notifier_call_chain(&dummycon_output_nh, 0, NULL);
	dummycon_putc_called = true;
}
#else
#define dummycon_putc	(void *)dummycon_dummy
#define dummycon_putcs	(void *)dummycon_dummy
#endif

static const char *dummycon_startup(void)
{
    return "dummy device";
}

static void dummycon_init(struct vc_data *vc, int init)
{
    vc->vc_can_do_color = 1;
    if (init) {
	vc->vc_cols = DUMMY_COLUMNS;
	vc->vc_rows = DUMMY_ROWS;
    } else
	vc_resize(vc, DUMMY_COLUMNS, DUMMY_ROWS);
}

static int dummycon_dummy(void)
{
    return 0;
}

#define DUMMY	(void *)dummycon_dummy

/*
 *  The console `switch' structure for the dummy console
 *
 *  Most of the operations are dummies.
 */

const struct consw dummy_con = {
    .owner =		THIS_MODULE,
    .con_startup =	dummycon_startup,
    .con_init =		dummycon_init,
    .con_deinit =	DUMMY,
    .con_clear =	DUMMY,
    .con_putc =		dummycon_putc,
    .con_putcs =	dummycon_putcs,
    .con_cursor =	DUMMY,
    .con_scroll =	DUMMY,
    .con_switch =	DUMMY,
    .con_blank =	DUMMY,
    .con_font_set =	DUMMY,
    .con_font_default =	DUMMY,
    .con_font_copy =	DUMMY,
};
EXPORT_SYMBOL_GPL(dummy_con);
