/*[[[cog
import cogging as c
c.tpl(cog,templateFile,c.a(prefix=configFile))
]]]*/

#include "../selector.h"

void* selector_cnets_osblinnikov_github_com_readNext(bufferKernelParams *params, int waitThreshold);
bufferReadData selector_cnets_osblinnikov_github_com_readNextWithMeta(bufferKernelParams *params, int waitThreshold);
int selector_cnets_osblinnikov_github_com_readFinished(bufferKernelParams *params);
void* selector_cnets_osblinnikov_github_com_writeNext(bufferKernelParams *params, int waitThreshold);
int selector_cnets_osblinnikov_github_com_writeFinished(bufferKernelParams *params);
int selector_cnets_osblinnikov_github_com_size(bufferKernelParams *params);
int64_t selector_cnets_osblinnikov_github_com_timeout(bufferKernelParams *params);
int selector_cnets_osblinnikov_github_com_gridSize(bufferKernelParams *params);
int selector_cnets_osblinnikov_github_com_uniqueId(bufferKernelParams *params);
int selector_cnets_osblinnikov_github_com_addSelector(bufferKernelParams *params, void* selectorContainer);
void selector_cnets_osblinnikov_github_com_onCreate(selector_cnets_osblinnikov_github_com *that);
void selector_cnets_osblinnikov_github_com_onDestroy(selector_cnets_osblinnikov_github_com *that);

reader selector_cnets_osblinnikov_github_com_createReader(selector_cnets_osblinnikov_github_com *that, int gridId){
  bufferKernelParams_create(params, that, gridId, NULL,selector_cnets_osblinnikov_github_com_)
  reader_create(res,params)
  return res;
}

writer selector_cnets_osblinnikov_github_com_createWriter(selector_cnets_osblinnikov_github_com *that, int gridId){
  bufferKernelParams_create(params, that, gridId, NULL,selector_cnets_osblinnikov_github_com_)
  writer_create(res,params)
  return res;
}

void selector_cnets_osblinnikov_github_com_init(struct selector_cnets_osblinnikov_github_com *that,
    arrayObject _reducableReaders){
  
  that->reducableReaders = _reducableReaders;
  selector_cnets_osblinnikov_github_com_onCreate(that);
}
}

void selector_cnets_osblinnikov_github_com_deinit(struct selector_cnets_osblinnikov_github_com *that){
  selector_cnets_osblinnikov_github_com_onDestroy(that);
}
/*[[[end]]] (checksum: 5db8a757ea9496c797b772ab75260ad3)*/

#include <assert.h>

void selector_cnets_osblinnikov_github_com_onCreate(selector_cnets_osblinnikov_github_com *that){


#undef selector_cnets_osblinnikov_github_com_onCreateMacro
#define selector_cnets_osblinnikov_github_com_onCreateMacro(_NAME_)\
    /* _allContainers_ */\
    linkedContainer* _NAME_##_allContainers_ = (linkedContainer*)salloca(sizeof(linkedContainer)*_NAME_.reducableReaders.length); \
    _NAME_.allContainers = _NAME_##_allContainers_;\
    /* _writesToContainers_ */\
    uint32_t* _NAME_##_writesToContainers_ = (uint32_t*)salloca(sizeof(uint32_t)*_NAME_.reducableReaders.length); \
    _NAME_.writesToContainers = _NAME_##_writesToContainers_;

  
  that->timeout_milisec = -1;
  that->lastReadId = -1;
  that->sumWrites = 0;
  int res;
  int i;
  res = pthread_mutex_init(&that->switch_cv_lock, NULL);
  assert(!res);
  res = pthread_cond_init (&that->switch_cv, NULL);
  assert(!res);
  for(i=0; i<(int)that->reducableReaders.length; i++){
    that->writesToContainers[i] = 0;
    reader* rdReaderItem = (reader*)&((char*)that->reducableReaders.array)[i * that->reducableReaders.itemSize];
    if(rdReaderItem == NULL){
      that->allContainers[i].add = NULL;
      continue;
    }
    linkedContainer_create(_linkedContainer_,selector_cnets_osblinnikov_github_com_getWriter(that, NULL, i))
    that->allContainers[i] = _linkedContainer_;
    if(0!=rdReaderItem->addSelector(rdReaderItem,&that->allContainers[i]) ) {
      printf("ERROR: selector_cnets_osblinnikov_github_com_onCreate addSelector failed\n");
    }
    int64_t to = rdReaderItem->timeout(rdReaderItem);
    if( to >= 0 && (that->timeout_milisec < 0 || that->timeout_milisec > to) ){
      that->timeout_milisec = to;
    }
  }
  return;
}

void selector_cnets_osblinnikov_github_com_onDestroy(selector_cnets_osblinnikov_github_com *that){
  int i;
  for(i=0; i<(int)that->reducableReaders.length; i++){
    if(that->allContainers[i].add != NULL) {
      that->allContainers[i].remove(&that->allContainers[i]);
    }
  }

  int res;
  res = pthread_mutex_destroy(&that->switch_cv_lock);
  assert(!res);
  res = pthread_cond_destroy (&that->switch_cv);
  assert(!res);
  return;
}

void* selector_cnets_osblinnikov_github_com_readNext(bufferKernelParams *params, int waitThreshold) {
  bufferReadData res = selector_cnets_osblinnikov_github_com_readNextWithMeta(params, waitThreshold);
  return res.data;
}

bufferReadData selector_cnets_osblinnikov_github_com_readNextWithMeta(bufferKernelParams *params, int waitThreshold) {
  bufferReadData res;
  res.data = NULL;
  if(params == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com readNextWithMeta: params is NULL\n");
    return res;
  }
  selector_cnets_osblinnikov_github_com *that = (selector_cnets_osblinnikov_github_com*)params->target;
  if(that == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com readNextWithMeta: Some Input parameters are wrong\n");
    return res;
  }
  BOOL r = FALSE;
  uint64_t nanosec = waitThreshold < 0? (uint64_t)that->timeout_milisec : (uint64_t)waitThreshold;
  struct timespec wait_timespec;
  wait_timespec = getTimespecDelay(nanosec*(uint64_t)1000000L);

  pthread_mutex_lock(&that->switch_cv_lock);
  if(that->sumWrites <= 0 && nanosec > 0){
    do{
      if(ETIMEDOUT == pthread_cond_timedwait(&that->switch_cv, &that->switch_cv_lock, &wait_timespec)){
        printf("ERROR: selector_cnets_osblinnikov_github_com_readNextWithMeta: wait timeout, params->grid_id='%d'\n",params->grid_id);
      }
      if(that->sumWrites > 0){
        break;
      }else if(!(r = compareTimespec(&wait_timespec)<0) ){
        pthread_mutex_unlock(&that->switch_cv_lock);
        return res;
      }
    }while(r);
  }
  do{
    ++that->lastReadId;
    if(that->lastReadId >= (int)that->reducableReaders.length){that->lastReadId = 0;}
  }while(that->writesToContainers[that->lastReadId] == 0);
  that->sumWrites--;
  that->writesToContainers[that->lastReadId]--;
  pthread_mutex_unlock(&that->switch_cv_lock);
  reader* rdReaderItem = (reader*)&((char*)that->reducableReaders.array)[that->lastReadId * that->reducableReaders.itemSize];
  res = rdReaderItem->readNextWithMeta(rdReaderItem,FALSE);
  if(res.data != NULL){
    res.nested_buffer_id = that->lastReadId;
    selector_cnets_osblinnikov_github_com_container *container = (selector_cnets_osblinnikov_github_com_container*)params->additionalData;
    if(container != NULL){
      container->bufferId = that->lastReadId;
    }
  }
  return res;
}

int selector_cnets_osblinnikov_github_com_readFinished(bufferKernelParams *params) {
  int res = -1;
  if(params == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com readFinished: params is NULL\n");
    return res;
  }
  selector_cnets_osblinnikov_github_com *that = (selector_cnets_osblinnikov_github_com*)params->target;
  if(that == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com readFinished: Some Input parameters are wrong\n");
    return res;
  }
  selector_cnets_osblinnikov_github_com_container *container = (selector_cnets_osblinnikov_github_com_container *)params->additionalData;
  if(container != NULL && container->bufferId >=0 && container->bufferId < (int)that->reducableReaders.length){
    reader* rdReaderItem = (reader*)&((char*)that->reducableReaders.array)[container->bufferId * that->reducableReaders.itemSize];
    res = rdReaderItem->readFinished(rdReaderItem);
    container->bufferId = -1;
  }else{
    printf("ERROR: selector_cnets_osblinnikov_github_com readFinished: some params are wrong\n");
  }
  return res;
}

void* selector_cnets_osblinnikov_github_com_writeNext(bufferKernelParams *params, int waitThreshold) {
  if(params == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com writeNext: params is NULL\n");
    return NULL;
  }
  selector_cnets_osblinnikov_github_com *that = (selector_cnets_osblinnikov_github_com*)params->target;
  void* res = NULL;
  if(that == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com writeNext: Some Input parameters are wrong\n");
    return res;
  }
  printf("ERROR: selector_cnets_osblinnikov_github_com writeNext is not allowed\n");
  return res;
}

int selector_cnets_osblinnikov_github_com_writeFinished(bufferKernelParams *params) {
  if(params == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com writeFinished: params is NULL\n");
    return -1;
  }
  selector_cnets_osblinnikov_github_com *that = (selector_cnets_osblinnikov_github_com*)params->target;
  if(that == NULL){
    printf("ERROR: selector writeFinished: Some Input parameters are wrong\n");
    return -1;
  };
  pthread_mutex_lock(&that->switch_cv_lock);
  ++that->sumWrites;
  ++that->writesToContainers[params->grid_id];
  pthread_cond_broadcast(&that->switch_cv);
  pthread_mutex_unlock(&that->switch_cv_lock);
  return 0;
}

int selector_cnets_osblinnikov_github_com_size(bufferKernelParams *params){
  if(params == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com size: params is NULL\n");
    return -1;
  }
  selector_cnets_osblinnikov_github_com *that = (selector_cnets_osblinnikov_github_com*)params->target;
  if(that == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com size: Some Input parameters are wrong\n");
    return -1;
  };
  selector_cnets_osblinnikov_github_com_container *container = (selector_cnets_osblinnikov_github_com_container *)params->additionalData;
  if(container == NULL || container->bufferId <0 || container->bufferId >= (int)that->reducableReaders.length){
    return -1;
  }
  reader* rdReaderItem = (reader*)&((char*)that->reducableReaders.array)[container->bufferId * that->reducableReaders.itemSize];
  return rdReaderItem->size(rdReaderItem);
}

int64_t selector_cnets_osblinnikov_github_com_timeout(bufferKernelParams *params){
  if(params == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com timeout: params is NULL\n");
    return -1;
  }
  selector_cnets_osblinnikov_github_com *that = (selector_cnets_osblinnikov_github_com*)params->target;
  if(that == NULL){
    printf("ERROR: selector_cnets_osblinnikov_github_com timeout: Some Input parameters are wrong\n");
    return -1;
  };
  return that->timeout_milisec;
}

int selector_cnets_osblinnikov_github_com_gridSize(bufferKernelParams *params){
  return 1;/*only one selector allowed to listen*/
}

int selector_cnets_osblinnikov_github_com_uniqueId(bufferKernelParams *params){
  return -1;/*it is unnecessary to have unique id*/
}

int selector_cnets_osblinnikov_github_com_addSelector(bufferKernelParams *params, void* selectorContainer) {
  printf("ERROR: selector_cnets_osblinnikov_github_com addSelector is not allowed: nested selectors look ambiguous\n");
  return -1;
}

void selector_cnets_osblinnikov_github_com_onKernels(selector_cnets_osblinnikov_github_com *that){
  /*do nothing*/
  return;
}