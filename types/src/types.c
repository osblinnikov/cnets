/*[[[cog
import cogging as c
c.tpl(cog,templateFile,c.a(prefix=configFile))
]]]*/

#include "../types.h"
void types_cnets_osblinnikov_github_com_onCreate(types_cnets_osblinnikov_github_com *that);
void types_cnets_osblinnikov_github_com_onDestroy(types_cnets_osblinnikov_github_com *that);
void types_cnets_osblinnikov_github_com_init(struct types_cnets_osblinnikov_github_com *that){
  
  types_cnets_osblinnikov_github_com_onCreate(that);
}

void types_cnets_osblinnikov_github_com_deinit(struct types_cnets_osblinnikov_github_com *that){
  types_cnets_osblinnikov_github_com_onDestroy(that);
}
/*[[[end]]] (checksum: b5a85543aee82267b3a23966e28c9f7b)*/

void types_cnets_osblinnikov_github_com_onCreate(types_cnets_osblinnikov_github_com *that){
  
  return;
}

void types_cnets_osblinnikov_github_com_onDestroy(types_cnets_osblinnikov_github_com *that){
  
  return;
}


void types_cnets_osblinnikov_github_com_onKernels(types_cnets_osblinnikov_github_com *that){
  
  return;
}

struct arrayObject arrayObjectNULL(){
  struct arrayObject res;
  res.array = NULL;
  res.length = 0;
  res.itemSize = 0;
  return res;
}

struct arrayObject arrayObject_init_dynamic(unsigned itemSize, unsigned capacity){
  struct arrayObject res;
  res.array = (void*)malloc(itemSize*capacity);
  if(res.array == NULL){
    printf("MALLOC FAILED %dx%d\n", itemSize, capacity);
    res.length = 0;
    res.itemSize = 0;
  }else{
    res.length = capacity;
    res.itemSize = itemSize;
  }
  return res;
}

void arrayObject_free_dynamic(struct arrayObject res){
  if(res.array != NULL){
    free(res.array);
  }
}