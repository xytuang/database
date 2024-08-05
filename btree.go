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
    binary.LittleEndian.PutUint64(node[pos:pos+8], val)
}

//offset list
func offsetPos(node BNode, idx uint16) uint16 { //get offset of key value pair at idx
    assert(1 <= idx && idx <= node.nkeys())
    return HEADER + 8 * node.nkeys() +  2 * (idx - 1)
}

func (node BNode) getOffset(idx uint16) uint16 { //get offset of idxth kv pair
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
    for i := 0; i < n; i++ {
        nodeAppendKV(new, dstNew + i, old.getPointer(srcOld + i), old.getKey(srcOld + i), old.getVal(srcOld + i)) //probably wrong
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



// split a oversized node into 2 so that the 2nd node always fits on a page
func nodeSplit2(left BNode, right BNode, old BNode) {
}

// split a node if it's too big. the results are 1~3 nodes.
func nodeSplit3(old BNode) (uint16, [3]BNode) {
    if old.nbytes() <= BTREE_PAGE_SIZE {
        old = old[:BTREE_PAGE_SIZE]
        return 1, [3]BNode{old} // not split
    }
    left := BNode(make([]byte, 2*BTREE_PAGE_SIZE)) // might be split later
    right := BNode(make([]byte, BTREE_PAGE_SIZE))
    nodeSplit2(left, right, old)
    if left.nbytes() <= BTREE_PAGE_SIZE {
        left = left[:BTREE_PAGE_SIZE]
        return 2, [3]BNode{left, right} // 2 nodes
    }
    leftleft := BNode(make([]byte, BTREE_PAGE_SIZE))
    middle := BNode(make([]byte, BTREE_PAGE_SIZE))
    nodeSplit2(leftleft, middle, left)
    assert(leftleft.nbytes() <= BTREE_PAGE_SIZE)
    return 3, [3]BNode{leftleft, middle, right} // 3 nodes
}

// insert a KV into a node, the result might be split.
// the caller is responsible for deallocating the input node
// and splitting and allocating result nodes.
func treeInsert(tree *BTree, node BNode, key []byte, val []byte) BNode {
    // the result node.
    // it's allowed to be bigger than 1 page and will be split if so
    new := BNode{data: make([]byte, 2*BTREE_PAGE_SIZE)}

    // where to insert the key?
    idx := nodeLookupLE(node, key)
    // act depending on the node type
    switch node.btype() {
    case BNODE_LEAF:
        // leaf, node.getKey(idx) <= key
        if bytes.Equal(key, node.getKey(idx)) {
            // found the key, update it.
            leafUpdate(new, node, idx, key, val)
        } else {
            // insert it after the position.
            leafInsert(new, node, idx+1, key, val)
        }
    case BNODE_NODE:
        // internal node, insert it to a kid node.
        nodeInsert(tree, new, node, idx, key, val)
    default:
        panic("bad node!")
    }
    return new
}

func nodeInsert(
    tree *BTree, new BNode, node BNode, idx uint16,
    key []byte, val []byte,
) {
    kptr := node.getPtr(idx)
    // recursive insertion to the kid node
    knode := treeInsert(tree, tree.get(kptr), key, val)
    // split the result
    nsplit, split := nodeSplit3(knode)
    // deallocate the kid node
    tree.del(kptr)
    // update the kid links
    nodeReplaceKidN(tree, new, node, idx, split[:nsplit]...)
}

// insert a new key or update an existing key
func (tree *BTree) Insert(key []byte, val []byte) {
    if tree.root == 0 {
        // create the first node
        root := BNode(make([]byte, BTREE_PAGE_SIZE))
        root.setHeader(BNODE_LEAF, 2)
        // a dummy key, this makes the tree cover the whole key space.
        // thus a lookup can always find a containing node.
        nodeAppendKV(root, 0, 0, nil, nil)
        nodeAppendKV(root, 1, 0, key, val)
        tree.root = tree.new(root)
        return
    }

    node := treeInsert(tree, tree.get(tree.root), key, val)
    nsplit, split := nodeSplit3(node)
    tree.del(tree.root)
    if nsplit > 1 {
        // the root was split, add a new level.
        root := BNode(make([]byte, BTREE_PAGE_SIZE))
        root.setHeader(BNODE_NODE, nsplit)
        for i, knode := range split[:nsplit] {
            ptr, key := tree.new(knode), knode.getKey(0)
            nodeAppendKV(root, uint16(i), ptr, key, nil)
        }
        tree.root = tree.new(root)
    } else {
        tree.root = tree.new(split[0])
    }
}
// delete a key and returns whether the key was there
func (tree *BTree) Delete(key []byte) bool

// remove a key from a leaf node
func leafDelete(new BNode, old BNode, idx uint16)
// merge 2 nodes into 1
func nodeMerge(new BNode, left BNode, right BNode)
// replace 2 adjacent links with 1
func nodeReplace2Kid(
    new BNode, old BNode, idx uint16, ptr uint64, key []byte,
)

func shouldMerge(
    tree *BTree, node BNode,
    idx uint16, updated BNode,
) (int, BNode) {
    if updated.nbytes() > BTREE_PAGE_SIZE/4 {
        return 0, BNode{}
    }

    if idx > 0 {
        sibling := BNode(tree.get(node.getPtr(idx - 1)))
        merged := sibling.nbytes() + updated.nbytes() - HEADER
        if merged <= BTREE_PAGE_SIZE {
            return -1, sibling // left
        }
    }
    if idx+1 < node.nkeys() {
        sibling := BNode(tree.get(node.getPtr(idx + 1)))
        merged := sibling.nbytes() + updated.nbytes() - HEADER
        if merged <= BTREE_PAGE_SIZE {
            return +1, sibling // right
        }
    }
    return 0, BNode{}
}

// delete a key from the tree
func treeDelete(tree *BTree, node BNode, key []byte) BNode

// delete a key from an internal node; part of the treeDelete()
func nodeDelete(tree *BTree, node BNode, idx uint16, key []byte) BNode {
    // recurse into the kid
    kptr := node.getPtr(idx)
    updated := treeDelete(tree, tree.get(kptr), key)
    if len(updated) == 0 {
        return BNode{} // not found
    }
    tree.del(kptr)

    new := BNode(make([]byte, BTREE_PAGE_SIZE))
    // check for merging
    mergeDir, sibling := shouldMerge(tree, node, idx, updated)
    switch {
    case mergeDir < 0: // left
        merged := BNode(make([]byte, BTREE_PAGE_SIZE))
        nodeMerge(merged, sibling, updated)
        tree.del(node.getPtr(idx - 1))
        nodeReplace2Kid(new, node, idx-1, tree.new(merged), merged.getKey(0))
    case mergeDir > 0: // right
        merged := BNode(make([]byte, BTREE_PAGE_SIZE))
        nodeMerge(merged, updated, sibling)
        tree.del(node.getPtr(idx + 1))
        nodeReplace2Kid(new, node, idx, tree.new(merged), merged.getKey(0))
    case mergeDir == 0 && updated.nkeys() == 0:
        assert(node.nkeys() == 1 && idx == 0) // 1 empty child but no sibling
        new.setHeader(BNODE_NODE, 0)          // the parent becomes empty too
    case mergeDir == 0 && updated.nkeys() > 0: // no merge
        nodeReplaceKidN(tree, new, node, idx, updated)
    }
    return new
}
