importScripts(
  '/dist/com_github_osblinnikov_cnets_readerWriter/readerWriter.js',
  '/dist/com_github_osblinnikov_cnets_types/types.js',
  '/dist/com_github_osblinnikov_cnets_mapBuffer/mapBuffer.js'
)

_this = new Dispatcher(this)

_this.onStart = ->
  console.log "onStart"

receivedData = null

onRun = {}
onRun.callback = (mapBufferId, mapBufferObj)->
  switch mapBufferId
    when 0
      r = readersWriters[mapBufferId].readNext()
      if r != null
        # console.log readersWriters[mapBufferId].getGridId()
        receivedData = r.obj
        # console.log "onRun _bufferToReadTest"
        readersWriters[mapBufferId].readFinished()
        # console.log "onRun _bufferToReadTest OK"
        onSend()
    when 1
      console.log "onRun _bufferToWriteTest"
    else
      console.log "onRun, unknown buffer"

onSend = ->
  if receivedData == null
    return
  r = readersWriters[1].writeNext()
  if r!=null
    r.obj = receivedData
    readersWriters[1].writeFinished()
    receivedData = null
  else
    setTimeout(onSend,100)

_this.onStop = ->
  console.log "onStop"

bId = 0
readersWriters = {}

_bufferToReadTest = new mapBuffer.create()
_bufferToReadTest.setDispatcher(bId, _this)
readersWriters[bId++] = _bufferToReadTest.getReader(onRun)

_bufferToWriteTest = new mapBuffer.create()
_bufferToWriteTest.setDispatcher(bId, _this)
readersWriters[bId++] = _bufferToWriteTest.getWriter()