
/*[[[cog
import cogging as c
c.tpl(cog,templateFile,c.a(prefix=configFile))
]]]*/

#include "../include/mapBuffer.h"
/*[[[end]]] (checksum: c8758e660135eca590d877f4dbe24d53)*/


int testWrite(writer* w){
  void* res = w->writeNext(w, -1);
  if(res != NULL){
    *(unsigned*)res = 1;
    return w->writeFinished(w);
  }else{
    return -1;
  }
}

int testRead(reader* r){
  void* res = r->readNext(r, -1);
  if(res != NULL){
    BOOL rs = *(unsigned*)res == 1 && r->readFinished(r) == 0;
    return rs?0:-1;
  }else{
    return -1;
  }
}

void *writeKernel(void* inTarget){
  int cnt = 0;
  uint64_t nextTime = 0;
  while(TRUE){
    uint64_t curTime = curTimeMilisec();
    if(curTime>nextTime){
      printf("writer: %d\n",cnt);
      cnt = 0;
      nextTime = curTime + 1000L;
    }
    testWrite((writer*)inTarget);
    ++cnt;
  }
  return NULL;
}

void *readKernel(void* inTarget){
  int cnt = 0;
  uint64_t nextTime = 0;
  while(TRUE){
    uint64_t curTime = curTimeMilisec();
    if(curTime>nextTime){
      printf("reader: %d\n",cnt);
      cnt = 0;
      nextTime = curTime + 1000L;
    }
    testRead((reader*)inTarget);
    ++cnt;
  }
  return NULL;
}

int main(int argc, char* argv[]){
  arrayObject_create(arrBufs,unsigned,100)
  com_github_airutech_cnets_mapBuffer_create(classObj,arrBufs,1000,2)
  com_github_airutech_cnets_mapBuffer_createReader(classObjR0,&classObj,0)
  com_github_airutech_cnets_mapBuffer_createReader(classObjR1,&classObj,1)
  com_github_airutech_cnets_mapBuffer_createWriter(classObjW0,&classObj,0)
  com_github_airutech_cnets_mapBuffer_createWriter(classObjW1,&classObj,1)

  if(testWrite(&classObjW0) < 0){
    printf("testWrite: res < 0 should be 0\n");
    return -1;
  }
  printf("testWrite 1\n");

  if(testWrite(&classObjW1) < 0){
    printf("testWrite: res < 0 should be 0\n");
    return -1;
  }

  printf("testWrite 2\n");

  if(testRead(&classObjR0) < 0){
    printf("testRead: res < 0 should be 0\n");
    return -1;
  }
  printf("testRead 1\n");


  if(testRead(&classObjR0) < 0){
    printf("testRead: res < 0 should be 0\n");
    return -1;
  }
  printf("testRead 2\n");

  if(testRead(&classObjR1) < 0){
    printf("testRead: res < 0 should be 0\n");
    return -1;
  }
  printf("testRead 3\n");


  if(testRead(&classObjR1) < 0){
    printf("testRead: res < 0 should be 0\n");
    return -1;
  }
  printf("testRead 4\n");

  if(testRead(&classObjR0) >= 0){
    printf("testRead: res >= 0 should be < 0\n");
    return -1;
  }
  printf("testRead 5\n");
  pthread_t threadW0, threadW1, threadR0, threadR1;
  pthread_create(&threadW0, NULL, writeKernel, (void *)&classObjW0);
  pthread_create(&threadW1, NULL, writeKernel, (void *)&classObjW1);
  pthread_create(&threadR0, NULL, readKernel, (void *)&classObjR0);
  pthread_create(&threadR1, NULL, readKernel, (void *)&classObjR1);
  taskDelay(5000000000L);
  printf("test OK 6\n");

  // com_github_airutech_cnets_mapBuffer_create(classObj_test2,arrBufs,1000,1)
  // com_github_airutech_cnets_mapBuffer_createReader(classObj_test2R0,&classObj_test2,0)
  // com_github_airutech_cnets_mapBuffer_createWriter(classObj_test2W0,&classObj_test2,0)

  // pthread_t threadW0, threadR0;
  // pthread_create(&threadW0, NULL, writeKernel, (void *)&classObj_test2W0);
  // pthread_create(&threadR0, NULL, readKernel, (void *)&classObj_test2R0);
  // taskDelay(5000000000L);
  // printf("test OK 6\n");

  // com_github_airutech_cnets_mapBuffer_deinitialize(&classObj);
  return 0;
}