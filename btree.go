const HEADER = 4

const BTREE_PAGE_SIZE = 4096


/*

Node format

| type | nkeys |  pointers  |  offsets   | key-values | unused |
|  2b  |  2b   | nkeys * 8b | nkeys * 2b |    ...     |        |


pointers go from internal nodes to child nodes
offsets is a list of offsets to key value pairs and can be used to binary search key value pairs
key-values is a list of key-value pairs

key-value pair format

| klen | vlen | key | val |
|  2b  |  2b  | ... | ... |

*/

func init() {
    node1max := HEADER + 8 + 2 + 4 + BTREE_MAX_KEY_SIZE + BTREE_MAX_VAL_SIZE //number of bytes for node with one key, 8 is size of one pointer, 2 is 2 byte offset, 4 is len of key + len of val
    assert(node1max <= BTREE_PAGE_SIZE)
}

type BNode []byte

type BTree struct {
    root uint64
    get func(uint64) []byte //dereference pointer, read page from disk
    new func([]byte) uint64 //allocate new page and write (copy-on-write)
    del func(uint64)        //deallocate a page
}

const (
    BNODE_NODE = 1
    BNODE_LEAF = 2
)

//headers
func (node BNode) btype() uint16 {
    return binary.LittleEndian.Uint16(node[0:2]) //based on node format above
}

func (node BNode) nkeys() uint16 {
    return binary.LittleEndian.Uint16(node[2:4])
}

func (node BNode) setHeader(btype uint16, nkeys uint16) {
    binary.LittleEndian.PutUint16(node[0:2], btype)
    binary.LittleEndian.PutUint16(node[2:4], nkeys)
}

//pointers
func (node BNode) getPointer(idx uint16) uint64 { //pointer from internal node to leaf node
    assert(idx < node.nkeys())
    pos := HEADER + 8 * idx
    return binary.LittleEndian.Uint64(node[pos:])
}

func (node BNode) setPointer(idx uint16, val uint64) {
    pos := HEADER + 8 * idx
    binary.LittleEndian.PutUint64(node[pos:], val)
}

//offset list
func offsetPos(node BNode, idx uint16) uint16 { //get offset of key value pair at idx
    assert(1 <= idx && idx <= node.nkeys())
    return HEADER + 8 * node.nkeys() +  2 * (idx - 1)
}

func (node BNode) getOffset(idx uint16) uint16 { //get key value pair at idx
    if (idx == 0) {
        return 0
    }
    return binary.LittleEndian.Uint16(node[offsetPos(node, idx):])
}

func (node BNode) setOffset(idx uint16, offset uint16) {
    binary.LittleEndian.PutUint16(node[offsetPos(node, idx):], offset)
}

//key values
func (node BNode) kvPos(idx uint16) uint16 {
    assert(idx <= node.nkeys())
    return HEADER + 8*node.nkeys() + 2*node.nkeys() + node.getOffset(idx) //assume node is  a leaf node. hence pointers list contains pointers to key value pairs
}

func (node BNode) getKey(idx uint16) []byte {
    assert(idx <= node.nkeys())
    pos := node.kvPos(idx)
    klen := binary.LittleEndian.Uint16(node[pos:]) //based on format of key-value pair (see above)
    return node[pos+4:][:klen] //pos + 4 because key value pair starts with 4 bytes for klen and vlen
}

func (node BNode) getVal(idx uint16) []byte {
    assert(idx <= node.nkeys())
    pos := node.kvPos(idx) 
    klen := binary.LittleEndian.Uint16(node[pos:]) //based on format of key-value pair (see above)
    vlen := binary.LittleEndian.Uint16(node[pos + 2: ])
    return node[pos+4:][klen:klen + vlen] //pos + 4 because key value pair starts with 4 bytes for klen and vlen
}

func (node BNode) nbytes() uint16 { //return size of node
    return node.kvPos(node.nkeys())
}

//kv lookups within node

//return first child node whose range intersects key
func nodeLookupLE(node BNode, key []byte) uint16 { //LE because less than or equal operator
    nkeys := node.nkeys()
    found := uint16(0)
    for i := uint16(1); i < nkeys; i++ {
        cmp := bytes.Compare(key, node.getKey(i))
        if cmp <= 0 { //key <= node.getKey(i)
            found = i
        }
        if cmp >= 0 { //key should come right after found
            break
        }
    }
    return found
}

//Update leaf node
func leafInsert(new BNode, old BNode, idx uint16, key []byte, val []byte) {
    new.setHeader(BNODE_LEAF, old.nkeys() + 1)
    nodeAppendRange(new, old, 0, 0, idx)
    nodeAppendKV(new, idx, 0, key, val)
    nodeAppendRange(new, old, idx + 1, idx, old.nkeys() - idx)
}


func nodeAppendRange(new BNode, old BNode, dstNew uint16, srcOld uint16, n uint16) {
    for i := srcOld; i < n; i++ {
        nodeAppendKV(new, i, i, old.getKey(i), old.getVal(i)) //probably wrong
    }
}

//copy key value pair into position
func nodeAppendKV(new BNode, idx uint16, ptr uint64, key []byte, val []byte) {
    new.setPointer(idx, ptr)
    pos := new.kvPos(idx)
    binary.LittleEndian.PutUint16(new[pos:], uint16(len(key)))
    binary.LittleEndian.PutUint16(new[pos + 2:], uint16(len(val)))
    copy(new[pos + 4:], key)
    copy(new[pos + 4 + uint16(len(key)):], val)
    new.setOffset(idx + 1, new.getOffset(idx) + 4 + uint16(len(key) + len(val))) 
}


//Update internal nodes

func nodeReplaceChildN(tree *BTree, new BNode, old BNode, idx uint16, children ...BNode) {
    inc := uint16(len(children))
    new.setHeader(BNODE_NODE, old.nkeys()+inc-1)

    nodeAppendRange(new, old, 0, 0, idx)

    for i, node := range kids {
        nodeAppendRange(new, idx+uint16(i), tree.new(node), node.getKey(0), nil)
    }

    nodeAppendRange(new, old, idx + inc, idx + 1, old.nkeys() - (idx + 1))
}




