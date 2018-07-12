#include "plx9030.h"



static int __init init_plx9030(void){
  printk(PINFO "module start.\n");
  return 0;
}

static void __exit exit_plx9030(void){

  printk(PINFO "module exit.\n");
  return;
}

module_init(init_plx9030);
module_exit(exit_plx9030);

MODULE_LICENSE("GPL");

/* EOF */

