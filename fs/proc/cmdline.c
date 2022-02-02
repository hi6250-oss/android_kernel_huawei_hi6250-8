#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

static char new_command_line[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", saved_command_line);
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void replace_flag(char *cmd, const char *flag, const char *flag_new)
{
       char *start_addr, *end_addr;

       // Ensure all instances of a flag are replaced
       while ((start_addr = strstr(cmd, flag))) {
               end_addr = strchr(start_addr, ' ');
               if (end_addr)
                       memcpy(start_addr, flag_new, strlen(flag));
               else
                       *(start_addr - 1) = '\0';
       }
}

static void replace_erecovery_flags(char *cmd)
{

	replace_flag(cmd, "enter_erecovery=1", "enter_erecovery=0");
}

static int __init proc_cmdline_init(void)
{
	static char new_command_line[COMMAND_LINE_SIZE];
	strcpy(new_command_line, saved_command_line);

	/* Huawei eRecovery hates our data filesystem and trends to appear
	 * after multiple reboots. Since this is a bit useless and
	 * annoying, always force the enter_erecovery flag to 0.
	 */
	replace_erecovery_flags(new_command_line);

	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
fs_initcall(proc_cmdline_init);
