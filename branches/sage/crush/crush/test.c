
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "crush.h"
#include "mapper.h"
#include "builder.h"


int main()
{
  int sub[10];
  int subw[10];
  int i, j;
  int d;
  int o[100];
  int root;
  int ruleno;
  int r[10];
  
  int uw[10] = { 10, 10, 5, 10, 20, 10, 10, 30, 10, 5 };

  struct crush_bucket *b;
  struct crush_rule *rule;
  
  struct crush_map *map = crush_create();

  d = 0;
  for (i=0; i<10; i++) {
    for (j=0; j<10; j++)
      o[j] = d++;
    b = (struct crush_bucket*)crush_make_uniform_bucket(1, 1, o, uw[i]);
    sub[i] = crush_add_bucket(map, b);
    subw[i] = b->weight;
    printf("make bucket %d weight %d\n", sub[i], subw[i]);
  }

  root = crush_add_bucket(map, (struct crush_bucket*)crush_make_straw_bucket(2, 10, sub, subw));

  rule = crush_make_rule();
  crush_rule_add_step(rule, CRUSH_RULE_TAKE, root, 0);
  crush_rule_add_step(rule, CRUSH_RULE_CHOOSE_FIRSTN, 3, 1);
  crush_rule_add_step(rule, CRUSH_RULE_CHOOSE_FIRSTN, 1, 0);
  crush_rule_add_step(rule, CRUSH_RULE_EMIT, 0, 0);
  ruleno = crush_add_rule(map, -1, rule);

  printf("finalize\n");
  crush_finalize(map);
  printf("built\n");

  /* test */
  memset(o, 0, 100*sizeof(o[0]));
  for (i=0; i<10000; i++) {
    crush_do_rule(map, ruleno, i, r, 3, -1);
    /*printf("%d %d %d\n", r[0], r[1], r[2]);*/
    for (j=0; j<3; j++)
      o[r[j]]++;
  }

  for (i=0; i<100; i += 10)
    printf("%2d : %d\n", i, o[i]); 

  return 0;
}
